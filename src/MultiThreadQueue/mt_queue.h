//
// Created by ZZK on 2024/6/16.
//

#pragma once

#include <deque>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <chrono>

namespace experimental
{
    template <typename T, typename Deque = std::deque<T>>
    struct MTQueue
    {
    private:
        Deque m_queue;
        std::mutex m_mutex;
        std::condition_variable m_cv_empty;
        std::condition_variable m_cv_full;
        std::size_t m_limit;

    public:
        MTQueue() : m_limit(static_cast<std::size_t>(-1)) {}

        explicit MTQueue(std::size_t limit) : m_limit(limit) {}

        // Push data/task, blocked if full
        void push(T value)
        {
            std::unique_lock lock{ m_mutex };

            m_cv_full.wait(lock, [this] { return m_queue.size() < m_limit; });

            m_queue.push_front(std::move(value));
            m_cv_empty.notify_one();
        }

        // Push data/task, return if full
        bool try_push(T value)
        {
            std::unique_lock lock{ m_mutex };

            if (m_queue.size() >= m_limit)
            {
                return false;
            }

            m_queue.push_front(std::move(value));
            m_cv_empty.notify_one();
            return true;
        }

        // Push data/task, wait for a while if full; if still full, return false
        bool try_push_for(T value, std::chrono::steady_clock::duration timeout)
        {
            std::unique_lock lock{ m_mutex };

            if (!m_cv_full.wait_for(lock, timeout, [this] { return m_queue.size() < m_limit; }))
            {
                return false;
            }

            m_queue.push_front(std::move(value));
            m_cv_empty.notify_one();
            return true;
        }

        // Push data/task, wait until time point if full; if still fill, return false
        bool try_push_until(T value, std::chrono::steady_clock::time_point time_point)
        {
            std::unique_lock lock{ m_mutex };

            if (!m_cv_full.wait_until(lock, time_point, [this] { return m_queue.size() < m_limit; }))
            {
                return false;
            }

            m_queue.push_front(std::move(value));
            m_cv_empty.notify_one();
            return true;
        }

        // Blocked until no empty
        T pop()
        {
            std::unique_lock lock{ m_mutex };

            m_cv_empty.wait(lock, [this] { return !m_queue.empty(); });

            T value = std::move(m_queue.back());
            m_queue.pop_back();
            m_cv_full.notify_one();
            return value;
        }

        // Obtain data/task, return nullopt if empty
        std::optional<T> try_pop()
        {
            std::unique_lock lock{ m_mutex };

            if (m_queue.empty())
            {
                return std::nullopt;
            }

            T value = std::move(m_queue.back());
            m_queue.pop_back();
            m_cv_full.notify_one();
            return value;
        }

        // Obtain data/task, wait for a while if empty; if still empty, return nullopt
        std::optional<T> try_pop_for(std::chrono::steady_clock::duration timeout)
        {
            std::unique_lock lock{ m_mutex };

            if (!m_cv_empty.wait_for(lock, timeout, [this] { return !m_queue.empty(); }))
            {
                return std::nullopt;
            }

            T value = std::move(m_queue.back());
            m_queue.pop_back();
            m_cv_full.notify_one();
            return value;
        }

        // Obtain data/task, wait until time point if empty; if still empty, return nullopt
        std::optional<T> try_pop_until(std::chrono::steady_clock::time_point time_point)
        {
            std::unique_lock lock{ m_mutex };

            if (!m_cv_empty.wait_until(lock, time_point, [this] { !m_queue.empty(); }))
            {
                return std::nullopt;
            }

            T value = std::move(m_queue.back());
            m_queue.pop_back();
            m_cv_full.notify_one();
            return value;
        }
    };
}













































