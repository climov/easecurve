export module main.easecurve;

import std;
import alx.trig;
import alx.va;

namespace trig = alx::trig;
namespace va = alx::va;

export class EaseCurve
{
public: // ctors, dtors, copy and move
    EaseCurve() = default;
    ~EaseCurve() = default;

    EaseCurve(const EaseCurve& ) = default;
    EaseCurve(      EaseCurve&&) = default;

    EaseCurve& operator=(const EaseCurve& ) = default;
    EaseCurve& operator=(      EaseCurve&&) = default;

public: // setters

    //! Set the position of the last point. The last point is also the maximum x & y for the curve
    void setLastPoint(const float x, const float y) { _lastPoint = {x, y}; solve(); }

    //! Set the "global" radius for the curve circles. This gets copied into every circle.
    void setRadius(const float radius) { _radius = radius; setRadius(); }

    //! Set the horizontal stretch factor for the ellipses. A factor of 2 means the horizonal radius is twice as large.
    void setXStretch(const float stretch) { _xStretch = stretch; solve(); }

    //! Set the radius of the first point
    void setFirstRadius(const float radius) { _firstRadius = radius; solve(); }

    //! Set the radius of the last point
    void setLastRadius(const float radius) { _lastRadius = radius; solve(); }

    //! Add an intermediate point at the end (right before the last point) and sets its circle radius.
    void addPoint(float x, float y, float radius);
    //! Add an intermediate point at the end (right before the last point) using the global circle radius.
    void addPoint(const float x, const float y) { addPoint(x, y, _radius); }

    //! Inserts an intermediate point at a certain index (0 is the first one after the inital (0, 0) point)
    void insertPointAt(int index, float x, float y, float radius);
    //! Inserts an intermediate point at a certain index (0 is the first one after the inital (0, 0) point)
    void insertPointAt(const int index, const float x, const float y) { insertPointAt(index, x, y, _radius); }

    //! Remove the intermediate point at the given index
    void removePointAt(int index);

public:
    //! Evaluate for the given x and return associated y value
    [[nodiscard]] float evaluate(float x) const;
    //! Get the speed of the curve for the given x
    [[nodiscard]] float speed(float x) const;
    //! Get the acceleration of the curve for the given x
    [[nodiscard]] float accel(float x) const;

public: // internal (private)
    void setRadius();
    void solve();

public: // internal (private)
    using Point = va::Vec2f;

    // internal helper structs
    enum class Side
    {
        Auto,
        ForceTrue,
        ForceFalse,
    };
    enum class SegmentType
    {
        Circle,
        Line,
    };
    struct Circle
    {
        // y(x) = center.y + sign * sqrt(sqRadius - (x - center.x)^2)
        // speed(x) = sign * (x - center.x) / sqrt(sqRadius - (x - center.x)^2)
        // accel(x) = sign * sqRadius / sqrt((sqRadius - (x - center.x)^2)^3)
        Point center;
        float sqRadius; // squared radius
        float sign;
    };
    struct Line
    {
        // y(x) = slope * x + deltaY
        // speed(x) = slope
        // accel(x) = 0
        float slope;
        float deltaY;
    };
    struct Segment
    {
        Segment(Circle c) : circle {c}, type {SegmentType::Circle} {}
        Segment(Line l) : line {l}, type {SegmentType::Line} {}

        [[nodiscard]] float evaluate(float x) const;
        [[nodiscard]] float speed(float x) const;
        [[nodiscard]] float accel(float x) const;

        [[nodiscard]] float initialY() const { return evaluate(initialX); }
        [[nodiscard]] float finalY() const { return evaluate(finalX); }

        float initialX = 0;
        float finalX = 0;
        union {
            Circle circle;
            Line   line;
        };
        SegmentType type;
    };

public: // internal (private)
    // params
    float _xStretch = 1.0f;
    float _minDistance = kMinDistance;
    float _precision = kPrecision;
    float _radius = kDefaultRadius;
    float _firstRadius = kDefaultRadius;
    float _lastRadius = kDefaultRadius;
    Point _lastPoint = kDefaultLastPoint;
    std::vector<Point> _points;
    std::vector<float> _radii;

