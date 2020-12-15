#include "maths.h"

using namespace Honeybear;

Vec2::Vec2() :
    x(0.0f),
    y(0.0f)
{}

Vec2::Vec2(const float value) :
    x(value),
    y(value)
{}

Vec2::Vec2(const float x, const float y) : 
    x(x),
    y(y)
{}

Vec3::Vec3() :
    x(0.0f),
    y(0.0f),
    z(0.0f)
{}

Vec3::Vec3(const float value) :
    x(value),
    y(value),
    z(value)
{}

Vec3::Vec3(const float x, const float y, const float z) : 
    x(x),
    y(y),
    z(z)
{}

Vec4::Vec4() :
    x(0.0f),
    y(0.0f),
    z(0.0f),
    w(0.0f)
{}

Vec4::Vec4(const float value) :
    x(value),
    y(value),
    z(value),
    w(value)
{}

Vec4::Vec4(const float x, const float y, const float z, const float w) : 
    x(x),
    y(y),
    z(z),
    w(w)
{}