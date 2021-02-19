#include <cmath>
#include "maths.h"

using namespace Honeybear;

// constructors ------------------
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

Vec2 Vec2::ZERO = Vec2();

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

Vec3 Vec3::ZERO = Vec3();

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

Vec4 Vec4::ZERO = Vec4();
// --------------------------------

float Vec2::Dot(const Vec2& other)
{
    return(x * other.x) + (y * other.y);
}

float Vec2::Magnitude()
{
    return std::sqrt(x*x + y*y);
}

Vec2 Vec2::Normalised()
{
    float length = Magnitude();
    return Vec2(x / length, y / length);
}

Vec2 Vec2::Perp()
{
    return Vec2(y, -x);
}

Vec2 Vec2::Negated()
{
    return Vec2(-x, -y);
}

Vec2 Vec2::Add(const Vec2& other) const
{
    return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::Subtract(const Vec2& other) const
{
    return Vec2(x - other.x, y - other.y);
}

bool Vec2::NonZero()
{
    return std::abs(x) > 0.0f || std::abs(y) > 0.0f;
}

float Vec2::Distance(const Vec2& other)
{
    return Subtract(other).Magnitude();
}

bool Vec2::operator==(const Vec2& other) const
{
    return x == other.x
        && y == other.y;
}

Vec2 Vec2::operator-(const Vec2& other) const
{
    return Subtract(other);
}

Vec2 Vec2::Rotated(const Vec2& anchor_point, float angle_deg)
{
    float angle_rad = angle_deg * (PI / 180.0f);
    float cos_angle = std::cos(angle_rad);
    float sin_angle = std::sin(angle_rad);

    float dx = x - anchor_point.x;
    float dy = y - anchor_point.y;

    float x_rotated = anchor_point.x + (dx * cos_angle - dy * sin_angle);
    float y_rotated = anchor_point.y + (dx * sin_angle + dy * cos_angle);

    return Vec2(x_rotated, y_rotated);
}

void Honeybear::Rotate(Vec2& value, const Vec2& origin, const float angle_deg)
{
    float angle_rad = angle_deg * (PI / 180.0f);
    float cos_angle = std::cos(angle_rad);
    float sin_angle = std::sin(angle_rad);
    Rotate(value, origin, cos_angle, sin_angle);
}

void Honeybear::Rotate(Vec2& value, const Vec2& origin, const float cos_angle, const float sin_angle)
{
    float dx = value.x - origin.x;
    float dy = value.y - origin.y;

    value.x = origin.x + (dx * cos_angle - dy * sin_angle);
    value.y = origin.y + (dx * sin_angle + dy * cos_angle);
}

Vec2 Vec2::MovedInDirection(const Vec2& dir, float distance)
{
    return Vec2(
        x + (dir.x * distance),
        y + (dir.y * distance)
    );
}

float Honeybear::VectorToRadians(const Vec2& v)
{
    return std::atan2(v.y, v.x);
}
void Honeybear::VectorToRadians(const Vec2& v, float& degrees)
{
    degrees = std::atan2(v.y, v.x);
}

float Honeybear::VectorToDegrees(const Vec2& v)
{
    return RadiansToDegrees(VectorToRadians(v));
}
void Honeybear::VectorToDegrees(const Vec2& v, float& radians)
{
    radians = RadiansToDegrees(VectorToRadians(v));
}

float Honeybear::DegreesToRadians(const float degrees)
{
    return degrees * (PI / 180.0f);
}
void Honeybear::DegreesToRadians(const float degrees, float& radians)
{
    radians = degrees * (PI / 180.0f);
}

float Honeybear::RadiansToDegrees(const float radians)
{
    return (radians / PI) * 180.0f;
}
void Honeybear::RadiansToDegrees(const float radians, float& degrees)
{
    degrees = (radians / PI) * 180.0f;
}

Vec2 Honeybear::RadiansToVector(const float radians)
{
    return Vec2(
        std::cos(radians),
        std::sin(radians)
    );
}
void Honeybear::RadiansToVector(const float radians, Vec2& v)
{
    v.x = std::cos(radians);
    v.y = std::sin(radians);
}

Vec2 Honeybear::DegreesToVector(const float degrees)
{
    return RadiansToVector(DegreesToRadians(degrees));
}
void Honeybear::DegreesToVector(const float degrees, Vec2& v)
{
    RadiansToVector(DegreesToRadians(degrees), v);
}

float Honeybear::Ease(float t, float b, float c, float d, EasingType easing_type)
{
    if(c == 0.0f) return b;

    if(easing_type == LINEAR)
    {
        return EaseLinear(t, b, c, d);
    }
    else if(easing_type == EASE_IN_CUBIC)
    {
        return EaseInCubic(t, b, c, d);
    }
    else if(easing_type == EASE_OUT_CUBIC)
    {
        return EaseOutCubic(t, b, c, d);
    }
    else if(easing_type == EASE_IN_OUT_CUBIC)
    {
        return EaseInOutCubic(t, b, c, d);
    }
    else if(easing_type == EASE_IN_BACK)
    {
        return EaseInBack(t, b, c, d);
    }
    else if(easing_type == EASE_OUT_BACK)
    {
        return EaseOutBack(t, b, c, d);
    }
    else if(easing_type == EASE_IN_OUT_BACK)
    {
        return EaseInOutBack(t, b, c, d);
    }
}

float Honeybear::EaseLinear(float t, float b, float c, float d)
{
    float p = t / d;
    return b + (c * p);
}

float Honeybear::EaseInCubic(float t, float b, float c, float d)
{
    return c * (t /= d) * t * t + b;
}

float Honeybear::EaseOutCubic(float t, float b, float c, float d)
{
    return c * ((t = t / d - 1) * t * t + 1) + b;
}

float Honeybear::EaseInOutCubic(float t, float b, float c, float d)
{
    if ((t /= d / 2) < 1)
    {
        return c / 2 * t * t * t + b;
    }
    return c / 2 * ((t -= 2) * t * t + 2) + b;
}

float Honeybear::EaseInBack(float t, float b, float c, float d)
{
    float s = 1.70158f;
	float postFix = t /= d;
	return c * (postFix) * t * ((s + 1) * t - s) + b;
}

float Honeybear::EaseOutBack(float t, float b, float c, float d)
{
    float s = 1.70158f;
	return c * ((t = t / d - 1) * t * ((s + 1) * t + s) + 1) + b;
}

float Honeybear::EaseInOutBack(float t, float b, float c, float d)
{
    float s = 1.70158f;
	if ((t /= d/2) < 1)
    {
        return c / 2 * (t * t * (((s *= (1.525f)) + 1) * t - s)) + b;
    }
	float postFix = t -= 2;
	return c / 2 * ((postFix) * t * (((s *= (1.525f)) + 1) * t + s) + 2) + b;
}

void Honeybear::Interp(float& value, const float a, const float b, const double t)
{
    value = a + (b - a) * t;
}

void Honeybear::Interp(Vec2& value, const Vec2& a, const Vec2& b, const double t)
{
    Interp(value.x, a.x, b.x, t);
    Interp(value.y, a.y, b.y, t);
}

void Honeybear::Interp(Vec3& value, const Vec3& a, const Vec3& b, const double t)
{
    Interp(value.x, a.x, b.x, t);
    Interp(value.y, a.y, b.y, t);
    Interp(value.z, a.z, b.z, t);
}

void Honeybear::Interp(Vec4& value, const Vec4& a, const Vec4& b, const double t)
{
    Interp(value.x, a.x, b.x, t);
    Interp(value.y, a.y, b.y, t);
    Interp(value.z, a.z, b.z, t);
    Interp(value.w, a.w, b.w, t);
}