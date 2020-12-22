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

        float Dot(const Vec2& other);
        float Magnitude();
        float Distance(const Vec2& other);

        Vec2 Normalised();
        Vec2 Perp();
        Vec2 Negated();
        Vec2 Subtract(const Vec2& other) const;
        Vec2 Rotated(const Vec2& anchor_point, float angle_deg);
        Vec2 MovedInDirection(const Vec2& dir, float distance);

        bool NonZero();

        bool operator==(const Vec2& other) const;
        Vec2 operator-(const Vec2& other) const;
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
};

#endif