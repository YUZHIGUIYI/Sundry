//
// Created by ZZK on 2024/6/26.
//

#include "signal.h"
#include <iostream>
#include <string>
#include <format>

struct Foo
{
    int age = 14;

    void on_input(int i) const
    {
        std::cout << std::format("Foo of age {} got i = {}\n", age, i);
    }

    ~Foo()
    {
        std::cout << "Foo destruct\n";
    }
};

struct Bar
{
    int age = 42;
    void on_input(int i) const
    {
        std::cout << std::format("Bar of age {} got {}\n", age, i);
    }

    void on_exit(const std::string &msg1, const std::string &msg2) const
    {
        std::cout << std::format("Bar got exit event: {} {}\n", msg1, msg2);
    }
};

std::shared_ptr<Bar> gbar = nullptr;
std::shared_ptr<void> gmine = nullptr;

using namespace experimental;

struct Input
{
    Signal<int> on_input;
    Signal<std::string, std::string> on_exit;

    void main_loop()
    {
        int i;
        while (std::cin >> i)
        {
            on_input.emit(i);
            if (i == 100)
            {
                gbar = nullptr;
                break;
            }
        }
        on_exit.emit("Hello", "world");
    }
};

void test(const std::string &msg1, const std::string &msg2)
{
    std::cout << std::format("main received exit event: {} {}\n", msg1, msg2);
}

void test(int msg1, const std::string &msg2)
{
    std::cout << std::format("main received exit event: {} {}\n", msg1, msg2);
}

struct Mine : std::enable_shared_from_this<Mine>
{
    void register_on(Input &input)
    {
        input.on_input.connect(weak_from_this(), &Mine::on_input);
    }

    void on_input(int i)
    {
        std::cout << std::format("Mine got i = {}\n", i);
        if (i == 5)
        {
            gmine = nullptr;
        }
    }

    ~Mine()
    {
        std::cout << "Mine destruct\n";
    }
};

void dummy(Input &input)
{
    auto bar = std::make_shared<Bar>();
    input.on_input.connect(std::weak_ptr<Bar>(bar), &Bar::on_input, NShot{ 3 });
    gbar = bar;

    auto mine = std::make_shared<Mine>();
    mine->register_on(input);
    gmine = mine;
}

int main()
{
    std::shared_ptr<Foo> foo = std::make_shared<Foo>();
    Input input;
    dummy(input);

    input.on_input.connect(SIG_FUNC(foo->on_input));

    input.on_input.connect([=] (int i) {
        std::cout << std::format("main received input: {}\n", i);
    });
    input.main_loop();
}





























