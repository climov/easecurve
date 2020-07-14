#ifndef TRIG_H
#define TRIG_H

#include <cmath>

namespace trig {

template <typename Rep>
constexpr Rep pi = static_cast<Rep>(3.141592653589793238462643383);

//! convert degrees to radians
template <typename T>
constexpr T degToRad(T deg)
{
    return pi<T> / 180 * deg;
}

//! convert radians to degrees
template <typename T>
constexpr T radToDeg(T rad)
{
    return 180 / pi<T> * rad;
}

enum class AngleUnit
{
    Radians,
    Degrees,
};

template <typename Rep_, AngleUnit unit_>
struct Angle
{
    using Rep = Rep_;
    static constexpr AngleUnit unit = unit_;
    static constexpr AngleUnit otherUnit = (unit == AngleUnit::Radians ? AngleUnit::Degrees : AngleUnit::Radians);
    static constexpr Rep divisor = (unit == AngleUnit::Radians ? pi<Rep> : 180);
    static constexpr Angle fullCircle() { return {2 * divisor}; }

    Rep value = {};

    constexpr operator Angle<Rep, otherUnit>() { return {value * Angle<Rep, otherUnit>::divisor / divisor}; }
    constexpr Angle& operator +=(const Angle other) { value += other.value; return *this; }
    constexpr Angle& operator -=(const Angle other) { value -= other.value; return *this; }
    constexpr Angle& operator *=(const Rep factor) { value *= factor; return *this; }
    constexpr Angle* operator /=(const Rep factor) { value /= factor; return *this; }
};

template <typename Rep>
using Rad = Angle<Rep, AngleUnit::Radians>;
using RadF = Rad<float>;

template <typename Rep>
using Deg = Angle<Rep, AngleUnit::Degrees>;
using DegF = Deg<float>;

template <typename Angle1, typename Angle2>
constexpr Angle1 cast(const Angle2 angle)
{
    return Angle1 {static_cast<typename Angle1::Rep>(angle.value * Angle1::divisor / Angle2::divisor)};
}

template <typename Rep1, typename Rep2, AngleUnit unit>
constexpr auto operator +(const Angle<Rep1, unit> a1, const Angle<Rep2, unit> a2)
{
    using Rep = std::common_type_t<Rep1, Rep2>;
    return Angle<Rep, unit> {a1.value + a2.value};
}

template <typename Rep1, typename Rep2, AngleUnit unit>
constexpr auto operator -(const Angle<Rep1, unit> a1, const Angle<Rep2, unit> a2)
{
    using Rep = std::common_type_t<Rep1, Rep2>;
    return Angle<Rep, unit> {a1.value - a2.value};
}

template <typename Rep, AngleUnit unit>
constexpr Angle<Rep, unit> operator +(const Angle<Rep, unit> a)
{
    return a;
}

template <typename Rep, AngleUnit unit>
constexpr Angle<Rep, unit> operator -(const Angle<Rep, unit> a)
{
    return {-a.value};
}

template <typename Rep1, typename Rep2, AngleUnit unit>
constexpr auto operator *(const Angle<Rep1, unit> angle, Rep2 factor)
{
    using Rep = std::common_type_t<Rep1, Rep2>;
    return Angle<Rep, unit> {angle.value * factor};
}

template <typename Rep1, typename Rep2, AngleUnit unit>
constexpr auto operator *(Rep2 factor, const Angle<Rep1, unit> angle)
{
    using Rep = std::common_type_t<Rep1, Rep2>;
    return Angle<Rep, unit> {angle.value * factor};
}

template <typename Rep1, typename Rep2, AngleUnit unit>
constexpr auto operator /(const Angle<Rep1, unit> angle, Rep2 divisor)
{
    using Rep = std::common_type_t<Rep1, Rep2>;
    return Angle<Rep, unit> {angle.value / divisor};
}

template <typename Rep, AngleUnit unit>
constexpr bool operator <(const Angle<Rep, unit> a1, const Angle<Rep, unit> a2)
{
    return a1.value < a2.value;
}

template <typename Rep, AngleUnit unit>
constexpr bool operator >(const Angle<Rep, unit> a1, const Angle<Rep, unit> a2)
{
    return a1.value > a2.value;
}

template <typename Ang> auto     sin(Ang ang)                { return std::sin(cast<Rad<typename Ang::Rep>>(ang).value); }
template <typename Ang> auto     cos(Ang ang)                { return std::cos(cast<Rad<typename Ang::Rep>>(ang).value); }
template <typename Ang> auto     tan(Ang ang)                { return std::tan(cast<Rad<typename Ang::Rep>>(ang).value); }
template <typename Rep> Rad<Rep> asin(Rep value)             { return {static_cast<Rep>(std::asin(value))}; }
template <typename Rep> Rad<Rep> acos(Rep value)             { return {static_cast<Rep>(std::acos(value))}; }
template <typename Rep> Rad<Rep> atan2(Rep valY, Rep valX)   { return {static_cast<Rep>(std::atan2(valY, valX))}; }

inline namespace literals {

constexpr RadF operator ""_radf(long double v) { return RadF {static_cast<float>(v)}; }
constexpr RadF operator ""_radf(unsigned long long int v) { return RadF {static_cast<float>(v)}; }
constexpr DegF operator ""_degf(long double v) { return DegF {static_cast<float>(v)}; }
constexpr DegF operator ""_degf(unsigned long long int v) { return DegF {static_cast<float>(v)}; }

} // namespace literals

} // namespace trig

#endif // TRIG_H
