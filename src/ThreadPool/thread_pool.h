//
// Created by ZZK on 2023/12/30.
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
#include <stdexcept>

namespace toy
{
    template<typename T>
    class SafeQueue
    {
    public:
        SafeQueue() = default;
        ~SafeQueue() = default;
        SafeQueue(const SafeQueue &) = delete;
        SafeQueue& operator=(const SafeQueue &) = delete;
        SafeQueue(SafeQueue &&) = delete;
        SafeQueue& operator=(SafeQueue &&) = delete;

        [[nodiscard]] bool empty() const
        {
            std::unique_lock<std::mutex> lock_guard(m_mutex);

            return m_queue.empty();
        }

        [[nodiscard]] size_t size() const
        {
            std::unique_lock<std::mutex> lock_guard(m_mutex);

            return m_queue.size();
        }

        void enqueue(T &value)
        {
            std::unique_lock<std::mutex> lock_guard(m_mutex);

            m_queue.emplace(value);
        }

        bool dequeue(T &value)
        {
            std::unique_lock<std::mutex> lock_guard(m_mutex);
            if (m_queue.empty())
            {
                return false;
            }

            value = std::move(m_queue.front());
            m_queue.pop();

            return true;
        }

    private:
        std::queue<T> m_queue;
        mutable std::mutex m_mutex;
    };


    class ThreadPool
    {
    public:
        explicit ThreadPool(size_t thread_num = 2)
        : m_shut_down{ false }
        {
            m_threads.reserve(thread_num);
            for (size_t i = 0; i < thread_num; ++i)
            {
                m_threads.emplace_back(ThreadWorker{ this, i } );
            }
        }

        ThreadPool(const ThreadPool &) = delete;
        ThreadPool& operator=(const ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;
        ThreadPool& operator=(ThreadPool &&) = delete;

        ~ThreadPool() noexcept
        {
            // Wait until threads have finished their current tasks, and then shut down thread pool
            m_shut_down.store(true, std::memory_order_release);

            // Notify all worker threads
            m_conditional_variable.notify_all();

            // Block, wait for all threads to finish executing
            for (auto&& each_thread : m_threads)
            {
                if (each_thread.joinable())
                {
                    each_thread.join();
                }
            }
        }

        // Submit a function to be executed asynchronously by thread pool
        template <typename F, typename ... Args>
        auto submit(F &&f, Args && ... args) -> std::future<std::invoke_result_t<F, Args ...>>
        {
            // Alias of type of return value of function
            using return_type = std::invoke_result_t<F, Args ...>;

            // Create a function with bounded parameter ready to execute
            std::function<return_type()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

            // Encapsulate it into a shared pointer in order to be able to copy construct
            std::shared_ptr<std::packaged_task<return_type()>> task = std::make_shared<std::packaged_task<return_type()>>(func);

            // Get future
            std::future<return_type> return_future = task->get_future();

            // Push wrap packaged task into queue
            {
                std::unique_lock<std::mutex> lock_guard(m_conditional_mutex);

                if (m_shut_down.load(std::memory_order_consume))
                {
                    throw std::runtime_error("Submit to a stopped thread pool");
                }

                m_safe_queue.emplace([wrapper_task = std::move(task)] () {
                    (*wrapper_task)();
                });
            }

            // Notify one thread
            m_conditional_variable.notify_one();

            // Return the previously registered task pointer
            return return_future;
        }

    private:
        std::queue<std::function<void()>> m_safe_queue;
        std::vector<std::thread> m_threads;
        std::mutex m_conditional_mutex;
        std::condition_variable m_conditional_variable;
        std::atomic<bool> m_shut_down;

        struct ThreadWorker
        {
            ThreadPool *m_thread_pool;
            size_t m_worker_id;

            explicit ThreadWorker(ThreadPool *thread_pool, size_t worker_id)
            : m_thread_pool(thread_pool), m_worker_id(worker_id)
            {

            }

            void operator()() const
            {
                while(true)
                {
                    std::function<void()> task = {};
                    {
                        std::unique_lock<std::mutex> lock_guard(m_thread_pool->m_conditional_mutex);

                        m_thread_pool->m_conditional_variable.wait(lock_guard, [this] () {
                            return m_thread_pool->m_shut_down.load(std::memory_order_consume) || !m_thread_pool->m_safe_queue.empty(); });

                        if (m_thread_pool->m_shut_down.load(std::memory_order_consume) && m_thread_pool->m_safe_queue.empty()) break;

                        // Pull out task
                        task = std::move(m_thread_pool->m_safe_queue.front());
                        m_thread_pool->m_safe_queue.pop();
                    }

                    // Execute task
                    if (task) task();
                }
            }
        };
    };
}































