//
// Created by ZZK on 2024/6/19.
//

#include "factory.h"

struct RobotStruct
{
    inline static const std::map<std::string, MsgFactoryBase::Ptr> lut = {
#define PER_MSG(type) { #type, makeFactory<type##Msg>() },
        PER_MSG(Move)
        PER_MSG(Jump)
        PER_MSG(Sleep)
        PER_MSG(Exit)
#undef PER_MSG
    };

    MsgBase::Ptr msg_base;

    void recv_data()
    {
        std::string type;
        std::cout << std::format("Input type: Move, Jump, Sleep, Exit\n");
        while (std::cin >> type)
        {
            if (!type.empty()) break;
        }
        try
        {
            msg_base = lut.at(type)->create();
            msg_base->load();
        } catch (const std::out_of_range &e)
        {
            std::cout << std::format("{}\n", e.what());
        }
    }

    void update()
    {
        if (msg_base)
        {
            msg_base->speak();
        }
    }
};

int main()
{
    {
        std::vector<std::shared_ptr<MsgBase>> msgs;
        msgs.emplace_back(std::make_shared<MsgImpl<MoveMsg>>(5, 10));
        msgs.emplace_back(std::make_shared<MsgImpl<JumpMsg>>(20));
        msgs.emplace_back(std::make_shared<MsgImpl<SleepMsg>>(8));
        msgs.emplace_back(std::make_shared<MsgImpl<ExitMsg>>());

        for (auto &msg : msgs)
        {
            msg->speak();
            msg->happy();
        }
    }

    {
        RobotStruct robot{};
        robot.recv_data();
        robot.update();
    }
}