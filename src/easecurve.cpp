
#include <easecurve.h>

#include <cmath>

namespace {

EaseCurve::Point operator- (const EaseCurve::Point lhs, const EaseCurve::Point rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

EaseCurve::Point operator* (const EaseCurve::Point lhs, const float rhs)
{
    return {lhs.x * rhs, lhs.y * rhs};
}

//! Returns the angle of this vector
//! The vector (1,0) corresponds 0 rad, (0,1) corresponds PI/2 rad.
float angle(const EaseCurve::Point p)
{
    return std::atan2(p.y, p.x);
}

//! Returns an unit vector with the given angle
EaseCurve::Point unitFromAngle(float ang)
{
    return {std::cos(ang), std::sin(ang)};
}

void validate(EaseCurve& curve)
{
    curve._valid = true;

    //const float maxRadius = std::min(curve._lastPoint.x, curve._lastPoint.y);
    const float maxRadius = curve._lastPoint.x;
    // check that the radius is in [0, maxRadius]
    if (!(curve._radiusValid = !(curve._radius < 0 || curve._radius > maxRadius))) {
        curve._valid = false;
    }
    // check that each radius is in [0, maxRadius]
    if (!(curve._firstRadiusValid = !(curve._firstRadius < 0 || curve._firstRadius > maxRadius))) {
        curve._valid = false;
    }
    if (!(curve._lastRadiusValid = !(curve._lastRadius < 0 || curve._lastRadius > maxRadius))) {
        curve._valid = false;
    }
    auto count = curve._points.size();
    for (decltype (count) i = 0; i < count; ++i) {
        const float radius = curve._radii[i];
        if (!(curve._radiiValid[i] = !(radius < 0 || radius > maxRadius))) {
            curve._valid = false;
        }
    }
    EaseCurve::Point prevPoint = EaseCurve::kFirstPoint;
    // check that points are in ascending order
    for (decltype (count) i = 0; i < count; ++i) {
        const EaseCurve::Point point = curve._points[i];
        // check each x is in [0, lastX] and (prevPoint.x, lastX]
        if (!(curve._pointsXValid[i] = !(point.x < 0.f || point.x > curve._lastPoint.x) && point.x > prevPoint.x)) {
            curve._valid = false;
        }
        // check each y is in [0, lastY] and [prevPoint.y, lastY]
        if (!(curve._pointsYValid[i] = !(point.y < 0.f || point.y > curve._lastPoint.y) && point.y > prevPoint.y)) {
            curve._valid = false;
        }
        prevPoint = point;
    }
    if (count) {
        // check last x and y are not less than end point
        if (!(curve._pointsXValid.back() = curve._pointsXValid.back() && prevPoint.x < curve._lastPoint.x)) {
            curve._valid = false;
        }
        if (!(curve._pointsYValid.back() = curve._pointsYValid.back() && prevPoint.y < curve._lastPoint.y)) {
            curve._valid = false;
        }
    }
}

inline float solve(EaseCurve& curve, EaseCurve::Point leftCenter, float leftRadius, bool leftSide, float initialX, EaseCurve::Point rightCenter, float rightRadius, bool rightSide)
{
    const EaseCurve::Point delta = rightCenter - leftCenter;
    const float deltaAngle = angle(delta);
    const float centerDistance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    const float speedUpSign = leftSide ? -1.f : 1.f;
    const float totalRadius = (leftSide == rightSide) ? std::abs(leftRadius - rightRadius) : leftRadius + rightRadius;
    const float tangentToDeltaAngle = std::asin(totalRadius / centerDistance);

    float leftAngle = 0.f;
    float rightAngle = 0.f;
    if (leftSide == rightSide) {
        // both circles are on the same side
        if (leftSide) {
            // both above
            if (leftRadius <= rightRadius) {
                // smaller circle on the left
                leftAngle = deltaAngle - tangentToDeltaAngle - kPi<float> / 2.f;
            } else {
                // smaller circle on the right
                leftAngle = deltaAngle + tangentToDeltaAngle - kPi<float> / 2.f;
            }
        } else {
            // both below
            if (leftRadius <= rightRadius) {
                // smaller circle on the left
                leftAngle = deltaAngle + tangentToDeltaAngle + kPi<float> / 2.f;
            } else {
                // smaller circle on the right
                leftAngle = deltaAngle - tangentToDeltaAngle + kPi<float> / 2.f;
            }
        }
        rightAngle = leftAngle;
    } else {
        if (leftSide) {
            // left circle is above
            leftAngle = deltaAngle + tangentToDeltaAngle - kPi<float> / 2.f;
            rightAngle = leftAngle + kPi<float>;
        } else {
            // left circle is below
            leftAngle = deltaAngle - tangentToDeltaAngle + kPi<float> / 2.f;
            rightAngle = leftAngle - kPi<float>;
        }
    }

    const float leftTangentX = leftCenter.x + leftRadius * std::cos(leftAngle);
    const float leftTangentY = leftCenter.y + leftRadius * std::sin(leftAngle);

    const float rightTangentX = rightCenter.x + rightRadius * std::cos(rightAngle);
    const float rightTangentY = rightCenter.y + rightRadius * std::sin(rightAngle);

    const float slope = (rightTangentY - leftTangentY) / (rightTangentX - leftTangentX);
    const float deltaY = leftTangentY - slope * leftTangentX;

    {
        EaseCurve::Segment speedUp = EaseCurve::Circle {leftCenter, leftRadius * leftRadius, speedUpSign};
        speedUp.initialX = initialX;
        speedUp.finalX = leftTangentX;
        curve._scaledSegments.push_back(speedUp);
    }
    {
        EaseCurve::Segment cruise = EaseCurve::Line {slope, deltaY};
        cruise.initialX = leftTangentX;
        cruise.finalX = rightTangentX;
        curve._scaledSegments.push_back(cruise);
    }
    return rightTangentX;
}

} // namespace

void EaseCurve::addPoint(float x, float y, float radius)
{
    _points.push_back({x, y});
    _radii.push_back(radius);
    _pointsXValid.push_back(true);
    _pointsYValid.push_back(true);
    _radiiValid.push_back(true);
    solve();
}

void EaseCurve::insertPointAt(int index, float x, float y, float radius)
{
    _points.insert(_points.begin() + index, {x, y});
    _radii.insert(_radii.begin() + index, radius);
    _pointsXValid.push_back(true);
    _pointsYValid.push_back(true);
    _radiiValid.push_back(true);
    solve();
}

void EaseCurve::removePointAt(int index)
{
    _points.erase(_points.begin() + index);
    _radii.erase(_radii.begin() + index);
    _pointsXValid.pop_back();
    _pointsYValid.pop_back();
    _radiiValid.pop_back();
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
        const float deltaX = x - circle.center.x;
        return circle.center.y + circle.sign * std::sqrt(circle.sqRadius - deltaX * deltaX);
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
        const float deltaX = x - circle.center.x;
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
        const float deltaX = x - circle.center.x;
        const float len = circle.sqRadius - deltaX * deltaX;
        return - circle.sign * circle.sqRadius / std::sqrt(len * len *len);
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
    validate(*this);

    const float xStretch = _xStretch;
    auto unstretch = [xStretch](EaseCurve::Point p) { return EaseCurve::Point{p.x / xStretch, p.y}; };

    auto size = _points.size();

    // figure out the centers and sides of the circles
    {
        _scaledCenters.clear();
        _sides.clear();

        _scaledFirstCenter = {EaseCurve::kFirstPoint.x, EaseCurve::kFirstPoint.y + _firstRadius};
        _scaledLastCenter = {_lastPoint.x / xStretch, _lastPoint.y - _lastRadius};

        for (decltype(size) i = 0; i < size; ++i) {
            const EaseCurve::Point prevPoint = unstretch((i == 0) ? EaseCurve::kFirstPoint : _points[i - 1]);
            const EaseCurve::Point nextPoint = unstretch((i == size - 1) ? _lastPoint : _points[i + 1]);
            const EaseCurve::Point currPoint = unstretch(_points[i]);
            // Figure out whether the current circle needs to go "above" or "below" the line segments.
            // In order to figure that out, we need to see whether the angle (ie. slope) of the consecutive segments
            // increases or decreases.
            // When the angle increases, the curve "goes up", so the circle needs to be "above"
            const float leftAngle = angle(currPoint - prevPoint);
            const float rightAngle = angle(nextPoint - currPoint);
            if (rightAngle > leftAngle) {
                // circle is above
                _sides.push_back(true);
                const float centerAngle = (rightAngle + leftAngle - kPi<float>) / 2;
                _scaledCenters.push_back(currPoint - unitFromAngle(centerAngle) * _radii[i]);
            } else {
                // circle is below
                _sides.push_back(false);
                const float centerAngle = (leftAngle + rightAngle + kPi<float>) / 2;
                _scaledCenters.push_back(currPoint - unitFromAngle(centerAngle) * _radii[i]);
            }
        }
    }

    // split curve into segments
    {
        _scaledSegments.clear();

        EaseCurve::Point prevCenter = _scaledFirstCenter;
        float prevRadius = _firstRadius;
        bool prevSide = true;
        float prevInitialX = 0.f;
        for (decltype (size) i = 0; i < size; ++i) {
            const EaseCurve::Point curCenter = _scaledCenters[i];
            const float curRadius = _radii[i];
            const bool curSide = _sides[i];

            prevInitialX = ::solve(*this, prevCenter, prevRadius, prevSide, prevInitialX, curCenter, curRadius, curSide);

            prevCenter = curCenter;
            prevRadius = curRadius;
            prevSide = curSide;
        }
        {
            const EaseCurve::Point curCenter = _scaledLastCenter;
            const float curRadius = _lastRadius;
            const bool curSide = false;

            prevInitialX = ::solve(*this, prevCenter, prevRadius, prevSide, prevInitialX, curCenter, curRadius, curSide);

            {
                EaseCurve::Segment speedDown = EaseCurve::Circle {_scaledLastCenter, _lastRadius * _lastRadius, 1.f};
                speedDown.initialX = prevInitialX;
                speedDown.finalX = _lastPoint.x / xStretch;
                _scaledSegments.push_back(speedDown);
            }
        }
    }
}