    // calculated by invoking solve()
    Point _scaledFirstCenter;
    Point _scaledLastCenter;
    float _firstReducedRadius;
    float _lastReducedRadius;
    std::vector<Point> _scaledCenters;
    std::vector<bool> _sides;
    std::vector<bool> _originalSides;
    std::vector<Side> _autoSides;
    std::vector<float> _reducedRadii;

    // debugging information
    long long _solveTimeUs = 0;
    int _iterations = 0;

    // needed for evaluate(), speed() and accel()
    std::vector<Segment> _scaledSegments;

    // calculated by invoking validate()
    std::vector<bool> _pointsXValid;
    std::vector<bool> _pointsYValid;
    bool _valid = true;

    // more params
    bool _autoFlip = true;
    bool _reduceRadii = true;

public: // internal (private)
    // default values
    static constexpr Point kFirstPoint {0.f, 0.f};
    static constexpr Point kDefaultLastPoint {10.f, 10.f};
    static constexpr float kDefaultRadius = 1.f;
    static constexpr float kMinDistance = 0.01f;
    static constexpr float kPrecision = 0.000001f;
};

namespace {

void validate(EaseCurve& curve)
{
    curve._valid = true;

    auto count = curve._points.size();
    EaseCurve::Point prevPoint = EaseCurve::kFirstPoint;
    // check that points are in ascending order
    for (decltype (count) i = 0; i < count; ++i) {
        const EaseCurve::Point point = curve._points[i];
        // check each x is in [0, lastX] and (prevPoint.x, lastX]
        if (!(curve._pointsXValid[i] = !(point.x() < 0.f || point.x() > curve._lastPoint.x()) && point.x() > prevPoint.x())) {
            curve._valid = false;
        }
        // check each y is in [0, lastY] and [prevPoint.y, lastY]
        if (!(curve._pointsYValid[i] = !(point.y() < 0.f || point.y() > curve._lastPoint.y()) && point.y() > prevPoint.y())) {
            curve._valid = false;
        }
        prevPoint = point;
    }
    if (count) {
        // check last x and y are not less than end point
        if (!(curve._pointsXValid.back() = curve._pointsXValid.back() && prevPoint.x() < curve._lastPoint.x())) {
            curve._valid = false;
        }
        if (!(curve._pointsYValid.back() = curve._pointsYValid.back() && prevPoint.y() < curve._lastPoint.y())) {
            curve._valid = false;
        }
    }
}

struct SolveResult
{
    EaseCurve::Segment speedUp;
    EaseCurve::Segment cruise;
};

// solve for a pair of points/circles
inline SolveResult solve(const float initialX, const EaseCurve::Point leftCenter, const float leftRadius, const bool leftSide, const EaseCurve::Point rightCenter, const float rightRadius, const bool rightSide)
{
    const EaseCurve::Point delta = rightCenter - leftCenter;
    const trig::RadF deltaAngle = delta.angle();
    const float centerDistance = delta.len();
    const float speedUpSign = leftSide ? -1.f : 1.f;
    const float totalRadius = (leftSide == rightSide) ? std::abs(leftRadius - rightRadius) : leftRadius + rightRadius;
    const trig::RadF tangentToDeltaAngle = trig::asin(totalRadius / centerDistance);

    trig::RadF leftAngle = trig::Zero<trig::RadF>;
    trig::RadF rightAngle = trig::Zero<trig::RadF>;
    if (leftSide == rightSide) {
        // both circles are on the same side
        if (leftSide) {
            // both above
            if (leftRadius <= rightRadius) {
                // smaller circle on the left
                leftAngle = deltaAngle - tangentToDeltaAngle - trig::Right<trig::RadF>;
            } else {
                // smaller circle on the right
                leftAngle = deltaAngle + tangentToDeltaAngle - trig::Right<trig::RadF>;
            }
        } else {
            // both below
            if (leftRadius <= rightRadius) {
                // smaller circle on the left
                leftAngle = deltaAngle + tangentToDeltaAngle + trig::Right<trig::RadF>;
            } else {
                // smaller circle on the right
                leftAngle = deltaAngle - tangentToDeltaAngle + trig::Right<trig::RadF>;
            }
        }
        rightAngle = leftAngle;
    } else {
        if (leftSide) {
            // left circle is above
            leftAngle = deltaAngle + tangentToDeltaAngle - trig::Right<trig::RadF>;
            rightAngle = leftAngle + trig::Straight<trig::RadF>;
        } else {
            // left circle is below
            leftAngle = deltaAngle - tangentToDeltaAngle + trig::Right<trig::RadF>;
            rightAngle = leftAngle - trig::Straight<trig::RadF>;
        }
    }

    const va::Vec2f leftTangent = leftCenter + va::Vec2f::fromPolar(leftAngle, leftRadius);
    const va::Vec2f rightTangent = rightCenter + va::Vec2f::fromPolar(rightAngle, rightRadius);
    const va::Vec2f tangentDelta = rightTangent - leftTangent;
    const float slope = tangentDelta.slope();
    const float deltaY = leftTangent.y() - slope * leftTangent.x();

    // add the circular segment corresponding to the left point/circle (accelerating)
    EaseCurve::Segment speedUp = EaseCurve::Circle {leftCenter, leftRadius * leftRadius, speedUpSign};
    speedUp.initialX = initialX;
    speedUp.finalX = leftTangent.x();

    // add the line segment corresponding to the straight line connecting the two circles (constant speed)
    EaseCurve::Segment cruise = EaseCurve::Line {slope, deltaY};
    cruise.initialX = leftTangent.x();
    cruise.finalX = rightTangent.x();

    return {speedUp, cruise};
}

bool reduceRadii(EaseCurve& curve, const EaseCurve::Point& leftPoint, EaseCurve::Point& leftCenter, float& leftRadius, bool leftSide, const EaseCurve::Point& rightPoint, EaseCurve::Point& rightCenter, float& rightRadius, bool rightSide)
{
    // check and reduce the radii if needed
    if (curve._reduceRadii && (leftRadius > 0.f || rightRadius > 0.f)) {
        if (leftSide == rightSide) {
            // both circles are on the same side
            const EaseCurve::Point pointDiff = rightPoint - leftPoint;
            const float minDistance = std::min(curve._minDistance, pointDiff.len());
            const bool reduceLeftSide = (leftRadius > 0.f) && (leftCenter - rightPoint).len() - leftRadius - minDistance < 0;
            if (reduceLeftSide) {
                // right point is "inside" the left circle
                const EaseCurve::Point pointDelta = leftPoint - rightPoint;
                const EaseCurve::Point leftDelta = leftCenter - leftPoint;
                float minRatio = 0.f;
                float maxRatio = 1.f;
                while (true) {
                    const float minDelta = (pointDelta + leftDelta * minRatio).len() - leftRadius * minRatio - minDistance;
                    const float maxDelta = (pointDelta + leftDelta * maxRatio).len() - leftRadius * maxRatio - minDistance;
                    const float midRatio = (minRatio + maxRatio) / 2;
                    if ((std::abs(minDelta + maxDelta) < curve._precision) || (midRatio <= minRatio) || (midRatio >= maxRatio)) {
                        leftRadius *= minRatio;
                        leftCenter = leftPoint + leftDelta * minRatio;
                        break;
                    }
                    const float midDelta = (pointDelta + leftDelta * midRatio).len() - leftRadius * midRatio - minDistance;
                    if (midDelta > 0.f) {
                        minRatio = midRatio;
                    } else {
                        maxRatio = midRatio;
                    }
                    ++curve._iterations;
                }
            }
            const bool reduceRightSide = (rightRadius > 0.f) && (rightCenter - leftPoint).len() - rightRadius - minDistance < 0;
            if (reduceRightSide) {
                // first point is "inside" the second circle
                const EaseCurve::Point pointDelta = rightPoint - leftPoint;
                const EaseCurve::Point rightDelta = rightCenter - rightPoint;
                float minRatio = 0.f;
                float maxRatio = 1.f;
                while (true) {
                    const float minDelta = (pointDelta + rightDelta * minRatio).len() - rightRadius * minRatio - minDistance;
                    const float maxDelta = (pointDelta + rightDelta * maxRatio).len() - rightRadius * maxRatio - minDistance;
                    const float midRatio = (minRatio + maxRatio) / 2;
                    if ((std::abs(minDelta + maxDelta) < curve._precision) || (midRatio <= minRatio) || (midRatio >= maxRatio)) {
                        rightRadius *= minRatio;
                        rightCenter = rightPoint + rightDelta * minRatio;
                        break;
                    }
                    const float midDelta = (pointDelta + rightDelta * midRatio).len() - rightRadius * midRatio - minDistance;
                    if (midDelta > 0.f) {
                        minRatio = midRatio;
                    } else {
                        maxRatio = midRatio;
                    }
                    ++curve._iterations;
                }
            }
            if (reduceLeftSide || reduceRightSide) {
                return true;
            }
        } else {
            // circles are on opposite sides
            const EaseCurve::Point leftDelta = leftCenter - leftPoint;
            const EaseCurve::Point rightDelta = rightCenter - rightPoint;
            const EaseCurve::Point centerDelta = rightCenter - leftCenter;
            const EaseCurve::Point pointDiff = rightPoint - leftPoint;
            const float totalRadius = leftRadius + rightRadius;
            const float minDistance = std::min(curve._minDistance, pointDiff.len());
            if (centerDelta.len() - totalRadius - minDistance < 0) {
                // circles are "overlapping"
                const EaseCurve::Point centerDiff = centerDelta - pointDiff;
                float minRatio = 0.f;
                float maxRatio = 1.f;
                while (true) {
                    const float minDelta = (pointDiff + centerDiff * minRatio).len() - totalRadius * minRatio - minDistance;
                    const float maxDelta = (pointDiff + centerDiff * maxRatio).len() - totalRadius * maxRatio - minDistance;
                    const float midRatio = (minRatio + maxRatio) / 2;
                    if ((std::abs(minDelta + maxDelta) < curve._precision) || (midRatio <= minRatio) || (midRatio >= maxRatio)) {
                        leftRadius *= minRatio;
                        leftCenter = leftPoint + leftDelta * minRatio;
                        rightRadius *= minRatio;
                        rightCenter = rightPoint + rightDelta * minRatio;
                        break;
                    }
                    const float midDelta = (pointDiff + centerDiff * midRatio).len() - totalRadius * midRatio - minDistance;
                    if (midDelta > 0.f) {
                        minRatio = midRatio;
                    } else {
                        maxRatio = midRatio;
                    }
                    ++curve._iterations;
                }
                return true;
            }
        }
        {
            const EaseCurve::Point leftDelta = leftCenter - leftPoint;
            const EaseCurve::Point rightDelta = rightCenter - rightPoint;
            const EaseCurve::Point pointDiff = rightPoint - leftPoint;
            const float minDistance = std::min(curve._minDistance, pointDiff.len());
            const SolveResult& res = ::solve(0, leftCenter, leftRadius, leftSide, rightCenter, rightRadius, rightSide);
#if 0
            if ((res.cruise.finalX - res.cruise.initialX - minDistance < 0) /*|| (res.cruise.finalY() - res.cruise.initialY() - minDistance < 0)*/) {
                // the constant speed segment goes "backwards"
                float minRatio = 0.f;
                float maxRatio = 1.f;
                while (true) {
                    const SolveResult& minRes = ::solve(0, leftPoint + leftDelta * minRatio, leftRadius * minRatio, leftSide, rightPoint + rightDelta * minRatio, rightRadius * minRatio, rightSide);
                    const SolveResult& maxRes = ::solve(0, leftPoint + leftDelta * maxRatio, leftRadius * maxRatio, leftSide, rightPoint + rightDelta * maxRatio, rightRadius * maxRatio, rightSide);
                    const float minXDelta = minRes.cruise.finalX - minRes.cruise.initialX - minDistance;
                    //const float minYDelta = minRes.cruise.finalY() - minRes.cruise.initialY() - minDistance;
                    const float maxXDelta = maxRes.cruise.finalX - maxRes.cruise.initialX - minDistance;
                    //const float maxYDelta = maxRes.cruise.finalY() - maxRes.cruise.initialY() - minDistance;
                    const float midRatio = (minRatio + maxRatio) / 2;
                    const bool degenerateCase = (minXDelta < 0.f) /*|| (minYDelta < 0.f)*/ || (midRatio <= minRatio) || (midRatio >= maxRatio);
                    const bool reachedPrecision = (std::abs(minXDelta + maxXDelta) < curve._precision) /*&& (std::abs(minYDelta + maxYDelta) < curve._precision)*/;
                    if (degenerateCase || reachedPrecision) {
                        leftRadius *= minRatio;
                        leftCenter = leftPoint + leftDelta * minRatio;
                        rightRadius *= minRatio;
                        rightCenter = rightPoint + rightDelta * minRatio;
                        break;
                    }
                    const SolveResult& midRes = ::solve(0, leftPoint + leftDelta * midRatio, leftRadius * midRatio, leftSide, rightPoint + rightDelta * midRatio, rightRadius * midRatio, rightSide);
                    const float midXDelta = midRes.cruise.finalX - midRes.cruise.initialX - minDistance;
                    //const float midYDelta = midRes.cruise.finalY() - midRes.cruise.initialY() - minDistance;
                    if ((midXDelta > 0.f) /*&& (midYDelta > 0.f)*/) {
                        minRatio = midRatio;
                    } else {
                        maxRatio = midRatio;
                    }
                    ++curve._iterations;
                }
                return true;
            }
#else
            if ((res.cruise.finalX - res.cruise.initialX - minDistance < 0) || (res.cruise.finalY() - res.cruise.initialY() - minDistance < 0)) {
                // the constant speed segment goes "backwards"
                float minRatio = 0.f;
                float maxRatio = 1.f;
                while (true) {
                    const SolveResult& minRes = ::solve(0, leftPoint + leftDelta * minRatio, leftRadius * minRatio, leftSide, rightPoint + rightDelta * minRatio, rightRadius * minRatio, rightSide);
                    const SolveResult& maxRes = ::solve(0, leftPoint + leftDelta * maxRatio, leftRadius * maxRatio, leftSide, rightPoint + rightDelta * maxRatio, rightRadius * maxRatio, rightSide);
                    const float minXDelta = minRes.cruise.finalX - minRes.cruise.initialX - minDistance;
                    const float minYDelta = minRes.cruise.finalY() - minRes.cruise.initialY() - minDistance;
                    const float maxXDelta = maxRes.cruise.finalX - maxRes.cruise.initialX - minDistance;
                    const float maxYDelta = maxRes.cruise.finalY() - maxRes.cruise.initialY() - minDistance;
                    const float midRatio = (minRatio + maxRatio) / 2;
                    const bool degenerateCase = (minXDelta < 0.f) || (minYDelta < 0.f) || (midRatio <= minRatio) || (midRatio >= maxRatio);
                    const bool reachedPrecision = (std::abs(minXDelta + maxXDelta) < curve._precision) && (std::abs(minYDelta + maxYDelta) < curve._precision);
                    if (degenerateCase || reachedPrecision) {
                        leftRadius *= minRatio;
                        leftCenter = leftPoint + leftDelta * minRatio;
                        rightRadius *= minRatio;
                        rightCenter = rightPoint + rightDelta * minRatio;
                        break;
                    }
                    const SolveResult& midRes = ::solve(0, leftPoint + leftDelta * midRatio, leftRadius * midRatio, leftSide, rightPoint + rightDelta * midRatio, rightRadius * midRatio, rightSide);
                    const float midXDelta = midRes.cruise.finalX - midRes.cruise.initialX - minDistance;
                    const float midYDelta = midRes.cruise.finalY() - midRes.cruise.initialY() - minDistance;
                    if ((midXDelta > 0.f) && (midYDelta > 0.f)) {
                        minRatio = midRatio;
                    } else {
                        maxRatio = midRatio;
                    }
                    ++curve._iterations;
                }
                return true;
            }
#endif
        }
    }
    return false;
};

} // namespace

