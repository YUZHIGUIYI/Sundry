//
// Created by ZZK on 2024/6/16.
//

#include <iostream>
#include <format>
#include <sstream>
#include <thread>
#include <vector>
#include <functional>
#include <barrier>
#include "mt_queue.h"

using namespace std::chrono_literals;
using namespace experimental;

struct CounterState
{
    int counter = 0;
    std::string msg = {};
    bool finish = false;
};

std::barrier g_barrier{ 11 };
MTQueue<std::monostate> g_finish_queue;

// Implement task queue
void counter_thread(MTQueue<std::function<void(CounterState &)>> &counter_queue, MTQueue<int> &result_queue)
{
    CounterState state{};
    while (!state.finish)
    {
        auto task = counter_queue.pop();
        task(state);
    }
    std::ostringstream ss;
    ss << "msg: " << state.msg << '\n';
    ss << "counter: " << state.counter << '\n';
    std::cout << ss.str();
    result_queue.push(state.counter);
}

void compute(int beg, int end, MTQueue<std::function<void(CounterState &)>> &counter_queue)
{
    for (int i = beg; i < end; ++i)
    {
        counter_queue.push([i] (CounterState &state) {
            state.counter += i;
        });
    }
    counter_queue.push([] (CounterState &state) {
        state.msg += "OK";
    });
}

// Implement std::barrier g_barrier{ 10 };
// Invoked by 10 threads
void push_thread()
{
    g_finish_queue.push(std::monostate{});
}

// Invoked by 1 thread
void finish_thread()
{
    for (int i = 0; i < 10; ++i)
    {
        (void)g_finish_queue.pop();
    }
}

// Implement std::semaphore
// Specify limit of MTQueue<std::monostate> m_semaphore{ limit }
// Push or pop will be blocked

int main()
{
    MTQueue<std::function<void(CounterState &)>> counter_queue;
    MTQueue<int> result_queue;

    std::vector<std::jthread> compute_pool;
    std::vector<std::jthread> counter_pool;

    // Push compute tasks
    for (int i = 0; i < 10000; i += 1000)
    {
        compute_pool.emplace_back( compute, i, i + 1000, std::ref(counter_queue) );
    }
    // Obtain tasks and execute them
    counter_pool.emplace_back( counter_thread, std::ref(counter_queue), std::ref(result_queue) );
    counter_pool.emplace_back( counter_thread, std::ref(counter_queue), std::ref(result_queue) );
    counter_pool.emplace_back( counter_thread, std::ref(counter_queue), std::ref(result_queue) );

    for (auto &&t : compute_pool)
    {
        t.join();
    }
    counter_queue.push([] (CounterState &state) {
        state.finish = true;
    });
    counter_queue.push([] (CounterState &state) {
        state.finish = true;
    });
    counter_queue.push([] (CounterState &state) {
        state.finish = true;
    });
    for (auto &&t : counter_pool)
    {
        t.join();
    }

    // Obtain final result
    int result = 0;
    for (int i = 0; i < 3; ++i)
    {
        result += result_queue.pop();
    }
    std::cout << std::format("final result: {}\n", result);
}