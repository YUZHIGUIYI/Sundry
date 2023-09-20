//
// Created by ZZK on 2023/9/12.
//

#include "memory_resource_inspector.h"
#include "ticktock.h"
#include <memory_resource>

// new/delete < synchronized < unsynchronized < monotonic
int main()
{
    TICK(MemoryResourceInspector)

    memory_resource_inspector mem{ std::pmr::new_delete_resource() };

    std::pmr::vector<int> vec{ &mem };
    for (int i = 0; i < 4096; ++i)
    {
        vec.push_back(i);
    }

    TOCK(MemoryResourceInspector)
}