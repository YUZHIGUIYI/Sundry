//
// Created by ZZK on 2023/12/31.
//

#include "multi_thread_pool.h"
#include <format>
#include <iostream>
#include <string>
#include <chrono>

#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cerr << std::format("{}: {} ms\n", #x, std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - bench_##x).count());

std::string func_string(const std::string &str)
{
    return "Hello" + str;
}

void test_multiple_thread_pool()
{
    std::cout << std::format("Test multiple thread pool\n");

    experimental::MultiThreadPool thread_pool{ 5 };

    for (size_t i = 0; i < 2000; ++i)
    {
        thread_pool.schedule_by_id([] { std::cout << "Hello\n"; });

        auto first_task = std::make_shared<std::packaged_task<std::string()>>([]() { return func_string(" world\n"); });
        std::future<std::string> first_future = first_task->get_future();
        thread_pool.schedule_by_id([task = std::move(first_task)]() { (*task)(); });

        std::cout << std::format("Future is: {}\n", first_future.get());
    }
}

int main()
{
    TICK(MultipleThreadPoolBenchmark)
    test_multiple_thread_pool();
    TOCK(MultipleThreadPoolBenchmark)
}























