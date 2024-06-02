//
// Created by ZZK on 2024/6/2.
//

#pragma once

#include <typeinfo>
#include <typeindex>
#include <array>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace experimental
{
    struct Message
    {
        virtual ~Message() = default;
    };

    struct MoveMessage : Message
    {
        std::array<float, 3> velocity_change = { 0.0f, 0.0f, 0.0f };
    };

    struct Component;

    struct GameObject final
    {
        std::vector<Component *> components;
        std::unordered_map<std::type_index, std::vector<Component *>> subscribers;

        template <typename EventType>
        void subscribe(Component *component);

        template <typename EventType>
        void send(EventType *msg);

        void add(Component *component);

        void update();
    };

    struct Component
    {
        virtual void update(GameObject *go) {};
        virtual void subscribe_messages(GameObject *go) {};
        virtual void handle_message(Message *msg) {};
        virtual ~Component() = default;
    };

    struct Movable final : Component
    {
        std::array<float, 3> position = { 0.0f, 0.0f, 0.0f };
        std::array<float, 3> velocity = { 0.0f, 0.0f, 0.0f };

        void subscribe_messages(GameObject *go) override
        {
            go->subscribe<MoveMessage>(this);
        }

        void handle_message(Message *msg) override
        {
            if (auto mm = dynamic_cast<MoveMessage *>(msg))
            {
                velocity[0] += mm->velocity_change[0];
                velocity[1] += mm->velocity_change[1];
                velocity[2] += mm->velocity_change[2];
                std::puts("Move message has been updated in Movable\n");
            }
        }
    };

    struct PlayerController final : Component
    {
        void update(GameObject *go) override
        {
            MoveMessage mm{};
            mm.velocity_change[1] += 1.0f;
            go->send(&mm);

            // ...
        }
    };

    template <typename EventType>
    inline void GameObject::subscribe(Component *component)
    {
        subscribers[std::type_index(typeid(EventType))].push_back(component);
    }

    template <typename EventType>
    inline void GameObject::send(EventType *msg)
    {
        for (auto &&c : subscribers[std::type_index(typeid(EventType))])
        {
            c->handle_message(msg);
        }
    }

    inline void GameObject::add(Component *component)
    {
        components.push_back(component);
        component->subscribe_messages(this);
    }

    inline void GameObject::update()
    {
        for (auto &&c : components)
        {
            c->update(this);
        }
    }

    inline void test_subscribe_mode()
    {
        GameObject go{};
        Movable move_component{};
        PlayerController player_controller{};

        go.add(&move_component);
        go.add(&player_controller);

        go.update();
    }
}

















