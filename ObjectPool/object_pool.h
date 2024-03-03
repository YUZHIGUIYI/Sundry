//
// Created by ZZK on 2024/3/3.
//

#pragma once

#include <cstddef>
#include <memory>
#include <cstdint>
#include <utility>
#include <vector>
#include <mutex>
#include <concepts>

#if !defined (_WIN32) || !defined(_WIN64)
#error "Requires windows platform"
#else
#include <corecrt_malloc.h>
#endif

namespace util::details
{
    struct NullMutex
    {
        void lock() const {}
        void unlock() const {}
    };

    template <typename Mutex>
    concept MutexConcept = requires (Mutex mutex)
    {
        { mutex.lock() }   -> std::same_as<void>;
        { mutex.unlock() } -> std::same_as<void>;
    };

    inline void* memalign_alloc(size_t boundary, size_t size)
    {
        return _aligned_malloc(size, boundary);
    }

    inline void memalign_free(void *ptr)
    {
        _aligned_free(ptr);
    }

    template <typename T>
    struct ObjectPool
    {
    public:
        virtual ~ObjectPool() = default;

        template <typename ... Args>
        T* allocate(Args&& ... args)
        {
            if (m_vacants.empty())
            {
                uint32_t num_objects = 64u << m_memory.size();
                T* ptr = static_cast<T *>(memalign_alloc(std::max<size_t>(64, alignof(T)),
                                                            num_objects * sizeof(T)));

                if (!ptr) return nullptr;

                for (uint32_t i = 0; i < num_objects; ++i)
                {
                    m_vacants.push_back(&ptr[i]);
                }

                m_memory.emplace_back(ptr);
            }

            T* ptr = m_vacants.back();
            m_vacants.pop_back();
            new (ptr) T(std::forward<Args>(args)...);
            return ptr;
        }

        void free(T *ptr)
        {
            ptr->~T();
            m_vacants.push_back(ptr);
        }

        void clear()
        {
            m_vacants.clear();
            m_memory.clear();
        }

    private:
        std::vector<T *> m_vacants;
        struct MallocDeleter
        {
            void operator()(T* ptr) const
            {
                memalign_free(ptr);
            }
        };
        std::vector<std::unique_ptr<T, MallocDeleter>> m_memory;
    };

    template <typename T, MutexConcept Mutex>
    struct UserDefinedObjectPool final : public ObjectPool<T>
    {
    public:
        ~UserDefinedObjectPool() override = default;

        template <typename ... Args>
        T* allocate(Args&& ... args)
        {
            std::lock_guard<Mutex> lock_guard{ m_mutex };
            return ObjectPool<T>::allocate(std::forward<Args>(args)...);
        }

        void free(T* ptr)
        {
            std::lock_guard<Mutex> lock_guard{ m_mutex };
            ObjectPool<T>::free(ptr);
        }

        void clear()
        {
            std::lock_guard<Mutex> lock_guard{ m_mutex };
            ObjectPool<T>::clear();
        }

    private:
        Mutex m_mutex;
    };
}

namespace util
{
    template <typename T>
    using ThreadSafeObjectPool = details::UserDefinedObjectPool<T, std::mutex>;

    template <typename T>
    using UnsafeObjectPool = details::UserDefinedObjectPool<T, details::NullMutex>;
}
