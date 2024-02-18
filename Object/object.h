//
// Created by ZZK on 2024/2/18.
//

#pragma once

#include <array>
#include <functional>
#include <string_view>
#include <unordered_map>
#include <span>
#include <tuple>
#include <stdexcept>

namespace experimental
{
    struct Type;

    struct Any
    {
    private:
        Type* type;     // Pointer to type information
        void* data;     // Pointer to actual data
        uint8_t flag;   // Special flag for destructor function

    public:
        Any() : type(nullptr), data(nullptr), flag(0) {}
        Any(Type* in_type, void* in_data) : type(in_type), data(in_data), flag(0B00000001) {}
        Any(const Any& other);
        Any(Any&& other) noexcept;

        template <typename T>
        Any(T&& value);

        ~Any();

        template <typename T>
        T& cast();

        // Get type information
        [[nodiscard]] Type* get_type() const { return type; }

        // Invoke member functions
        Any invoke(std::string_view name, std::span<Any> args);

        // Iterate member fields
        void foreach(const std::function<void(std::string_view, Any)>& fn);
    };

    struct Type
    {
        using Destroy = void (*)(void *);
        using Construct = void* (*)(void *);
        using Method = Any (*)(void *, std::span<Any>);
        using Field = std::pair<Type *, size_t>;

        std::string_view name;                                  // Type name
        Destroy destroy_fn;                                     // Destructor function
        Construct copy_fn;                                      // Copy constructor function
        Construct move_fn;                                      // Move constructor function
        std::unordered_map<std::string_view, Field> fields;     // Field information - type and offset
        std::unordered_map<std::string_view, Method> methods;   // Method information - function address
    };

    template <typename T>
    struct member_fn_traits
    {

    };

    // Non-const member function
    template <typename R, typename C, typename ... Args>
    struct member_fn_traits<R (C::*)(Args ...)>
    {
        using return_type = R;
        using class_type = C;
        using args_type = std::tuple<Args ...>;
    };

    // Const member function
    template <typename R, typename C, typename ... Args>
    struct member_fn_traits<R (C::*)(Args ...) const>
    {
        using return_type = R;
        using class_type = C;
        using args_type = std::tuple<Args ...>;
    };

    template <auto FnPtr>
    auto* type_ensure()
    {
        using traits = member_fn_traits<decltype(FnPtr)>;
        using class_type = typename traits::class_type;
        using result_type = typename traits::return_type;
        using args_type = typename traits::args_type;

        return +[](void* object, std::span<Any> args) -> Any
        {
            auto self = static_cast<class_type *>(object);
            return [=] <size_t ... Is> (std::index_sequence<Is ...>)
            {
                if constexpr (std::is_void_v<result_type>)
                {
                    (self->*FnPtr)(args[Is].cast<std::tuple_element_t<Is, args_type>>()...);
                    return Any{};
                } else
                {
                    auto result = (self->*FnPtr)(args[Is].cast<std::tuple_element_t<Is, args_type>>()...);
                    return Any{ result };
                }
            }(std::make_index_sequence<std::tuple_size_v<args_type>>{});
        };
    }

    template <typename T>
    Type* type_of()
    {
        static Type type;
        type.name = typeid(T).name();
        type.destroy_fn = [] (void* obj) { delete static_cast<T *>(obj); };
        type.copy_fn = [] (void* obj) { return (void*)(new T(*static_cast<T *>(obj))); };
        type.move_fn = [] (void* obj) { return (void*)(new T(std::move(*static_cast<T *>(obj)))); };
        return &type;
    }

    template <typename T>
    T& Any::cast()
    {
        if (type != type_of<T>())
        {
            throw std::runtime_error("Type mismatch");
        }
        return *static_cast<T *>(data);
    }

    inline Any::Any(const Any &other)
    {
        type = other.type;
        data = type->copy_fn(other.data);
        flag = 0;
    }

    inline Any::Any(Any &&other) noexcept
    {
        type = other.type;
        data = type->move_fn(other.data);
        flag = 0;
    }

    template <typename T>
    Any::Any(T &&value)
    {
        type = type_of<std::decay_t<T>>();
        data = new std::decay_t<T>(std::forward<T>(value));
        flag = 0;
    }

    inline Any::~Any()
    {
        if (!(flag & 0B00000001))
        {
            if (data && type)
            {
                type->destroy_fn(data);
            }
        }
    }

    inline void Any::foreach(const std::function<void(std::string_view, Any)> &fn)
    {
        for (auto& [name, field] : type->fields)
        {
            fn(name, Any{ field.first, static_cast<char *>(data) + field.second });
        }
    }

    inline Any Any::invoke(std::string_view name, std::span<Any> args)
    {
        auto it = type->methods.find(name);
        if (it == type->methods.end())
        {
            throw std::runtime_error("Method not found");
        }
        return it->second(data, args);
    }
}
