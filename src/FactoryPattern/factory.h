//
// Created by ZZK on 2024/6/19.
//

#pragma once

#include <iostream>
#include <format>
#include <map>
#include <memory>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

struct MoveMsg
{
    int x;
    int y;

    void speak()
    {
        std::cout << std::format("Move {}, {}\n", x, y);
    }
};

struct JumpMsg
{
    int height;

    void speak()
    {
        std::cout << std::format("Jump {}\n", height);
    }

    void happy()
    {
        std::cout << std::format("Happy jump\n");
    }
};

struct SleepMsg
{
    int time;

    void speak()
    {
        std::cout << std::format("Sleep {}\n", time);
    }

    void happy()
    {
        std::cout << std::format("Sleep happy\n");
    }
};

struct ExitMsg
{
    void speak()
    {
        std::cout << std::format("Exit\n");
    }
};

namespace msg_extra_func
{
    void load(MoveMsg &msg)
    {
        std::cin >> msg.x >> msg.y;
    }

    void load(JumpMsg &msg)
    {
        std::cin >> msg.height;
    }

    void load(SleepMsg &msg)
    {
        std::cin >> msg.time;
    }

    void load(ExitMsg &)
    {

    }
}

struct MsgBase
{
    using Ptr = std::shared_ptr<MsgBase>;

    virtual ~MsgBase() = default;

    virtual void speak() = 0;
    virtual void happy() = 0;
    virtual void load() = 0;
    [[nodiscard]] virtual Ptr clone() const = 0;
};

template <typename Msg>
struct MsgImpl : MsgBase
{
    Msg msg;

    ~MsgImpl() override = default;

    template <typename ... Args>
    MsgImpl(Args&& ... args) : msg(std::forward<Args>(args)...)
    {

    }

    void happy() override
    {
        if constexpr (requires { msg.happy(); })
        {
            msg.happy();
        } else
        {
            std::cout << "No happy\n";
        }
    }

    void speak() override
    {
        msg.speak();
    }

    void load() override
    {
        msg_extra_func::load(msg);
    }

    [[nodiscard]] Ptr clone() const override
    {
        return std::make_shared<MsgImpl<Msg>>(msg);
    }
};

template <typename MsgType, typename ... Args>
auto makeMsg(Args&& ... args)
{
    return std::make_shared<MsgImpl<MsgType>>(std::forward<Args>(args)...);
}

// Factory
struct MsgFactoryBase
{
    virtual MsgBase::Ptr create() = 0;
    virtual ~MsgFactoryBase() = default;

    using Ptr = std::shared_ptr<MsgFactoryBase>;
};

template <typename MsgType>
struct MsgFactoryImpl : MsgFactoryBase
{
    MsgBase::Ptr create() override
    {
        return std::make_shared<MsgImpl<MsgType>>();
    }
};

template <typename MsgType>
MsgFactoryBase::Ptr makeFactory()
{
    return std::make_shared<MsgFactoryImpl<MsgType>>();
}




















































