///
/// \file
///
/// This file is a part of pattern matching testing suite.
///
/// \autor Yuriy Solodkyy <yuriy.solodkyy@gmail.com>
///
/// This file is a part of the XTL framework (http://parasol.tamu.edu/xtl/).
/// Copyright (C) 2005-2012 Texas A&M University.
/// All rights reserved.
///

//#define POD_ONLY
#include <iostream>
#include "match_shape.hpp"

std::ostream& operator<<(std::ostream& os, const loc& l)
{
    return os << '(' << l.first << ',' << l.second << ')';
}

std::ostream& operator<<(std::ostream& os, const cloc& l)
{
    return os << '(' << l.first << ',' << l.second << ')';
}

double heron(const loc& a, const loc& b, const loc& c) { return 1.0/2; }

double area(const Shape& shape)
{
    wildcard       _; // Meta variable
    variable<loc>  x,y,z;
    variable<double> r,s;

    if (cons<Circle>(_,r)(shape))
        return 3.14 * r * r;

    if (cons<Square>(_,s)(shape))
        return s * s;

    if (cons<Triangle>(x,y,z)(shape))
        return heron(x,y,z);

    XTL_ASSERT(!"Inexhaustive search");
}

loc center(/*const*/ Shape& shape)
{
    variable<loc> c;

    if (cons<Circle>(c/*,_*/)(shape))
        return c;

    variable<double> x,y,s;

    if (cons<Square>(
            cons<loc>(x,y),s)(shape))
        return loc(x+s/2,y+s/2);

    variable<double> x1,y1,x2,y2,x3,y3;

    if (cons<Triangle>(
            cons<loc>(x1,y1),
            cons<loc>(x2,y2),
            cons<loc>(x3,y3)
                       )(shape))
        return loc((x1+x2+x3)/3,(y1+y2+y3)/3);

    XTL_ASSERT(!"Inexhaustive search");
}

void foo(Shape* s)
{
    variable<loc>  x,y,z;
    variable<double> a,b,c;
    wildcard       _;

    auto pattern = cons<Circle>(x, -a*2+1);

    if (pattern(s))
        std::cout << "Matched against pattern " << a << std::endl;

    if (cons<Circle>(x, -a*2+1)(s))
        std::cout << "Matched against subexpression " << a << std::endl;

    if (cons<Circle>(x |= x == loc(1,1), a)(s))
        std::cout << "Matched against guard " << a << std::endl;

    if (cons<Circle>(x, a |= a > 3 && a < 5)(s))
        std::cout << "Matched radius against COMPLEX guard " << a << std::endl;

    if (cons<Circle>(cons<loc>(b,1), a)(s))
        std::cout << "Matched against subexpression " << b << std::endl;

    if (cons<Circle>(x,4.0)(s))
        std::cout << "Circle with center " << x << " and FIXED radius " << std::endl;

    if (cons<Circle>(x,a)(s))
        std::cout << "Circle with center " << x << " and radius " << a << std::endl;
    else
    if (cons<Square>(x,a)(s))
        std::cout << "Square with top left " << x << " and side " << a << std::endl;
    else
    if (cons<Triangle>(x,y,z)(s))
        std::cout << "Triangle with corners " << x << ',' << y << ',' << z << std::endl;
}

void bar(ADTShape& s)
{
    variable<cloc>  x,y,z;
    variable<double> a;

#ifndef POD_ONLY
    if (cons<ADTShapeEx,ADTShape::circle>(x,a)(s))
        std::cout << "ADTCircleEx with center " << x << " and radius " << a << std::endl;
    else
    if (cons<ADTShapeEx,ADTShape::square>(x,a)(s))
        std::cout << "ADTSquareEx with top left " << x << " and side " << a << std::endl;
    else
    if (cons<ADTShapeEx,ADTShape::triangle>(x,y,z)(s))
        std::cout << "ADTTriangleEx with corners " << x << ',' << y << ',' << z << std::endl;
#endif
    if (cons<ADTShape,ADTShape::circle>(x,a)(s))
        std::cout << "ADTCircle with center " << x << " and radius " << a << std::endl;
    else
    if (cons<ADTShape,ADTShape::square>(x,a)(s))
        std::cout << "ADTSquare with top left " << x << " and side " << a << std::endl;
    else
    if (cons<ADTShape,ADTShape::triangle>(x,y,z)(s))
        std::cout << "ADTTriangle with corners " << x << ',' << y << ',' << z << std::endl;
}

int main()
{
    Shape* c = new Circle(loc(1,1),4);
    Shape* s = new Square(loc(2,2),2);
    Shape* t = new Triangle(loc(0,0),loc(0,1),loc(1,0));

    foo(c); std::cout << "Area: " << area(*c) << " Center: " << center(*c) << std::endl;
    foo(s); std::cout << "Area: " << area(*s) << " Center: " << center(*s) << std::endl;
    foo(t); std::cout << "Area: " << area(*t) << " Center: " << center(*t) << std::endl;

#ifdef POD_ONLY
    ADTShape ac = {ADTShape::circle,   {{{1,1}, 4}}};
    ADTShape as = {ADTShape::square,   {{{2,2}, 2}}};
    ADTShape at = {ADTShape::triangle, {{{1,1}, 1}}};
#else
    cloc l00 = {0,0};
    cloc l11 = {1,1};
    cloc l10 = {1,0};
    ADTShape ac(l11, 4);
    ADTShape as(2, l00);
    ADTShape at(l11, l10, l00);
#endif
    bar(ac);
    bar(as);
    bar(at);

    ADTShapeEx xc = ac;
    ADTShapeEx xs = as;
    ADTShapeEx xt = at;

    bar(xc);
    bar(xs);
    bar(xt);
}
