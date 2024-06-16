//
// Created by ZZK on 2023/12/31.
//

#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <future>
#include <queue>
#include <vector>
#include <atomic>
#include <random>
#include <cassert>

#if defined(_DEBUG)
#include <iostream>
#include <format>
#endif

namespace experimental
{
    template <typename T>
    class SafeQueue
    {
    public:
        void push(const T &item)
        {
            {
                std::lock_guard<std::mutex> lock_guard(m_mutex);
                m_queue.push(item);
            }
            m_conditional_variable.notify_one();
        }

        void push(T &&item)
        {
            {
                std::lock_guard<std::mutex> lock_guard(m_mutex);
                m_queue.push(std::move(item));
            }
            m_conditional_variable.notify_one();
        }

        template <typename U>
        requires std::is_same_v<T, U>
        void push(U &&item)
        {
            {
                std::lock_guard<std::mutex> lock_guard(m_mutex);
                m_queue.push(std::forward<U>(item));
            }
            m_conditional_variable.notify_one();
        }

        bool pop(T &item)
        {
            std::unique_lock<std::mutex> lock_guard(m_mutex);
            m_conditional_variable.wait(lock_guard, [this] () {
                return !m_queue.empty() || m_stop.load(std::memory_order_consume);
            });
            if (m_queue.empty()) return false;

            item = std::move(m_queue.front());
            m_queue.pop();
            return true;
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock_guard(m_mutex);
            return m_queue.size();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock_guard(m_mutex);
            return m_queue.empty();
        }

        void stop()
        {
            m_stop.store(true, std::memory_order_release);
            m_conditional_variable.notify_all();
        }

    private:
        std::condition_variable m_conditional_variable;
        mutable std::mutex m_mutex;
        std::queue<T> m_queue;
        std::atomic<bool> m_stop = false;
    };

    class MultiThreadPool
    {
    public:
        using WorkItem = std::function<void()>;

        explicit MultiThreadPool(size_t thread_num = std::thread::hardware_concurrency())
        : m_queues{ std::vector<SafeQueue<WorkItem>>(thread_num) },
        m_thread_num{ thread_num }
        {
            auto worker = [this] (size_t worker_id) {
                while (true)
                {
                    WorkItem task = {};

                    if (!m_queues[worker_id].pop(task))
                    {
#if defined(_DEBUG)
                        std::cout << std::format("Thread {} has exited\n", worker_id);
#endif
                        break;
                    }

                    if (task) {
#if defined(_DEBUG)
                        std::cout << std::format("Completed by thread {}\n", worker_id);
#endif
                        task();
                    }
                }
            };

            m_workers.reserve(thread_num);
            for (size_t i = 0; i < thread_num; ++i)
            {
                m_workers.emplace_back(worker, i);
            }
        }

        MultiThreadPool(const MultiThreadPool &) = delete;
        MultiThreadPool& operator=(const MultiThreadPool &) = delete;
        MultiThreadPool(MultiThreadPool &&) = delete;
        MultiThreadPool& operator=(MultiThreadPool &&) = delete;

        bool schedule_by_id(WorkItem work, size_t id = std::numeric_limits<size_t>::max())
        {
            if (!work) return false;

            if (id == std::numeric_limits<size_t>::max())
            {
                static std::random_device rd;
                static std::mt19937 mt{ rd() };
                static std::uniform_int_distribution<size_t> uniform_dist{ 0, m_thread_num - 1 };

                auto random_id = uniform_dist(mt);
                m_queues[random_id].push(std::move(work));
            } else
            {
                assert(id < m_thread_num);
                m_queues[id].push(std::move(work));
            }

            return true;
        }

        ~MultiThreadPool() noexcept
        {
            for (auto&& queue : m_queues)
            {
                queue.stop();
            }
            for (auto&& worker : m_workers)
            {
                worker.join();
            }
        }

    private:
        std::vector<SafeQueue<WorkItem>> m_queues;
        std::vector<std::thread> m_workers;
        size_t m_thread_num;
    };
}






















