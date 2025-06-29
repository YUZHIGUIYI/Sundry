//
// Created by ZZK on 2024/3/3.
//

#include "callable.h"
#include "async_execution.h"
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

    std::cout << "=============================\n";
    auto s1 = just(42);
    auto op1 = connect(s1, cout_receiver{});
    start(op1);
    std::cout << "=============================\n";
    auto s2 = then(just(42), [] (int32_t i) { return i + 1; });
    auto op2 = connect(s2, cout_receiver{});
    start(op2);
    std::cout << "=============================\n";
    auto s3 = then(just(42), [](int32_t i) { return i + 1; });
    int32_t val3 = sync_wait(s3).value();
    std::cout << "Result: " << val3 << '\n';
    std::cout << "=============================\n";
    {
        run_loop loop;
        auto sched4 = loop.get_scheduler();
        auto s4 = then(schedule(sched4), [](auto) { return 42; });
        auto op4 = connect(s4, cout_receiver{});

        start(op4);
        auto s5 = then(schedule(sched4), [](auto) { return 43; });
        auto op5 = connect(s5, cout_receiver{});
        start(op5);

        loop.finish();
        loop.run();
    }
    std::cout << "=============================\n";
    {
        thread_context th;
        auto sched6 = th.get_scheduler();

        auto s6 = then(schedule(sched6), [](auto) { return 42; });
        auto s7 = then(s6, [](int i){ return i+1; });
        auto val7 = sync_wait(s7).value();
        th.finish();
        th.join();

        std::cout << val7 << '\n';
    }
}





















