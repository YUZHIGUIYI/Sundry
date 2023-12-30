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
        explicit ThreadPool(uint32_t thread_num = 2)
        : m_threads{ std::vector<std::thread>(thread_num) }, m_shut_down{ false }
        {

        }

        // Initialize thread pool
        void init()
        {
            for (size_t i = 0; i < m_threads.size(); ++i)
            {
                m_threads[i] = std::thread{ ThreadWorker{ this, i } };
            }
        }

        // Wait until threads have finished their current tasks, shut down thread pool
        void shut_down()
        {
            m_shut_down.store(true, std::memory_order_release);

            // Notify all worker threads
            m_conditional_variable.notify_all();

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
        auto submit(F &&f, Args && ... args) -> std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
        {
            // Alias of type of return value of function
            using return_type = decltype(std::forward<F>(f)(std::forward<Args>(args)...));

            // Create a function with bounded parameter ready to execute
            std::function<return_type()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

            // Encapsulate it into a shared pointer in order to be able to copy construct
            std::shared_ptr<std::packaged_task<return_type()>> task_ptr = std::make_shared<std::packaged_task<return_type()>>(func);

            // Wrap packaged task into a void-function
            std::function<void()> wrapper_func = [task_ptr] () {
                (*task_ptr)();
            };

            // Push to safe queue
            m_safe_queue.enqueue(wrapper_func);

            // Notify one thread
            m_conditional_variable.notify_one();

            // Return the previously registered task pointer
            return task_ptr->get_future();
        }

    private:
        SafeQueue<std::function<void()>> m_safe_queue;
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
                std::function<void()> target_func = {};
                bool has_obtained = false;

                while(!m_thread_pool->m_shut_down.load(std::memory_order_consume))
                {
                    {
                        std::unique_lock<std::mutex> lock_guard(m_thread_pool->m_conditional_mutex);

                        while (m_thread_pool->m_safe_queue.empty())
                        {
                            m_thread_pool->m_conditional_variable.wait(lock_guard);
                            if (m_thread_pool->m_shut_down.load(std::memory_order_consume)) break;
                        }

                        has_obtained = m_thread_pool->m_safe_queue.dequeue(target_func);
                    }

                    if (has_obtained) target_func();
                }
            }
        };
    };
}































