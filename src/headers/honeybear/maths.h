#ifndef MATHS_H
#define MATHS_H

#define PI 3.14159265358979323846

namespace Honeybear
{
    struct Vec2
    {
        float x;
        float y;

        Vec2();
        Vec2(const float value);
        Vec2(const float x, const float y);

        static Vec2 ZERO;

        // todo: remove member functions
        float Dot(const Vec2& other) const;
        float CrossProduct(const Vec2& other) const;
        float Magnitude() const;
        float SquaredMagnitude() const;
        float Distance(const Vec2& other) const;

        Vec2 Normalised() const;
        Vec2 Perp() const;
        Vec2 Negated() const;
        Vec2 Add(const Vec2& other) const;
        Vec2 Subtract(const Vec2& other) const;
        Vec2 Rotated(const Vec2& anchor_point, float angle_deg) const;
        Vec2 MovedInDirection(const Vec2& dir, float distance) const;

        bool NonZero() const;

        // todo: move
        bool operator==(const Vec2& other) const;
    };

    struct Vec3
    {
        float x;
        float y;
        float z;

        Vec3();
        Vec3(const float value);
        Vec3(const float x, const float y, const float z);

        static Vec3 ZERO;
    };

    struct Vec4
    {
        float x;
        float y;
        float z;
        float w;

        Vec4();
        Vec4(const float value);
        Vec4(const float x, const float y, const float z, const float w);

        static Vec4 ZERO;
    };

    float VectorToRadians(const Vec2& v);
    float VectorToDegrees(const Vec2& v);
    float RadiansToDegrees(const float radians);
    float DegreesToRadians(const float degrees);
    Vec2 RadiansToVector(const float radians);
    Vec2 DegreesToVector(const float degrees);

    void VectorToRadians(const Vec2& v, float& radians);
    void VectorToDegrees(const Vec2& v, float& degrees);
    void RadiansToDegrees(const float radians, float& degrees);
    void DegreesToRadians(const float degrees, float& radians);
    void RadiansToVector(const float radians, Vec2& v);
    void DegreesToVector(const float degrees, Vec2& v);

    float SmallestAngleDiff(const float a, const float b);

    // -------------------
    // ** INTERPOLATION **
    // -------------------

    void Interp(float& value, const float a, const float b, const double t);
    void Interp(Vec2& value, const Vec2& a, const Vec2& b, const double t);
    void Interp(Vec2& value, const Vec2& a, const Vec2& b, const Vec2& pivot_a, const Vec2& pivot_b, const double t);
    void Interp(Vec3& value, const Vec3& a, const Vec3& b, const double t);
    void Interp(Vec4& value, const Vec4& a, const Vec4& b, const double t);

    // ------------
    // ** EASING **
    // ------------
    enum EasingType
    {
        LINEAR,
        EASE_IN_CUBIC,
        EASE_OUT_CUBIC,
        EASE_IN_OUT_CUBIC,
        EASE_IN_BACK,
        EASE_OUT_BACK,
        EASE_IN_OUT_BACK
    };

    float Ease(float t, float b, float c, float d, EasingType easing_type);
    float EaseLinear(float t, float b, float c, float d);
    float EaseInCubic(float t, float b, float c, float d);
    float EaseOutCubic(float t, float b, float c, float d);
    float EaseInOutCubic(float t, float b, float c, float d);
    float EaseInBack(float t, float b, float c, float d);
    float EaseOutBack(float t, float b, float c, float d);
    float EaseInOutBack(float t, float b, float c, float d);

    // -------------------
    // ** VEC2 FUNCTIONS **
    // -------------------
    void Rotate(Vec2& value, const Vec2& origin, const float angle_deg);
    void Rotate(Vec2& value, const Vec2& origin, const float cos_angle, const float sin_angle);
    float Distance(const Vec2& a, const Vec2& b);

    // -------------------
    // ** VEC2 OPERATOR OVERLOADS **
    // -------------------

    bool operator<(const Vec2& a, const Vec2& b);

    Vec2 operator+(const Vec2& a, const Vec2& b);
    Vec2 operator+(const Vec2& a, const float b);
    Vec2 operator-(const Vec2& a, const Vec2& b);
    Vec2 operator-(const Vec2& a, const float b);
    Vec2 operator*(const Vec2& a, const Vec2& b);
    Vec2 operator*(const Vec2& a, const float b);
    Vec2 operator/(const Vec2& a, const Vec2& b);
    Vec2 operator/(const Vec2& a, const float b);

    Vec2& operator+=(Vec2& a, const Vec2& b);
    Vec2& operator+=(Vec2& a, const float b);
    Vec2& operator-=(Vec2& a, const Vec2& b);
    Vec2& operator-=(Vec2& a, const float b);
    Vec2& operator*=(Vec2& a, const Vec2& b);
    Vec2& operator*=(Vec2& a, const float b);
    Vec2& operator/=(Vec2& a, const Vec2& b);
    Vec2& operator/=(Vec2& a, const float b);
};

#endif
