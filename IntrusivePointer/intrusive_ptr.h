//
// Created by ZZK on 2024/5/16.
//

#pragma once

#include <atomic>
#include <utility>
#include <memory>
#include <type_traits>

#if defined(_DEBUG)
#include <iostream>
#include <format>
#define INTRUSIVE_DEBUG(...) std::cout << std::format(__VA_ARGS__) << '\n';
#else
#define INTRUSIVE_DEBUG(...)
#endif

namespace experimental
{
    struct SingleThreadCounter
    {
    public:
        void add_ref()
        {
            ++count;
            INTRUSIVE_DEBUG("Current counter: {}", count);
        }

        bool release()
        {
            --count;
            INTRUSIVE_DEBUG("Current counter: {}", count);
            return count == 0;
        }
    private:
        uint32_t count = 1;
    };

    struct MultiThreadCounter
    {
    public:
        MultiThreadCounter()
        {
            count.store(1, std::memory_order_relaxed);
            INTRUSIVE_DEBUG("Current counter: {}", count.load());
        }

        void add_ref()
        {
            count.fetch_add(1, std::memory_order_relaxed);
            INTRUSIVE_DEBUG("Current counter: {}", count.load());
        }

        bool release()
        {
            auto result = count.fetch_sub(1, std::memory_order_acq_rel);
            INTRUSIVE_DEBUG("Current counter: {}", result);
            return result == 1;
        }
    private:
        std::atomic<uint32_t> count;
    };

    template <typename T>
    struct IntrusivePtr;

    template <typename T>
    concept ReferenceOpsConcept = requires (T t)
    {
        t.add_ref();
        t.release();
    };

    template <typename T, typename Deleter = std::default_delete<T>, ReferenceOpsConcept ReferenceOps = SingleThreadCounter>
    struct IntrusivePtrEnabled
    {
    public:
        using IntrusivePtrType = IntrusivePtr<T>;
        using EnabledBase = T;
        using EnabledDeleter = Deleter;
        using EnabledReferenceOp = ReferenceOps;

        void release_reference()
        {
            if (reference_count.release())
            {
                Deleter{}(static_cast<T *>(this));
            }
        }

        void add_reference()
        {
            reference_count.add_ref();
        }

        IntrusivePtrEnabled() = default;

        IntrusivePtrEnabled(const IntrusivePtrEnabled &) = delete;

        IntrusivePtrEnabled &operator=(const IntrusivePtrEnabled &) = delete;

    protected:
        IntrusivePtr<T> reference_from_this();

    private:
        ReferenceOps reference_count;
    };

    template <typename T>
    struct IntrusivePtr
    {
    public:
        template <typename U>
        friend struct IntrusivePtr;

        IntrusivePtr() = default;

        explicit IntrusivePtr(T *handle)
        : data(handle)
        {

        }

        T &operator*()
        {
            return *data;
        }

        const T &operator*() const
        {
            return *data;
        }

        T *operator->()
        {
            return data;
        }

        const T *operator->() const
        {
            return data;
        }

        explicit operator bool() const
        {
            return data != nullptr;
        }

        bool operator==(const IntrusivePtr &other) const
        {
            return data == other.data;
        }

        bool operator!=(const IntrusivePtr &other) const
        {
            return data != other.data;
        }

        T *get()
        {
            return data;
        }

        const T *get() const
        {
            return data;
        }

        void reset()
        {
            using ReferenceBase = IntrusivePtrEnabled<
                    typename T::EnabledBase,
                    typename T::EnabledDeleter,
                    typename T::EnabledReferenceOp>;

            if (data != nullptr)
            {
                static_cast<ReferenceBase *>(data)->release_reference();
            }

            data = nullptr;
        }

        template <typename U>
        IntrusivePtr &operator=(const IntrusivePtr<U> &other)
        {
            static_assert(std::is_base_of_v<T, U>, "Cannot safely assign down cast intrusive pointers.");

            using ReferenceBase = IntrusivePtrEnabled<
                    typename T::EnabledBase,
                    typename T::EnabledDeleter,
                    typename T::EnabledReferenceOp>;

            reset();
            data = static_cast<T *>(other.data);

            if (data)
            {
                static_cast<ReferenceBase *>(data)->add_reference();
            }

            return *this;
        }

        IntrusivePtr &operator=(const IntrusivePtr &other)
        {
            using ReferenceBase = IntrusivePtrEnabled<
                    typename T::EnabledBase,
                    typename T::EnabledDeleter,
                    typename T::EnabledReferenceOp>;

            if (this != &other)
            {
                reset();
                data = other.data;

                if (data)
                {
                    static_cast<ReferenceBase *>(data)->add_reference();
                }
            }

            return *this;
        }

        template <typename U>
        IntrusivePtr(const IntrusivePtr<U> &other)
        {
            *this = other;
        }

        IntrusivePtr(const IntrusivePtr &other)
        {
            *this = other;
        }

        ~IntrusivePtr()
        {
            reset();
        }

        template <typename U>
        IntrusivePtr &operator=(IntrusivePtr<U> &&other) noexcept
        {
            reset();
            data = static_cast<T *>(other.data);
            other.data = nullptr;
            return *this;
        }

        IntrusivePtr &operator=(IntrusivePtr &&other) noexcept
        {
            if (this != &other)
            {
                reset();
                data = other.data;
                other.data = nullptr;
            }

            return *this;
        }

        template <typename U>
        IntrusivePtr(IntrusivePtr<U> &&other) noexcept
        {
            *this = std::move(other);
        }

        IntrusivePtr(IntrusivePtr &&other) noexcept
        {
            *this = std::move(other);
        }

        T *release() &
        {
            T *ret = data;
            data = nullptr;
            return ret;
        }

        T *release() &&
        {
            T *ret = data;
            data = nullptr;
            return ret;
        }

    private:
        T *data = nullptr;
    };

    template <typename T, typename Deleter, ReferenceOpsConcept ReferenceOps>
    IntrusivePtr<T> IntrusivePtrEnabled<T, Deleter, ReferenceOps>::reference_from_this()
    {
        add_reference();
        return IntrusivePtr<T>(static_cast<T *>(this));
    }

    template <typename Derived>
    using DerivedIntrusivePtrType = IntrusivePtr<Derived>;

    template <typename T, typename ... Args>
    DerivedIntrusivePtrType<T> make_handle(Args && ... args)
    {
        return DerivedIntrusivePtrType<T>(new T{ std::forward<Args>(args)... });
    }
}







































