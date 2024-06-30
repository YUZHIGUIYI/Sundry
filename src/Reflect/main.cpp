//
// Created by ZZK on 2024/6/29.
//

#include <iostream>
#include "reflect.h"
#include "reflect_json.h"

struct Address
{
    std::string country;
    std::string province;
    std::string city;

    void show() const
    {
        std::cout << "Country: " << country << '\n';
        std::cout << "Province: " << province << '\n';
        std::cout << "City: " << city << '\n';
    }

    [[nodiscard]] std::string to_str() const
    {
        return country + " " + province + " " + city;
    }

    static void test()
    {
        std::cout << "Static function test\n";
    }

    REFLECT(city, country, province, show, to_str, test);
};

struct Student
{
    std::string name;
    int age = 0;
    Address addr;

    REFLECT(name, age, addr);
};

int main()
{
    Student stu{
        .name = "Ming",
        .age = 25,
        .addr{
            .country = "China",
            .province = "Jiangsu",
            .city = "Suzhou",
        }
    };

    std::string binary = reflect_json::serialize(stu);
    std::cout << binary << '\n';
    auto stu_des = reflect_json::deserialize<Student>(binary);

    puts("=============================================================================");
    std::cout << stu_des.name << '\n';
    std::cout << stu_des.age << '\n';
    std::cout << stu_des.addr.country << '\n';
    std::cout << stu_des.addr.province << '\n';
    std::cout << stu_des.addr.city << '\n';

    puts("=============================================================================");
    auto vec = reflect_json::deserialize<std::vector<int>>(R"json([1, 2, 3])json");
    std::cout << vec.at(0) << '\n';
    std::cout << vec.at(1) << '\n';
    std::cout << vec.at(2) << '\n';
}



















