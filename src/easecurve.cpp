
#include <easecurve.h>

#include <cmath>

#include <chrono>

namespace {

EaseCurve::Point operator+ (const EaseCurve::Point lhs, const EaseCurve::Point rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

EaseCurve::Point operator- (const EaseCurve::Point lhs, const EaseCurve::Point rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

EaseCurve::Point operator* (const EaseCurve::Point lhs, const float rhs)
{
    return {lhs.x * rhs, lhs.y * rhs};
}

//! Return the length of this vector
float length(const EaseCurve::Point p)
{
    return std::sqrt(p.x * p.x + p.y * p.y);
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

//! calculate the unit vector (same direction, same orientation, length of 1)
//! for a zero vector it will return a zero vector
//EaseCurve::Point unitOrZero(const EaseCurve::Point v)
//{
//    if (const float l = length(v); l > 0) {
//        return {v.x / l, v.y / l};
//    }
//    return {};
//}

void validate(EaseCurve& curve)
{
    curve._valid = true;

    auto count = curve._points.size();
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

struct SolveResult
{
    EaseCurve::Segment speedUp;
    EaseCurve::Segment cruise;
};

// solve for a pair of points/circles
inline SolveResult solve(float initialX, EaseCurve::Point leftCenter, float leftRadius, bool leftSide, EaseCurve::Point rightCenter, float rightRadius, bool rightSide)
{
    const EaseCurve::Point delta = rightCenter - leftCenter;
    const float deltaAngle = angle(delta);
    const float centerDistance = length(delta);
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

    // add the circular segment corresponding to the left point/circle (accelerating)
    EaseCurve::Segment speedUp = EaseCurve::Circle {leftCenter, leftRadius * leftRadius, speedUpSign};
    speedUp.initialX = initialX;
    speedUp.finalX = leftTangentX;

    // add the line segment corresponding to the straight line connecting the two circles (constant speed)
    EaseCurve::Segment cruise = EaseCurve::Line {slope, deltaY};
    cruise.initialX = leftTangentX;
    cruise.finalX = rightTangentX;

    return {speedUp, cruise};
}

bool reduceRadii(EaseCurve& curve, const EaseCurve::Point& leftPoint, EaseCurve::Point& leftCenter, float& leftRadius, bool leftSide, const EaseCurve::Point& rightPoint, EaseCurve::Point& rightCenter, float& rightRadius, bool rightSide)
{
    // check and reduce the radii if needed
    if (curve._reduceRadii && (leftRadius > 0.f || rightRadius > 0.f)) {
        if (leftSide == rightSide) {
            // both circles are on the same side
            const EaseCurve::Point pointDiff = rightPoint - leftPoint;
            const float minDistance = std::min(curve._minDistance, length(pointDiff));
            const bool reduceLeftSide = (leftRadius > 0.f) && length(leftCenter - rightPoint) - leftRadius - minDistance < 0;
            if (reduceLeftSide) {
                // right point is "inside" the left circle
                const EaseCurve::Point pointDelta = leftPoint - rightPoint;
                const EaseCurve::Point leftDelta = leftCenter - leftPoint;
                float minRatio = 0.f;
                float maxRatio = 1.f;
                while (true) {
                    const float minDelta = length(pointDelta + leftDelta * minRatio ) - leftRadius * minRatio - minDistance;
                    const float maxDelta = length(pointDelta + leftDelta * maxRatio ) - leftRadius * maxRatio - minDistance;
                    const float midRatio = (minRatio + maxRatio) / 2;
                    if ((std::abs(minDelta + maxDelta) < curve._precision) || (midRatio <= minRatio) || (midRatio >= maxRatio)) {
                        leftRadius *= minRatio;
                        leftCenter = leftPoint + leftDelta * minRatio;
                        break;
                    }
                    const float midDelta = length(pointDelta + leftDelta * midRatio ) - leftRadius * midRatio - minDistance;
                    if (midDelta > 0.f) {
                        minRatio = midRatio;
                    } else {
                        maxRatio = midRatio;
                    }
                    ++curve._iterations;
                }
            }
            const bool reduceRightSide = (rightRadius > 0.f) && length(rightCenter - leftPoint) - rightRadius - minDistance < 0;
            if (reduceRightSide) {
                // first point is "inside" the second circle
                const EaseCurve::Point pointDelta = rightPoint - leftPoint;
                const EaseCurve::Point rightDelta = rightCenter - rightPoint;
                float minRatio = 0.f;
                float maxRatio = 1.f;
                while (true) {
                    const float minDelta = length(pointDelta + rightDelta * minRatio) - rightRadius * minRatio - minDistance;
                    const float maxDelta = length(pointDelta + rightDelta * maxRatio) - rightRadius * maxRatio - minDistance;
                    const float midRatio = (minRatio + maxRatio) / 2;
                    if ((std::abs(minDelta + maxDelta) < curve._precision) || (midRatio <= minRatio) || (midRatio >= maxRatio)) {
                        rightRadius *= minRatio;
                        rightCenter = rightPoint + rightDelta * minRatio;
                        break;
                    }
                    const float midDelta = length(pointDelta + rightDelta * midRatio) - rightRadius * midRatio - minDistance;
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
            const float minDistance = std::min(curve._minDistance, length(pointDiff));
            if (length(centerDelta) - totalRadius - minDistance < 0) {
                // circles are "overlapping"
                const EaseCurve::Point centerDiff = centerDelta - pointDiff;
                float minRatio = 0.f;
                float maxRatio = 1.f;
                while (true) {
                    const float minDelta = length(pointDiff + centerDiff * minRatio) - totalRadius * minRatio - minDistance;
                    const float maxDelta = length(pointDiff + centerDiff * maxRatio) - totalRadius * maxRatio - minDistance;
                    const float midRatio = (minRatio + maxRatio) / 2;
                    if ((std::abs(minDelta + maxDelta) < curve._precision) || (midRatio <= minRatio) || (midRatio >= maxRatio)) {
                        leftRadius *= minRatio;
                        leftCenter = leftPoint + leftDelta * minRatio;
                        rightRadius *= minRatio;
                        rightCenter = rightPoint + rightDelta * minRatio;
                        break;
                    }
                    const float midDelta = length(pointDiff + centerDiff * midRatio) - totalRadius * midRatio - minDistance;
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
            const float minDistance = std::min(curve._minDistance, length(pointDiff));
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
    auto unstretch = [xStretch](Point p) { return Point{p.x / xStretch, p.y}; };

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

            _scaledFirstCenter = {kFirstPoint.x, kFirstPoint.y + _firstReducedRadius};
            _scaledLastCenter = {_lastPoint.x / xStretch, _lastPoint.y - _lastReducedRadius};

            for (decltype(size) i = 0; i < size; ++i) {
                const Point prevPoint = unstretch((i == 0) ? kFirstPoint : _points[i - 1]);
                const Point nextPoint = unstretch((i == size - 1) ? _lastPoint : _points[i + 1]);
                const Point currPoint = unstretch(_points[i]);
                // Figure out whether the current circle needs to go "above" or "below" the line segments.
                // In order to figure that out, we need to see whether the angle (ie. slope) of the consecutive segments
                // increases or decreases.
                // When the angle increases, the curve "goes up", so the circle needs to be "above"
                const float leftAngle = angle(currPoint - prevPoint);
                const float rightAngle = angle(nextPoint - currPoint);
                const bool above = (_autoSides[i] == Side::ForceTrue) || ((_autoSides[i] == Side::Auto) && (rightAngle > leftAngle));
                const float signedPi = above ? kPi<float> : -kPi<float>;
                const float centerAngle = (leftAngle + rightAngle + signedPi) / 2;
                _sides[i] = above;
                _scaledCenters[i] = currPoint + unitFromAngle(centerAngle) * _reducedRadii[i];
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
                    speedDown.finalX = _lastPoint.x / xStretch;
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
