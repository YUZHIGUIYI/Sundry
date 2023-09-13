//
// Created by ZZK on 2023/9/12.
//

#pragma once

#include <format>
#include <iostream>

struct memory_resource_inspector : std::pmr::memory_resource
{
public:
    explicit memory_resource_inspector(std::pmr::memory_resource *upstream)
    : m_upstream(upstream) {}

private:
    void *do_allocate(size_t bytes, size_t alignment) override
    {
        void *ptr = m_upstream->allocate(bytes, alignment);
        std::cout << std::format("allocate {} {} {}\n", ptr, bytes, alignment);
        return ptr;
    }

    bool do_is_equal(std::pmr::memory_resource const &other) const noexcept override
    {
        return other.is_equal(*m_upstream);
    }

    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override
    {
        std::cout << std::format("deallocate {} {} {}\n", ptr, bytes, alignment);
        return m_upstream->deallocate(ptr, bytes, alignment);
    }

private:
    std::pmr::memory_resource *m_upstream;
};





















































