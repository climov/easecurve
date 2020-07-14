#ifndef V_ALGEBRA_H
#define V_ALGEBRA_H

#include "trig.h"

#include <SFML/System/Vector2.hpp>
//#include <SFML/System/Vector3.hpp>

#include <vector>

namespace va {

template <typename T, typename T1>
sf::Vector2<T> cast(const sf::Vector2<T1> v)
{
    return {static_cast<T>(v.x), static_cast<T>(v.y)};
}

//! dot product
template <typename T>
T dot(const sf::Vector2<T> v1, const sf::Vector2<T> v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

//! squared length
template <typename T>
T sqLen(const sf::Vector2<T> v)
{
    return dot(v, v);
}

//! length
template <typename T>
float len(const sf::Vector2<T> v)
{
    return std::sqrt(sqLen(v));
}

//! change this vector to a unit vector with the same direction and orientation
//! zero vectors will cause division by 0
template <typename T>
sf::Vector2<T>& makeUnit(sf::Vector2<T>& v)
{
    v /= len(v);
    return v;
}

//! change this vector to a unit vector with the same direction and orientation
//! zero vectors are transformed into zero vectors
template <typename T>
sf::Vector2<T>& makeUnitOrZero(sf::Vector2<T>& v)
{
    if (const T l = len(v); l > 0) {
        v /= l;
    } else {
        v = {};
    }
    return v;
}

//! calculate the unit vector (same direction, same orientation, length of 1)
//! zero vectors will cause division by 0
template <typename T>
sf::Vector2<T> unit(const sf::Vector2<T> v)
{
    return v / len(v);
}
//! calculate the unit vector (same direction, same orientation, length of 1)
//! for a zero vector it will return a zero vector
template <typename T>
sf::Vector2<T> unitOrZero(const sf::Vector2<T> v)
{
    if (const T l = len(v); l > 0) {
        return v / l;
    }
    return {};
}

template <typename T, trig::AngleUnit AU>
sf::Vector2<T> unit(const trig::Angle<T, AU> ang)
{
    return {trig::cos(ang), trig::sin(ang)};
}

//! set the length of the vector without changing the angle
//! zero vectors will cause a division by 0
template <typename T>
sf::Vector2<T>& setLen(sf::Vector2<T>& v, const T newLen)
{
    v *= newLen / len(v);
    return v;
}

//! set the length of the vector without changing the angle
//! zero vectors will return a zero vector
template <typename T>
sf::Vector2<T>& setLenOrZero(sf::Vector2<T>& v, const T newLen)
{
    if (const T l = len(v); l > 0) {
        v *= newLen / l;
    } else {
        v = {};
    }
    return v;
}

//! get a vector with the same angle and the provided length
//! zero vectors will cause a division by 0
template <typename T>
sf::Vector2<T> withLen(const sf::Vector2<T> v, const T newLen)
{
    return v * newLen / len(v);
}

//! get a vector with the same angle and the provided length
//! zero vectors will cause a division by 0
template <typename T>
sf::Vector2<T> withLenOrZero(const sf::Vector2<T> v, const T newLen)
{
    if (const T l = len(v); l > 0) {
        return v * newLen / l;
    }
    return {};
}

//! Returns the angle of this vector
//! The vector (1,0) corresponds 0 degrees, (0,1) corresponds 90 degrees.
template <typename T>
trig::Rad<T> angle(const sf::Vector2<T> v)
{
    return trig::atan2(v.y, v.x);
}

//! Sets the angle of the vector while keeping the length
//! The vector (1,0) corresponds 0 degrees, (0,1) corresponds 90 degrees.
template <typename T, typename Ang>
sf::Vector2<T>& setAngle(sf::Vector2<T>& v, const Ang newAngle)
{
    const T vecLen = len(v);
    v.x = vecLen * trig::cos(newAngle);
    v.y = vecLen * trig::sin(newAngle);
    return v;
}

//! Returns a new vector with the given angle and same length
//! The vector (1,0) corresponds 0 degrees, (0,1) corresponds 90 degrees.
template <typename T, typename Ang>
sf::Vector2<T> withAngle(const sf::Vector2<T> v, const Ang newAngle)
{
    const T l = len(v);
    return {l * trig::cos(newAngle), l * trig::sin(newAngle)};
}

//! Returns a vector rotated by the given angle
//! The vector (1,0) corresponds 0 degrees, (0,1) corresponds 90 degrees.
template <typename T, typename Ang>
sf::Vector2<T> rotated(const sf::Vector2<T> v, const Ang angle)
{
    const T cos = trig::cos(angle);
    const T sin = trig::sin(angle);
    return {cos * v.x - sin * v.y, sin * v.x + cos * v.y};
}

//! Rotates the vector by the given angle
//! The vector (1,0) corresponds 0 degrees, (0,1) corresponds 90 degrees.
template <typename T, typename Ang>
sf::Vector2<T>& rotate(sf::Vector2<T>& v, const Ang angle)
{
    v = rotated(v, angle);
}

//! Returns a perpendicular vector.
//! Returns a vector turned by 90 degrees counter clockwise; (x,y) becomes (-y,x).
template <typename T>
sf::Vector2<T> perpendicularCcw(const sf::Vector2<T> v)
{
    return {-v.y, v.x};
}

//! Returns a perpendicular vector.
//! Returns a vector turned by 90 degrees counter clockwise; (x,y) becomes (-y,x).
template <typename T>
sf::Vector2<T> perpendicularCw(const sf::Vector2<T> v)
{
    return {v.y, -v.x};
}

//! Computes the cross product of two 2D vectors (Z component only).
//! Treats the operands as 3D vectors, computes their cross product and returns the result's Z component
//! (X and Y components are always zero).
template <typename T>
T cross(const sf::Vector2<T> v1, const sf::Vector2<T> v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

//! Computes the signed angle from v1 to v2.
//! The angle determines how much you have to turn v1 until it points to the same direction as v2.
template <typename T>
trig::Rad<T> signedAngle(const sf::Vector2<T> v1, const sf::Vector2<T> v2)
{
    return trig::atan2(cross(v1, v2), dot(v1, v2));
}

//! Returns the projection of v onto axis.
//! Axis needs not be a unit vector, but if it's zero it will cause a division by zero
template <typename T>
sf::Vector2<T> projected(const sf::Vector2<T> v, const sf::Vector2<T> axis)
{
    return dot(v, axis) / sqLen(axis) * axis;
}

//template <typename T>
//sf::Vector2<T>& invertScale(sf::Vector2<T>& scale)
//{
//    scale.x = 1 / scale.x;
//    scale.y = 1 / scale.y;
//    return scale;
//}

//! Returns the inverted scale for the given scale ( { 1 / x, 1 / y } )
//! If the scale is zero, it will cause division by 0
template <typename T>
sf::Vector2<T> invertedScale(const sf::Vector2<T> scale)
{
    return {1 / scale.x, 1 / scale.y};
}

//! Returns the vector scaled component-wise by the given scale ( {x * scale.x, y * scale.y} )
template <typename T>
sf::Vector2<T> scaled(const sf::Vector2<T> v, const sf::Vector2<T> scale)
{
    return {v.x * scale.x, v.y * scale.y};
}

//! returns the triple vector products
template <typename T>
sf::Vector2<T> tripleProduct(const sf::Vector2<T> a, const sf::Vector2<T> b, const sf::Vector2<T> c)
{
    return b * dot(a, c) - a * dot(b, c);
}

//! true if the points form a convex polygon, false otherwise
template <typename T>
bool isConvex(const std::vector<sf::Vector2<T>>& points)
{
    if (points.size() < 3) {
        return false;
    }

    int wSign = 0; // first nonzero orientation (positive or negative)

    int xSign = 0;
    int xFirstSign = 0; // sign of first nonzero edge vector x
    int xFlips = 0; // number of sign changes in x

    int ySign = 0;
    int yFirstSign = 0; // sign of first nonzero edge vector y
    int yFlips = 0; // number of sign changes in y

    sf::Vector2f prev = points[points.size() - 2]; // second-to-last vertex
    sf::Vector2f curr = points.back(); // last vertex

    for (const sf::Vector2f& next: points) {
        // previous edge vector ("before")
        const sf::Vector2f b = curr - prev;
        // next edge vector ("after")
        const sf::Vector2f a = next - curr;

        // Calculate sign flips using the next edge vector ("after"),
        // recording the first sign.
        if (a.x > 0) {
            if (xSign == 0) {
                xFirstSign = 1;
            } else if (xSign < 0) {
                ++xFlips;
            }
            xSign = 1;
        } else if (a.x < 0) {
            if (xSign == 0) {
                xFirstSign = -1;
            } else if (xSign > 0) {
                ++xFlips;
            }
            xSign = -1;
        }
        if (xFlips > 2) {
            return false;
        }

        if (a.y > 0) {
            if (ySign == 0) {
                yFirstSign = 1;
            } else if (ySign < 0) {
                ++yFlips;
            }
            ySign = 1;
        } else if (a.y < 0) {
            if (ySign == 0) {
                yFirstSign = -1;
            } else if (ySign > 0) {
                ++yFlips;
            }
            ySign = -1;
        }
        if (yFlips > 2) {
            return false;
        }

        auto signum = [](auto x) -> int { return x > 0 ? 1 : x < 0 ? 1 : 0; };
        // find out the orientation of this pair of edges,
        // and ensure it does not differ from previous ones.
        const int w = signum(cross(a, b));
        if (!wSign) {
            wSign = w;
        } else if (wSign != w) {
            return false;
        }

        prev = curr; // current vertex
        curr = next; // next vertex
    }

    // final/wraparound sign flips:
    if (xSign && xFirstSign && xSign != xFirstSign) {
        ++xFlips;
    }
    if (ySign && yFirstSign && ySign != yFirstSign) {
        ++yFlips;
    }

    // convex polygons have two sign flips along each axis.
    return (xFlips == 2) && (yFlips == 2);
}

#if 0
//! dot product
template <typename T>
T dot(const sf::Vector3<T> v1, const sf::Vector3<T> v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

//! squared length.
template <typename T>
T sqLen(const sf::Vector3<T> v)
{
    return dot(v, v);
}

//! length
template <typename T>
T len(const sf::Vector3<T>& v)
{
    return std::sqrt(sqLen(v));
}

//! change this vector to a unit vector with the same direction and orientation
//! zero vectors will cause division by 0
template <typename T>
sf::Vector3<T>& makeUnit(sf::Vector3<T>& v)
{
    v /= len(v);
    return v;
}

//! change this vector to a unit vector with the same direction and orientation
//! zero vectors are transformed into zero vectors
template <typename T>
sf::Vector3<T>& makeUnitOrZero(sf::Vector3<T>& v)
{
    if (const T l = len(v); l > 0) {
        v /= l;
    } else {
        v = {};
    }
    return v;
}

//! calculate the unit vector (same direction, same orientation, length of 1)
//! zero vectors will cause division by 0
template <typename T>
sf::Vector3<T> unit(const sf::Vector3<T> v)
{
    return v / len(v);
}
//! calculate the unit vector (same direction, same orientation, length of 1)
//! for a zero vector it will return a zero vector
template <typename T>
sf::Vector3<T> unitOrZero(const sf::Vector3<T> v)
{
    if (const T l = len(v); l > 0) {
        return v / l;
    }
    return {};
}

//! Returns the angle between the projection of v onto the XY-plane and the X-axis
template <typename T>
trig::Rad<T> polarAngle(const sf::Vector3<T> v)
{
    return trig::atan2(v.y, v.x);
}

//! Returns a 2D vector from the x & y components
template <typename T>
sf::Vector2<T> xy(const sf::Vector3<T> v)
{
    return {v.x, v.y};
}

//! Returns the elevation angle of v
//! The elevation angle is the angle between the vector and the XY-plane. It has the same sign as @a vector.z.
template <typename T>
trig::Rad<T> elevationAngle(const sf::Vector3<T> v)
{
	return trig::atan2(v.z, len(xy(v)));
}

//! cross product
template <typename T>
sf::Vector3<T> cross(const sf::Vector3<T> v1, const sf::Vector3<T> v2)
{
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

//! Returns the inverted scale for the given scale ( { 1 / x, 1 / y, 1 / z } )
//! If the scale is zero, it will cause division by 0
template <typename T>
sf::Vector3<T> invertedScale(const sf::Vector3<T> scale)
{
    return {1 / scale.x, 1 / scale.y, 1 / scale.z};
}

//! Returns the vector scaled component-wise by the given scale ( {x * scale.x, y * scale.y, z * scale.z} )
template <typename T>
sf::Vector3<T> scaled(const sf::Vector3<T> v, const sf::Vector3<T> scale)
{
    return {v.x * scale.x, v.y * scale.y, v.z * scale.z};
}

//! @brief Constructs a 3D vector from a 2D vector by setting the Z component to zero.
//!
template <typename T>
sf::Vector3<T> toVector3(const sf::Vector2<T> v2)
{
    return {v2.x, v2.y, {}};
}
#endif

} // namespace va

#endif // V_ALGEBRA_H