void EaseCurve::addPoint(float x, float y, float radius)
{
    _points.push_back({x, y});
    _radii.push_back(radius);
    _pointsXValid.push_back(true);
    _pointsYValid.push_back(true);
    solve();
}

void EaseCurve::insertPointAt(int index, float x, float y, float radius)
{
    _points.insert(_points.begin() + index, {x, y});
    _radii.insert(_radii.begin() + index, radius);
    _pointsXValid.push_back(true);
    _pointsYValid.push_back(true);
    solve();
}

void EaseCurve::removePointAt(int index)
{
    _points.erase(_points.begin() + index);
    _radii.erase(_radii.begin() + index);
    _pointsXValid.pop_back();
    _pointsYValid.pop_back();
    solve();
}

void EaseCurve::setRadius()
{
    _firstRadius = _radius;
    _lastRadius = _radius;
    for (float& r: _radii) {
        r = _radius;
    }
    solve();
}

float EaseCurve::Segment::evaluate(float x) const
{
    switch (type) {
    case EaseCurve::SegmentType::Line:
        // y(x) = x * slope + deltaY
        return line.slope * x + line.deltaY;
    case EaseCurve::SegmentType::Circle: {
        // y(x) = center.y + sign * sqrt(sqRadius - (x - center.x)^2)
        const float deltaX = x - circle.center.x();
        return circle.center.y() + circle.sign * std::sqrt(circle.sqRadius - deltaX * deltaX);
    }
    }
    return 0.f;
}
float EaseCurve::Segment::speed(float x) const
{
    switch (type) {
    case EaseCurve::SegmentType::Line:
        return line.slope;
    case EaseCurve::SegmentType::Circle: {
        // speed(x) = -sign * (x - center.x) / sqrt(sqRadius - (x - center.x)^2)
        const float deltaX = x - circle.center.x();
        return - circle.sign * deltaX / std::sqrt(circle.sqRadius - deltaX * deltaX);
    }
    }
    return 0.f;
}
float EaseCurve::Segment::accel(float x) const
{
    switch (type) {
    case EaseCurve::SegmentType::Line:
        return 0.f;
    case EaseCurve::SegmentType::Circle: {
        // accel(x) = -sign * sqRadius / sqrt((sqRadius - (x - center.x)^2)^3)
        const float deltaX = x - circle.center.x();
        const float len = circle.sqRadius - deltaX * deltaX;
        return - circle.sign * circle.sqRadius / std::sqrt(len * len * len);
    }
    }
    return 0.f;
}

