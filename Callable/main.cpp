//
// Created by ZZK on 2024/3/3.
//

#include "callable.h"
#include <iostream>

void sum(uint32_t n)
{
    uint32_t sum = 1;
    for (uint32_t i = 1; i <= n; ++i)
    {
        sum *= i;
    }
    std::cout << sum << '\n';
}

struct Test
{
private:
    std::string m_name;

public:
    explicit Test(std::string &&name) : m_name(std::move(name)) {}

    std::string& get_name()
    {
        return m_name;
    }

    [[nodiscard]] const std::string& get_name() const
    {
        return m_name;
    }
};

int main()
{
    Test test{ "This is a simple test, do not use SSO\n" };
    util::SmallCallable<void(), 32, 8> log_out{
        [] () { std::cout << "Hello world!\n"; }
    };
    util::SmallCallable<void(uint32_t), 32, 8> sum_out{ sum };
    util::SmallCallable<std::string&(), 32, 8> test_out{&test, &Test::get_name };
    log_out.call();
    sum_out.call(10);
    std::cout << test_out.call();
}