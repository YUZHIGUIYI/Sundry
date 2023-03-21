#include <iostream>
#include <vector>
#include <memory>

// Type Erasure MVD
// Non-owning abstraction does neither allocate nor copy

class Circle
{
public:
    explicit Circle(double rad) : radius{ rad } {}

    double getRadius() const noexcept { return radius; }

private:
    double radius;
};

class Square
{
public:
    explicit Square(double s) : side{ s } {}

    double getSide() const noexcept { return side; }

private:
    double side;
};

void draw(Circle const& circle)
{
    std::cout << "Draw circle\n";
}

void draw(Square const& square)
{
    std::cout << "Draw square\n";
}

void serialize(Circle const& circle)
{
    std::cout << "Serialize circle\n";
}

void serialize(Square const& square)
{
    std::cout << "Serialize square\n";
}

// Type Erasure with manual virtual dispatch
// Non-owning abstraction does neither allocate nor copy
class ShapeConstRef
{
public:
    template<typename ShapeT>
    ShapeConstRef(ShapeT const& shape)
        : shape_{ std::addressof(shape) }, 
        draw_{ [](void const* shape){
            draw( *static_cast<ShapeT const*>(shape) );
        } }
    {}

private:
    friend void draw(ShapeConstRef const& shape)
    {
        shape.draw_(shape.shape_);
    }

    using DrawOperation = void(void const*);

    void const* shape_{ nullptr };
    DrawOperation* draw_{ nullptr };
};

void draw_all(ShapeConstRef shape)
{
    
}

int main()
{
    Circle circle{ 2.0 };
    Square square{ 4.0 };

    draw( ShapeConstRef{circle} );
    draw(ShapeConstRef{square});
}