float EaseCurve::evaluate(float x) const
{
    x /= _xStretch;
    if (x < 0) {
        return 0.f;
    }
    for (const EaseCurve::Segment& segment: _scaledSegments) {
        if (x <= segment.finalX) {
            return segment.evaluate(x);
        }
    }
    return 0.f;
}

float EaseCurve::speed(float x) const
{
    x /= _xStretch;
    if (x < 0) {
        return 0.f;
    }
    for (const EaseCurve::Segment& segment: _scaledSegments) {
        if (x <= segment.finalX) {
            return segment.speed(x);
        }
    }
    return 0.f;
}

float EaseCurve::accel(float x) const
{
    x /= _xStretch;
    if (x < 0) {
        return 0.f;
    }
    for (const EaseCurve::Segment& segment: _scaledSegments) {
        if (x <= segment.finalX) {
            return segment.accel(x);
        }
    }
    return 0.f;
}

void EaseCurve::solve()
{
    const auto startTime = std::chrono::steady_clock::now();
    validate(*this);

    const float xStretch = _xStretch;
    auto unstretch = [xStretch](Point p) { return Point{p.x() / xStretch, p.y()}; };

    auto size = _points.size();

    _autoSides.clear();
    _autoSides.resize(size, Side::Auto);
    _originalSides.clear();
    _originalSides.resize(size, false);
    _firstReducedRadius = _firstRadius;
    _lastReducedRadius = _lastRadius;
    _reducedRadii = _radii;
    _iterations = 0;

    bool done = false;
    while (!done) {
        done = true;
        // figure out the centers and sides of the circles
        {
            _scaledCenters.clear();
            _scaledCenters.resize(size, {});
            _sides.clear();
            _sides.resize(size, false);

            _scaledFirstCenter = {kFirstPoint.x(), kFirstPoint.y() + _firstReducedRadius};
            _scaledLastCenter = {_lastPoint.x() / xStretch, _lastPoint.y() - _lastReducedRadius};

            for (decltype(size) i = 0; i < size; ++i) {
                const Point prevPoint = unstretch((i == 0) ? kFirstPoint : _points[i - 1]);
                const Point nextPoint = unstretch((i == size - 1) ? _lastPoint : _points[i + 1]);
                const Point currPoint = unstretch(_points[i]);
                // Figure out whether the current circle needs to go "above" or "below" the line segments.
                // In order to figure that out, we need to see whether the angle (ie. slope) of the consecutive segments
                // increases or decreases.
                // When the angle increases, the curve "goes up", so the circle needs to be "above"
                const trig::RadF leftAngle = (currPoint - prevPoint).angle();
                const trig::RadF rightAngle = (nextPoint - currPoint).angle();
                const bool above = (_autoSides[i] == Side::ForceTrue) || ((_autoSides[i] == Side::Auto) && (rightAngle > leftAngle));
                const trig::RadF signedPi = above ? trig::Straight<trig::RadF> : -trig::Straight<trig::RadF>;
                const trig::RadF centerAngle = (leftAngle + rightAngle + signedPi) / 2;
                _sides[i] = above;
                _scaledCenters[i] = currPoint + Point::fromPolar(centerAngle, _reducedRadii[i]);
                if (!_iterations) {
                    _originalSides[i] = above;
                }
            }
        }

        // split curve into segments
        {
            _scaledSegments.clear();

            // start at the first point
            Point prevPoint = unstretch(kFirstPoint);
            Point* prevCenter = &_scaledFirstCenter;
            float* prevRadius = &_firstReducedRadius;
            bool prevSide = true;
            float prevInitialX = 0.f;
            // for each intermediate point
            for (decltype (size) i = 0; i < size; ++i) {
                const Point curPoint = unstretch(_points[i]);
                Point* curCenter = &_scaledCenters[i];
                float* curRadius = &_reducedRadii[i];
                const bool curSide = _sides[i];

                if (::reduceRadii(*this, prevPoint, *prevCenter, *prevRadius, prevSide, curPoint, *curCenter, *curRadius, curSide)) {
                    // we reduced the radii of the circles, start over
                    done = false;
                    break;
                }

                // solve for the current point and previous point pair
                const SolveResult& res = ::solve(prevInitialX, *prevCenter, *prevRadius, prevSide, *curCenter, *curRadius, curSide);
                _scaledSegments.push_back(res.speedUp);
                _scaledSegments.push_back(res.cruise);
                prevInitialX = res.cruise.finalX;

                // the current point becomes the previous point for the next one
                prevPoint = curPoint;
                prevCenter = curCenter;
                prevRadius = curRadius;
                prevSide = curSide;
            }
            // for the last point
            if (done) {
                const Point curPoint = unstretch(_lastPoint);
                Point* curCenter = &_scaledLastCenter;
                float* curRadius = &_lastReducedRadius;
                const bool curSide = false;

                if (::reduceRadii(*this, prevPoint, *prevCenter, *prevRadius, prevSide, curPoint, *curCenter, *curRadius, curSide)) {
                    // we reduced the radii of the circles, start over
                    done = false;
                } else {
                    // solve for the last point and the previous point pair
                    const SolveResult& res = ::solve(prevInitialX, *prevCenter, *prevRadius, prevSide, *curCenter, *curRadius, curSide);
                    _scaledSegments.push_back(res.speedUp);
                    _scaledSegments.push_back(res.cruise);
                    prevInitialX = res.cruise.finalX;

                    // also add the final segment, corresponding to the circle on the last point
                    Segment speedDown = Circle {_scaledLastCenter, _lastReducedRadius * _lastReducedRadius, 1.f};
                    speedDown.initialX = prevInitialX;
                    speedDown.finalX = _lastPoint.x() / xStretch;
                    _scaledSegments.push_back(speedDown);
                }
            }
        }

        if (done && _autoFlip) {
            // check if we need to flip around some circles
            for (decltype (size) i = 0; i < size; ++i) {
                const auto j = i * 2 + 2;
                if ((_scaledSegments[j].type == SegmentType::Circle) && (_scaledSegments[j].initialX > _scaledSegments[j].finalX)) {
                    // flip the circle and start over with solving the curve
                    if (_autoSides[i] == Side::Auto) {
                        _autoSides[i] = _sides[i] ? Side::ForceFalse : Side::ForceTrue;
                        done = false;
                        break;
                    }
                }
            }
        }
        ++_iterations;
    }

    _solveTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();
}


// Notes:
// * ensure x and y input point coordinates are always increasing
// * clamp input point coordinates to min distance (!) !
// * manipulate y when there are overshoots (!)
// * if all fails, reduce radii based on y inversions
