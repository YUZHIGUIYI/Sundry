//
// Created by ZZK on 2023/9/19.
//

#include "scope_profiler.h"

int main()
{
    RepeatScopeProfiler(100)
    {
        DefScopeProfiler
        std::vector<size_t> vec;
        for (size_t i = 0; i < 1000; ++i)
        {
            vec.push_back(i);
        }
    }
}