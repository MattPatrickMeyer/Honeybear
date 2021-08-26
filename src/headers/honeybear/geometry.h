#ifndef GEOMETRY
#define GEOMETRY

#include <vector>

#include "maths.h"

namespace Honeybear
{
    struct AABB
    {
        Vec2 position;
        Vec2 size;

        AABB(const float x, const float y, const Vec2& size);
        AABB(const Vec2& position, const Vec2& size);
    };

    struct Rect
    {
        Vec2 position;
        float width;
        float height;

        Rect(const float x, const float y, const float width, const float height);
        Rect(const Vec2& pos, const Vec2& size);

        float Left() const;
        float Right() const;
        float Top() const;
        float Bottom() const;

        bool Contains(Rect& other) const;
    };

    struct Line
    {
        Vec2 start;
        Vec2 end;
        Line();
        Line(const float x1, const float y1, const float x2, const float y2);
        Line(const Vec2 start, const Vec2 end);

        float Length();
        float DifferenceX();
        float DifferenceY();
        bool GetIntersection(Line other, Vec2& intersection);

        Vec2 Direction();

        bool operator==(const Line& other) const;
        bool operator<(const Line& other) const;
    };

    struct Polygon
    {
        std::vector<Vec2> points;
        std::vector<Line> edges;

        Polygon();
        Polygon(const float x, const float y, const float w, const float h);
        Polygon(const std::vector<Vec2>& points);

        std::vector<Vec2> GetEdgeNormals();
        Polygon CloneToLocation(const float x, const float y);
        Vec2 ClosestPoint(const Vec2 point);
        Vec2 Center();
    };

    struct Circle
    {
        Vec2 position;
        float radius;

        Circle();
        Circle(const Vec2 position, const float radius);
        Circle(const float x, const float y, const float radius);
    };

    struct Projection
    {
        float min;
        float max;

        Projection(const float min, const float max);

        bool Overlaps(const Projection& other);
        float GetOverlap(const Projection& other);
    };

    Rect RectFromAABB(const AABB& aabb);

    bool RectContainsPoint(const Rect& rect, const Vec2 point);
    bool RectsOverlap(const Rect& a, const Rect& b);

    Projection ProjectToAxis(const Circle& circle, Vec2 axis);
    Projection ProjectToAxis(const Polygon& polygon, Vec2 axis);
    Projection ProjectToAxis(const Line& line, Vec2 axis);

    // todo: remove this maybe
    struct PolarCompare
    {
        Vec2 pivot;
        PolarCompare(const Vec2& pivot);
        bool operator() (const Vec2& a, const Vec2& b);
    };
};

#endif