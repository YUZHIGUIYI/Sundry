#include <iostream>
#include <vector>
#include <memory>

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

class Shape
{
private:
    struct ShapeConcept
    {
        virtual ~ShapeConcept() = default;

        virtual void do_serialize() const = 0;
        virtual void do_draw() const = 0;
        virtual std::unique_ptr<ShapeConcept> clone() const = 0;    // The Prototype design pattern
    };

    template<typename ShapeT>
    struct ShapeModel : public ShapeConcept
    {

        ShapeModel(ShapeT shape) : shape_{ std::move(shape) } {}

        void do_serialize() const override
        {
            serialize(shape_);
        }

        void do_draw() const override
        {
            draw(shape_);
        }

        std::unique_ptr<ShapeConcept> clone() const override    // The Prototype design pattern
        {
            return std::make_unique<ShapeModel>(*this);
        } 

        ShapeT shape_;
    };

    friend void serialize(Shape const& shape)
    {
        shape.pimpl->do_serialize();
    }

    friend void draw(Shape const& shape)
    {
        shape.pimpl->do_draw();
    }

    std::unique_ptr<ShapeConcept> pimpl;    // The bridge design pattern 

public:
    // A templated constructor, creating a bridge
    template<typename ShapeT>
    Shape(ShapeT shape)
        : pimpl{ std::make_unique<ShapeModel<ShapeT>>(std::move(shape)) } {}
    
    // Copy operations
    Shape(Shape const& other)
        : pimpl(other.pimpl->clone())
    {
    }

    Shape& operator=(Shape const& other)
    {
        other.pimpl->clone().swap( pimpl );
        return *this;
    }

    // Move operations
    // Option 1: Moved-from shapes are semantically equivalent to a nullptr
    // Option 2: Move remains undefined, copy serves as a fallback 
    // Option 3: The move constructor is undefined, the move assignment operator is implemented in terms of swap
    Shape(Shape&& other) = default;
    Shape& operator=(Shape&& other) = default;
    // Option 3
    // Shape& operator=(Shape&& other)
    // {
    //     pimpl.swap( other.pimpl );
    //     return *this;
    // }
};

void drawAllShapes(std::vector<Shape> const& shapes)
{
    for (auto const& shape : shapes)
    {
        draw( shape );
    }
}

int main()
{
    using Shapes = std::vector<Shape>;

    // No pointers
    // No manual dynamic allocation
    // No manual life-time management
    // value semantics 
    // Very simple code
    // Beautiful C++

    Shapes shapes;
    shapes.emplace_back( Circle{2.0} );   
    shapes.emplace_back( Square{2.5} );
    drawAllShapes( shapes );
}