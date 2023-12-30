//
// Created by ZZK on 2023/12/30.
//

#include "thread_pool.h"
#include <random>
#include <chrono>
#include <format>
#include <iostream>

#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cerr << #x ": " << std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - bench_##x).count(); std::cerr << "ms\n";

std::random_device rd;
std::mt19937 mt{ rd() };
std::uniform_int_distribution<int32_t> uniform_dist{ -1000, 1000 };

void simulate_hard_computation()
{
    std::this_thread::sleep_for(std::chrono::microseconds{ 1000 + uniform_dist(mt) });
}

void multiply(int32_t a, int32_t b)
{
    simulate_hard_computation();
    auto res = a * b;
    std::cout << std::format("{} x {} = {}\n", a, b, res);
}

void multiply_output(int32_t &res, int32_t a, int32_t b)
{
    simulate_hard_computation();
    res = a * b;
    std::cout << std::format("{} x {} = {}\n", a, b, res);
}

int32_t multiply_return(int32_t a, int32_t b)
{
    simulate_hard_computation();
    auto res = a * b;
    std::cout << std::format("{} x {} = {}\n", a, b, res);
    return res;
}

void benchmark()
{
    toy::ThreadPool thread_pool{ 3 };

    thread_pool.init();

    for (int32_t i = 1; i <= 15; ++i)
    {
        for (int32_t j = 1; j <= 15; ++j)
        {
            thread_pool.submit(multiply, i, j);
        }
    }

    int32_t output_value = 0;
    auto first_future = thread_pool.submit(multiply_output, std::ref(output_value), 9, 67);
    first_future.get();
    std::cout << std::format("Last operation(output) result equals to: {}\n", output_value);

    auto second_future = thread_pool.submit(multiply_return, 112, 789);
    auto future_value = second_future.get();
    std::cout << std::format("Last operation(return) result equals to: {}\n", future_value);

    thread_pool.shut_down();
}

int main()
{
    TICK(ThreadPoolBenchmark)
    benchmark();
    TOCK(ThreadPoolBenchmark)
}