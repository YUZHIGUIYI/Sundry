//
// Created by ZZK on 2024/6/29.
//

#pragma once

#include <type_traits>
#include <stdexcept>
#include <utility>
#include <string>

namespace reflect
{
#define REFLECT__PP_FOREACH_1(f, _1) f(_1)
#define REFLECT__PP_FOREACH_2(f, _1, _2) f(_1) f(_2)
#define REFLECT__PP_FOREACH_3(f, _1, _2, _3) f(_1) f(_2) f(_3)
#define REFLECT__PP_FOREACH_4(f, _1, _2, _3, _4) f(_1) f(_2) f(_3) f(_4)
#define REFLECT__PP_FOREACH_5(f, _1, _2, _3, _4, _5) f(_1) f(_2) f(_3) f(_4) f(_5)
#define REFLECT__PP_FOREACH_6(f, _1, _2, _3, _4, _5, _6) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6)
#define REFLECT__PP_FOREACH_7(f, _1, _2, _3, _4, _5, _6, _7) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7)
#define REFLECT__PP_FOREACH_8(f, _1, _2, _3, _4, _5, _6, _7, _8) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8)
#define REFLECT__PP_FOREACH_9(f, _1, _2, _3, _4, _5, _6, _7, _8, _9) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9)
#define REFLECT__PP_FOREACH_10(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10)
#define REFLECT__PP_FOREACH_11(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11)
#define REFLECT__PP_FOREACH_12(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12)
#define REFLECT__PP_FOREACH_13(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13)
#define REFLECT__PP_FOREACH_14(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14)
#define REFLECT__PP_FOREACH_15(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15)
#define REFLECT__PP_FOREACH_16(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16)
#define REFLECT__PP_FOREACH_17(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17)
#define REFLECT__PP_FOREACH_18(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18)
#define REFLECT__PP_FOREACH_19(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19)
#define REFLECT__PP_FOREACH_20(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20)
#define REFLECT__PP_FOREACH_21(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21)
#define REFLECT__PP_FOREACH_22(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22)
#define REFLECT__PP_FOREACH_23(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23)
#define REFLECT__PP_FOREACH_24(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24)
#define REFLECT__PP_FOREACH_25(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25)
#define REFLECT__PP_FOREACH_26(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26)
#define REFLECT__PP_FOREACH_27(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27)
#define REFLECT__PP_FOREACH_28(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) f(_28)
#define REFLECT__PP_FOREACH_29(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) f(_28) f(_29)
#define REFLECT__PP_FOREACH_30(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) f(_28) f(_29) f(_30)
#define REFLECT__PP_FOREACH_31(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) f(_28) f(_29) f(_30) f(_31)
#define REFLECT__PP_FOREACH_32(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10) f(_11) f(_12) f(_13) f(_14) f(_15) f(_16) f(_17) f(_18) f(_19) f(_20) f(_21) f(_22) f(_23) f(_24) f(_25) f(_26) f(_27) f(_28) f(_29) f(_30) f(_31) f(_32)
#define REFLECT__PP_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, N, ...) N
#define REFLECT__PP_NARGS(...) REFLECT__PP_NARGS_IMPL(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define REFLECT__PP_EXPAND_2(...) __VA_ARGS__
#define REFLECT__PP_EXPAND(...) REFLECT__PP_EXPAND_2(__VA_ARGS__)
#define REFLECT__PP_CONCAT_2(x, y) x##y
#define REFLECT__PP_CONCAT(x, y) REFLECT__PP_CONCAT_2(x, y)
#define REFLECT__PP_FOREACH(f, ...) REFLECT__PP_EXPAND(REFLECT__PP_CONCAT(REFLECT__PP_FOREACH_, REFLECT__PP_NARGS(__VA_ARGS__))(f, __VA_ARGS__))

    template <typename T>
    constexpr auto _has_member_test(int) -> decltype(T::template foreach_member_ptr<T>(nullptr), true)
    {
        return true;
    }

    template <typename T>
    constexpr bool _has_member_test(...)
    {
        return false;
    }

    template <typename T>
    struct reflect_trait
    {
        static constexpr bool has_member()
        {
            return reflect::_has_member_test<T>(0);
        }

