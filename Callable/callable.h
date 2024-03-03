//
// Created by ZZK on 2024/3/3.
//

#pragma once

#include <functional>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace util
{
    template <typename Sig, size_t PayloadSize = 32, size_t Align = 8>
    struct SmallCallable;

    template <typename R, typename ... Args, size_t PayloadSize, size_t Align>
    struct SmallCallable<R (Args ...), PayloadSize, Align>
    {
    public:
        R call(Args ... args)
        {
            return get_invokable().call(std::forward<Args>(args)...);
        }

        explicit operator bool() const
        {
            return get_invokable().active();
        }

        ~SmallCallable()
        {
            get_invokable().~Invokable();
        }

        template <typename Func>
        explicit SmallCallable(Func&& func)
        {
            using PlainFunc = std::remove_cvref_t<Func>;
            // Avoid mistaken double wrap
            static_assert(!std::is_same_v<std::function<R(Args...)>, PlainFunc>, "Do not support std::function");
            static_assert(sizeof(CapturedInvokable<PlainFunc>) <= sizeof(m_payload), "Callable payload is too large");
            new (m_payload) CapturedInvokable<PlainFunc>(std::forward<PlainFunc>(func));
        }

        explicit SmallCallable(R (*fn)(Args...))
        {
            static_assert(sizeof(CapturedPlain) <= sizeof(m_payload), "Callable payload is too large");
            new (m_payload) CapturedPlain(fn);
        }

        // Non-const member function
        template <typename T>
        explicit SmallCallable(T* ptr, R (T::*mem_fn)(Args...))
        {
            using MemberFunc = CapturedMemberFunc<T>;
            static_assert(sizeof(MemberFunc) <= sizeof(m_payload), "Callable payload is too large");
            new (m_payload) MemberFunc(ptr, mem_fn);
        }

        SmallCallable()
        {
            new (m_payload) NullInvoker();
        }


        // Prohibit copy constructor and move constructor
        SmallCallable(const SmallCallable &) = delete;
        SmallCallable& operator=(const SmallCallable &) = delete;
        SmallCallable(SmallCallable &&) = delete;
        SmallCallable& operator=(SmallCallable &&) = delete;

    private:
        struct Invokable
        {
            virtual ~Invokable() = default;
            virtual R call(Args ... args) = 0;
            [[nodiscard]] virtual bool active() const = 0;
        };

        template <typename I>
        struct CapturedInvokable final : Invokable
        {
            explicit CapturedInvokable(I&& holder_) : holder(std::move(holder_)) {}
            R call(Args ... args) override { return holder(std::forward<Args>(args)...); }
            [[nodiscard]] bool active() const override { return true; }

            I holder;
        };

        struct NullInvoker final : Invokable
        {
            R call(Args ... args) override { return R{}; }
            [[nodiscard]] bool active() const override { return false; }
        };

        struct CapturedPlain final : Invokable
        {
            explicit CapturedPlain(R (*func_)(Args...)) : func(func_) {}
            R call(Args ... args) override { return func(std::forward<Args>(args)...); }
            [[nodiscard]] bool active() const override { return func != nullptr; }

            R (*func)(Args...);
        };

        template <typename T>
        struct CapturedMemberFunc final : Invokable
        {
            explicit CapturedMemberFunc(T* ptr_, R (T::*func_)(Args...)) : ptr(ptr_), func(func_) {}
            R call(Args ... args) override { return (ptr->*func)(std::forward<Args>(args)...); }
            [[nodiscard]] bool active() const override { return ptr != nullptr && func != nullptr; }
            T* ptr;
            R (T::*func)(Args...);
        };

        Invokable& get_invokable()
        {
            return *reinterpret_cast<Invokable *>(&m_payload[0]);
        }

        const Invokable& get_invokable() const
        {
            return *reinterpret_cast<const Invokable *>(&m_payload[0]);
        }

    private:
        alignas(Align) uint8_t m_payload[PayloadSize];
    };
}