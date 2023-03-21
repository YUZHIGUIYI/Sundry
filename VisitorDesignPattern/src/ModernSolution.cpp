#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include <variant>
#include <format>
#include <xutility>

// A Modern Solution of Visitor Design Pattern
// No base class required 
// No accumulation of dependencies via menber functions
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

class Ellipse
{
public:
    explicit Ellipse(double r1, double r2) : longRadius(std::max(r1, r2)), 
        shortRadius(std::min(r1, r2)) 
    {}

    std::pair<double, double> getSides() const noexcept 
    { 
        return { longRadius, shortRadius }; 
    }

private:
    double longRadius, shortRadius;
};

// No base class required 
// Operations can be non-intrusively be added (OCP)
class Draw
{
public:
    void operator()(Circle const& circle) const
    {
        std::cout << std::format("Draw circle: {0}\n", circle.getRadius());
    }

    void operator()(Square const& square) const
    {
        std::cout << std::format("Draw square: {0}\n", square.getSide());
    }

    void operator()(Ellipse const& ellipse) const
    {
        std::cout << std::format("Draw ellipse: {0}, {1}\n", 
                                std::get<0>(ellipse.getSides()), std::get<1>(ellipse.getSides()));
    }
};

using Shape = std::variant<Circle, Square, Ellipse>;

void drawAllShapes(std::vector<Shape> const& shapes)
{
    for (auto const& s : shapes)
    {
        std::visit(Draw{}, s);
    }
}

int main()
{
    // No pointers, no allocations, but values
    using Shapes = std::vector<Shape>;

    // Creating some shapes
    Shapes shapes;
    shapes.emplace_back(Circle{ 2.0 });
    shapes.emplace_back(Square{ 3.5 });
    shapes.emplace_back(Ellipse{ 1.0, 2.0 });

    drawAllShapes(shapes);
}
