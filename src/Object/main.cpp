//
// Created by ZZK on 2024/2/18.
//

#include "object.h"
#include <vector>
#include <iostream>

struct Person
{
    std::string_view name;
    size_t age;

    void say(std::string_view msg) const
    {
        std::cout << name << " say: " << msg << '\n';
    }
};

int main()
{
    using namespace experimental;
    Any person = Person{ "Tom", 18 };
    person.get_type()->fields.insert({ "name", { type_of<std::string_view>(), offsetof(Person, name) } });
    person.get_type()->fields.insert({ "age", {type_of<size_t>(), offsetof(Person, age) } });
    person.get_type()->methods.insert({ "say", type_ensure<&Person::say>() });

    std::vector<Any> args = { std::string_view{ "Hello" } };
    person.invoke("say", args);

    auto fn = [] (std::string_view name, Any value)
    {
        if (value.get_type() == type_of<std::string_view>())
        {
            std::cout << name << " = " << value.cast<std::string_view>() << '\n';
        } else if (value.get_type() == type_of<size_t>())
        {
            std::cout << name << " = " << value.cast<size_t>() << '\n';
        }
    };

    person.foreach(fn);
}