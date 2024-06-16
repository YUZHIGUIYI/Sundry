#include <disable_copy.h>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <string>

struct Vec3
{
    float x, y, z;
    Vec3() = default;
    Vec3(float value = 0.0f) : x(value), y(value), z(value) {}
};

struct Object
{
    float scale;
    Vec3 position, size;
    Object(float value = 1.0f) : position(0.0f), size(value) {}
};

struct Scene : disable_copy
{
    uint32_t m_Id  = 0;

    std::unordered_map<uint32_t, Object> elements;

    Scene() = default;
    ~Scene() = default;

    bool insert(const Object& object)
    {
        auto [it, isInsert] = elements.try_emplace(m_Id++, object);
        return isInsert;
    }
};

int main()
{
    Scene scene1;
    // No allowed since base class 'disable_copy' has a deleted copy assignment operator and
    // a deleted copy constructor
    // Scene scene2 = scene1;
    // Scene scene3 = std::move(scene1);
    // Scene scene4(scene1);
    // Scene scene5;
    // scene5 = scene1;
    // scene5 = std::move(scene1);
    scene1.insert(Object{ 1.0f });
}