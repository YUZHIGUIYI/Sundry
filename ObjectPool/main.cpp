//
// Created by ZZK on 2024/3/3.
//

#include "object_pool.h"

int main()
{
    util::ThreadSafeObjectPool<size_t> safe_object_pool;
    util::UnsafeObjectPool<size_t> unsafe_object_pool;
}