//
// Created by ZZK on 2024/5/16.
//

#include "intrusive_ptr.h"
#include <iostream>

struct Buffer;

struct BufferDeleter
{
    void operator()(Buffer *buffer);
};

struct Buffer : experimental::IntrusivePtrEnabled<Buffer, BufferDeleter, experimental::MultiThreadCounter>
{
public:
    friend struct BufferDeleter;

    Buffer()
    {
        std::cout << "Construct buffer object\n";
    }

    ~Buffer()
    {
        std::cout << "Deconstruct buffer object\n";
    }

    void call()
    {
        std::cout << "Do something =========\n";
    }
};

inline void BufferDeleter::operator()(Buffer *buffer)
{
    delete buffer;
    buffer = nullptr;
}

int main()
{
    auto buffer = experimental::make_handle<Buffer>();
    {
        auto buffer1 = buffer;
        buffer->call();
        auto buffer2 = std::move(buffer1);
        buffer->call();
    }
    buffer->call();
}





















