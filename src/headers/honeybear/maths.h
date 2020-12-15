#ifndef MATHS_H
#define MATHS_H

namespace Honeybear
{
    struct Vec2
    {
        float x;
        float y;

        Vec2();
        Vec2(const float value);
        Vec2(const float x, const float y);
    };

    struct Vec3
    {
        float x;
        float y;
        float z;

        Vec3();
        Vec3(const float value);
        Vec3(const float x, const float y, const float z);
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
    };
};

#endif