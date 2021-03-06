#include <algorithm>
#include "geometry.h"

using namespace Honeybear;

namespace
{
    void GenEdgeList(Polygon& polygon)
    {
        for(size_t i = 0; i < polygon.points.size(); ++i)
        {
            polygon.edges.emplace_back(polygon.points[i], polygon.points[(i + 1) % polygon.points.size()]);
        }
    }
}

AABB::AABB(const float x, const float y, const Vec2& size) : 
    position(x, y),
    size(size)
{}

AABB::AABB(const Vec2& position, const Vec2& size) :
    position(position),
    size(size)
{}

Rectangle::Rectangle(const float x, const float y, const float width, const float height) :
    position(x, y),
    width(width),
    height(height)
{}

float Rectangle::Left() const
{
    return position.x;
}

float Rectangle::Right() const
{
    return position.x + width;
}

float Rectangle::Top() const
{
    return position.y;
}

float Rectangle::Bottom() const
{
    return position.y + height;
}

bool Rectangle::Contains(Rectangle& other) const
{
    if(other.Left() < Left())
        return false;
    if(other.Right() > Right())
        return false;
    if(other.Top() < Top())
        return false;
    if(other.Bottom() > Bottom())
        return false;
    return true;
}

bool Honeybear::RectangleContainsPoint(const Rectangle& rectangle, const Vec2 point)
{
    if(point.x < rectangle.Left())   return false;
    if(point.x > rectangle.Right())  return false;
    if(point.y < rectangle.Top())    return false;
    if(point.y > rectangle.Bottom()) return false;
    return true;
}

Circle::Circle() :
    position(0.0f),
    radius(0.0f)
{}

Circle::Circle(const Vec2 position, const float radius) : 
    position(position),
    radius(radius)
{}

Circle::Circle(const float x, const float y, const float radius) :
    position(x, y),
    radius(radius)
{}

Projection::Projection(const float min, const float max) :
    min(min),
    max(max)
{}

bool Projection::Overlaps(const Projection& other)
{
    return !(min > other.max || other.min > max);
}

float Projection::GetOverlap(const Projection& other)
{
    if(Overlaps(other))
    {
        return std::min(max, other.max) - std::max(min, other.min);
    }
    return 0.0f;
}

Polygon::Polygon()
{}

Polygon::Polygon(const float x, const float y, const float w, const float h)
{
    points.emplace_back(x, y);
    points.emplace_back(x + w, y);
    points.emplace_back(x + w, y + h);
    points.emplace_back(x, y + h);

    GenEdgeList(*this);
}

Polygon::Polygon(const std::vector<Vec2>& points) :
    points(points)
{
    GenEdgeList(*this);
}

Vec2 Polygon::Center()
{
    float cx = 0.0f;
    float cy = 0.0f;

    for(size_t i = 0; i < points.size(); ++i)
    {
        cx += points[i].x;
        cy += points[i].y;
    }

    return Vec2(cx / points.size(), cy / points.size());
}

std::vector<Vec2> Polygon::GetEdgeNormals()
{
    std::vector<Vec2> normals;

    for(int i = 0; i < points.size(); i++)
    {
        Vec2 p1 = points[i];
        Vec2 p2 = points[(i + 1) % points.size()];
        Vec2 edge = p1.Subtract(p2);
        Vec2 normal = edge.Perp().Normalised();
        normals.push_back(normal);
    }

    return normals;
}

Polygon Polygon::CloneToLocation(const float x, const float y)
{
    std::vector<Vec2> new_points;

    for(int i = 0; i < points.size(); i++)
    {
        new_points.emplace_back(points[i].x + x, points[i].y + y);
    }

    return Polygon(new_points);
}

Vec2 Polygon::ClosestPoint(const Vec2 point)
{
    float min_distance = std::numeric_limits<float>::max();
    Vec2 closest_point(0.0f);

    for(int i = 0; i < points.size(); i++)
    {
        float distance = points[i].Subtract(point).Magnitude();
        if(distance < min_distance)
        {
            closest_point = points[i];
        }
    }

    return closest_point;
}

Line::Line(const float x1, const float y1, const float x2, const float y2) :
    start(x1, y1),
    end(x2, y2)
{}

Line::Line(const Vec2 start, const Vec2 end) :
    start(start),
    end(end)
{}

float Line::Length()
{
    return end.Subtract(start).Magnitude();
}

float Line::DifferenceX()
{
    return end.x - start.x;
}

float Line::DifferenceY()
{
    return end.y - start.y;
}

bool Line::GetIntersection(Line other, Vec2& intersection)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = end.x - start.x;
    s1_y = end.y - start.y;
    s2_x = other.end.x - other.start.x;
    s2_y = other.end.y - other.start.y;

    float s, t;
    s = (-s1_y * (start.x - other.start.x) + s1_x * (start.y - other.start.y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (start.y - other.start.y) - s2_y * (start.x - other.start.x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        intersection.x = start.x + (t * s1_x);
        intersection.y = start.y + (t * s1_y);
        return true;
    }

    return false; // No collision
}

Vec2 Line::Direction()
{
    return end.Subtract(start).Normalised();
}

Projection Honeybear::ProjectToAxis(const Circle& circle, Vec2 axis)
{
    float middle = axis.Dot(circle.position);
    return Projection(middle - circle.radius, middle + circle.radius);
}

Projection Honeybear::ProjectToAxis(const Polygon& polygon, Vec2 axis)
{
    float min = axis.Dot(polygon.points[0]);
    float max = min;

    for(size_t i = 1; i < polygon.points.size(); ++i)
    {
        float p = axis.Dot(polygon.points[i]);
        if(p < min) min = p;
        else if(p > max) max = p;
    }

    return Projection(min, max);
}

Projection Honeybear::ProjectToAxis(const Line& line, Vec2 axis)
{
    float start_p = axis.Dot(line.start);
    float end_p = axis.Dot(line.end);
    if(start_p < end_p)
    {
        return Projection(start_p, end_p);
    }
    else
    {
        return Projection(end_p, start_p);
    }
}

bool Line::operator==(const Line& other) const
{
    return (start == other.start || start == other.end)
        && (end == other.start || end == other.end);
}

bool Line::operator<(const Line& other) const
{
    return !(*this == other);
}

Honeybear::PolarCompare::PolarCompare(const Vec2& pivot) :
    pivot(pivot)
{}

bool Honeybear::PolarCompare::operator() (const Vec2& a, const Vec2& b)
{
    Line pa(pivot, a);
    Line pb(pivot, b);

    float angle_a = VectorToDegrees(pa.Direction());
    float angle_b = VectorToDegrees(pb.Direction());

    if(angle_a < 0.0f) angle_a += 360.0f;
    if(angle_b < 0.0f) angle_b += 360.0f;

    return angle_a < angle_b;
}

Rectangle Honeybear::RectangleFromAABB(const AABB& aabb)
{
    return Rectangle(
        aabb.position.x - aabb.size.x,
        aabb.position.y - aabb.size.y,
        aabb.size.x * 2,
        aabb.size.y * 2);
}