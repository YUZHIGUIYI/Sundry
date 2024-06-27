//
// Created by ZZK on 2024/6/26.
//

#pragma once

#include <type_traits>
#include <functional>
#include <memory>
#include <vector>

namespace experimental
{
    enum class CallbackResult
    {
        Keep,
        Erase,
    };

    constexpr struct OneShot
    {
        explicit OneShot() = default;
    } oneshot;

    enum class NShot : size_t { };

    namespace details
    {
        template <typename Self>
        std::shared_ptr<Self> lock_if_weak(const std::weak_ptr<Self> &self)
        {
            return self.lock();
        }

        template <typename Self>
        const Self &lock_if_weak(const Self &self)
        {
            return self;
        }

        template <typename Self, typename MemFn>
        auto bind(Self self, MemFn mem_fn, OneShot)
        {
            return [self = std::move(self), mem_fn] (auto ... args) {
                const auto &ptr = lock_if_weak(self);
                if (ptr != nullptr)
                {
                    ((*ptr).*mem_fn)(args ...);
                }
                return CallbackResult::Erase;
            };
        }

        template <typename Self, typename MemFn>
        auto bind(Self self, MemFn mem_fn, NShot n)
        {
            return [self = std::move(self), mem_fn, n = static_cast<size_t>(n)] (auto ... args) mutable {
                if (n == 0)
                {
                    return CallbackResult::Erase;
                }
                const auto &ptr = lock_if_weak(self);
                if (ptr == nullptr)
                {
                    return CallbackResult::Erase;
                }
                ((*ptr).*mem_fn)(args ...);
                --n;
                if (n == 0)
                {
                    return CallbackResult::Erase;
                }
                return CallbackResult::Keep;
            };
        }

        template <typename Self, typename MemFn>
        auto bind(Self self, MemFn mem_fn)
        {
            return [self = std::move(self), mem_fn] (auto ... args) {
                const auto &ptr = lock_if_weak(self);
                if (ptr == nullptr)
                {
                    return CallbackResult::Erase;
                }
                ((*ptr).*mem_fn)(args ...);
                return CallbackResult::Keep;
            };
        }
    }

    template <typename ... Args>
    struct Signal
    {
#if __cpp_lib_move_only_function
        using Functor = std::move_only_function<CallbackResult(Args...)>;
#else
        using Functor = std::function<CallbackResult(Args...)>;
#endif
    private:
        std::vector<Functor> m_callbacks;

    public:
#if __cpp_if_constexpr
        template <typename Func>
        void connect(Func callback)
        {
            if constexpr (std::is_invocable_r_v<CallbackResult, Func, Args...>)
            {
                m_callbacks.push_back(std::move(callback));
            } else
            {
                m_callbacks.push_back([callback = std::move(callback)] (Args ... args) {
                    callback(std::forward<Args>(args)...);
                    return CallbackResult::Erase;
                });
            }
        }
#else
        template <typename Func, typename std::enable_if<std::is_convertible<decltype(std::declval<Func>()(std::declval<Args>()...)), CallbackResult>::value, int>::type = 0>
        void connect(Func callback)
        {
            m_callbacks.push_back(std::move(callback));
        }

        template <typename Func, typename std::enable_if<std::is_void<decltype(std::declval<Func>()(std::declval<Args>()...))>::value, int>::type = 0>
        void connect(Func callback)
        {
            m_callbacks.push_back([callback = std::move(callback)] (Args ... args) mutable {
                callback(std::forward<Args>(args)...);
                return CallbackResult::Keep;
            });
        }
#endif

        template <typename Self, typename MemFn, typename ... Tag>
        void connect(Self self, MemFn mem_fn, Tag ... tag)
        {
            m_callbacks.push_back(details::bind(std::move(self), mem_fn, tag...));
        };

        void emit(Args ... args)
        {
            for (auto it = m_callbacks.begin(); it != m_callbacks.end();)
            {
                CallbackResult result = (*it)(std::forward<Args>(args)...);
                switch (result)
                {
                    case CallbackResult::Keep:
                        ++it;
                        break;
                    case CallbackResult::Erase:
                        it = m_callbacks.erase(it);
                        break;
                }
            }
        }
    };

#if __cplusplus >= 202002L && !(defined(_MSV_VER) && (!defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL))
#define SIG_FUNC(_func, ...) [=] (auto&& ... args) { return _func(__VA_ARGS__ __VA_OPT__(,) std::forward<decltype(args)>(args)...); }
#else
#define SIG_FUNC(_func) [=] (auto&& ... args) { return _func(std::forward<decltype(args)>(args)...); }
#endif
}


























