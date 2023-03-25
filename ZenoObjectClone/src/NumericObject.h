#include <type_traits>
#include <variant>
#include <exception>
#include <IObject.h>

using NumericValue = std::variant<int, float, double, size_t>;

struct NumericObject : IObjectClone<NumericObject>
{
    NumericValue value;

    NumericObject() = default;
    NumericObject(NumericValue const &value) : value(value) {}

    NumericValue& get() 
    {
        return value;
    }

    NumericValue const& get() const
    {
        return value;
    }

    template <class T>
    T get() const
    {
        return std::visit([] (auto const &val) -> T {
            using V = std::decay_t<decltype(val)>;
            if constexpr (!std::is_constructible_v<T, V>) 
            {
                throw std::exception{"Unknow type"};
            } else 
            {
                return T(val);
            }
        }, value);
    }

    template <class T>
    bool is() const 
    {
        return std::holds_alternative<T>(value);
    }

    template <class T>
    void set(T const &x)
    {
        value = x;
    }
};