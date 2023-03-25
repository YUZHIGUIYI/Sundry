#include <NumericObject.h>
#include <iostream>
#include <format>
#include <memory>
#include <utility>

int main()
{
    NumericObject obj1{ 0xffff };
    std::cout << std::boolalpha << obj1.is<double>() << std::endl;
    std::cout << std::format("Origin value index {}\n", obj1.value.index());
    std::cout << std::format("Origin value is {}\n", obj1.get<int>());
    std::cout << "==============--=====================\n";
    auto tmp = obj1.clone();
    auto obj2 = static_cast<NumericObject *>(tmp.get());
    obj2->set(999.0);
    std::cout << std::boolalpha << obj2->is<int>() << std::endl;
    std::cout << std::format("Cloned value index {}\n", obj2->value.index());
    std::cout << std::format("Cloned value is {}\n", obj2->get<double>());
}