        template <typename Func>
        static constexpr void foreach_member_ptr(Func &&func)
        {
            T::template foreach_member_ptr<T>(std::forward<Func>(func));
        }
    };

#define REFLECT__TYPE_PER_MEMBER_PTR(x) \
    func(#x, &This::x);

#define REFLECT_TYPE(Type, ...) \
template <> \
struct reflect::reflect_trait<Type> { \
    using This = Type; \
    static constexpr bool has_member() { return true; }; \
    template <typename Func> \
    static constexpr void foreach_member_ptr(Func &&func) { \
        REFLECT__PP_FOREACH(REFLECT__TYPE_PER_MEMBER_PTR, __VA_ARGS__) \
    } \
};

#define REFLECT__TYPE_TEMPLATED_FIRST(x, ...) REFLECT__PP_EXPAND(REFLECT__PP_EXPAND x)
#define REFLECT__TYPE_TEMPLATED_REST(x, ...) __VA_ARGS__

#define REFLECT_TYPE_TEMPLATED(Type, ...) \
template <REFLECT__PP_EXPAND(REFLECT__TYPE_TEMPLATED_REST Type)> \
struct reflect::reflect_trait<REFLECT__PP_EXPAND(REFLECT__TYPE_TEMPLATED_FIRST Type)> { \
    using This = REFLECT__PP_EXPAND(REFLECT__TYPE_TEMPLATED_FIRST Type); \
    static constexpr bool has_member() { return true; }; \
    template <class Func> \
    static constexpr void foreach_member_ptr(Func &&func) { \
        REFLECT__PP_FOREACH(REFLECT__TYPE_PER_MEMBER_PTR, __VA_ARGS__) \
    } \
};

#define REFLECT__PER_MEMBER(x) \
    func(#x, X);

#define REFLECT__PER_MEMBER_PTR(x) \
    func(#x, &This::x);

#define REFLECT(...) \
template <typename This, typename Func> \
static constexpr void foreach_member_ptr(Func &&func) \
{ \
    REFLECT__PP_FOREACH(REFLECT__PER_MEMBER_PTR, __VA_ARGS__) \
}

    enum class member_type
    {
        member_variable,
        member_function,
        static_variable,
        static_function,
    };

    template <class T>
    struct get_member_type;

    template <class T, class C>
    struct get_member_type<T C::*>
    {
        static constexpr member_type value = member_type::member_variable;
    };

    template <class T>
    struct get_member_type<T *>
    {
        static constexpr member_type value = member_type::static_variable;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...)>
    {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) const> {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) &> {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) const &>
    {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) &&>
    {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) const &&>
    {
        static constexpr member_type value = member_type::member_function;
    };

#if __cpp_noexcept_function_type
    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) noexcept>
    {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) const noexcept>
    {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) & noexcept>
    {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) const & noexcept>
    {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) && noexcept>
    {
        static constexpr member_type value = member_type::member_function;
    };

    template <class T, class C, class ...Ts>
    struct get_member_type<T (C::*)(Ts...) const && noexcept>
    {
        static constexpr member_type value = member_type::member_function;
    };
#endif

    template <typename T, typename ... Ts>
    struct get_member_type<T (*)(Ts...)>
    {
        static constexpr member_type value = member_type::static_function;
    };

#if __cpp_noexcept_function_type
    template <class T, class ...Ts>
    struct get_member_type<T (*)(Ts...) noexcept>
    {
        static constexpr member_type value = member_type::static_function;
    };
#endif

    template <class T, class Func>
    struct _foreach_visitor
    {
        T &&object;
        Func &&func;

        // For member variable
        template <class U>
        requires (get_member_type<U>::value == member_type::member_variable)
        constexpr void operator()(const char *name, U member) const
        {
            func(name, object.*member);
        }

        // For member function
        constexpr void operator()(...) const
        {

        }
    };

    template <class T>
    using _rmcvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

    template <class T, class Func>
    constexpr void foreach_member(T &&object, Func &&func)
    {
        _foreach_visitor<T, Func> visitor{ std::forward<T>(object), std::forward<Func>(func) };
        reflect_trait<_rmcvref_t<T>>::foreach_member_ptr(visitor);
    }

    template <class T, class Func>
    constexpr void foreach_member_ptr(Func &&func)
    {
        reflect_trait<_rmcvref_t<T>>::foreach_member_ptr(func);
    }

    template <class T>
    [[nodiscard]] constexpr bool has_member()
    {
        return reflect_trait<_rmcvref_t<T>>::has_member();
    }

    template <class MemberType, class T>
    [[nodiscard]] constexpr MemberType *try_get_member(T &&object, std::string const &name)
    {
        void const *ret = nullptr;
        reflect::foreach_member(object, [&] (const char *that_name, auto &member) {
            if (name == that_name && std::is_same<_rmcvref_t<MemberType>, _rmcvref_t<decltype(member)>>::value) {
                ret = &member;
            }
        });
        return static_cast<MemberType *>(const_cast<void *>(ret));
    }

    template <class MemberType, class T>
    [[nodiscard]] constexpr MemberType &get_member(T &&object, std::string const &name)
    {
        MemberType *ret = try_get_member<MemberType>(std::forward<T>(object), name);
        if (!ret) {
            throw std::invalid_argument("member name or type wrong");
        }
        return *ret;
    }

    template <class T>
    [[nodiscard]] constexpr bool has_member(std::string const &name)
    {
        bool ret = false;
        reflect::foreach_member_ptr<T>([&] (const char *that_name, auto) {
            if (name == that_name) {
                ret = true;
            }
        });
        return ret;
    }

    template <class T>
    [[nodiscard]] constexpr bool is_member_type(std::string const &name, member_type kind) {
        bool ret = false;
        reflect::foreach_member_ptr<T>([&] (const char *that_name, auto member) {
            if (name == that_name && get_member_type<decltype(member)>::value == kind) {
                ret = true;
            }
        });
        return ret;
    }

    template <class T, class MemberPtrType>
    [[nodiscard]] constexpr bool is_member_ptr_type(std::string const &name)
    {
        bool ret = false;
        reflect::foreach_member_ptr<T>([&] (const char *that_name, auto member) {
            if (name == that_name && std::is_same<decltype(member), MemberPtrType>::value) {
                ret = true;
            }
        });
        return ret;
    }

    template <class T, class MemberType>
    [[nodiscard]] constexpr bool is_member_type(std::string const &name)
    {
        bool ret = false;
        reflect::foreach_member_ptr<T>([&] (const char *that_name, auto member) {
            if (name == that_name && std::is_same<decltype(member), MemberType T::*>::value) {
                ret = true;
            }
        });
        return ret;
    }
};









































