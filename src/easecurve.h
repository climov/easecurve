#ifndef EASE_CURVE
#define EASE_CURVE

#include <vector>

template <typename Rep>
[[maybe_unused]]
constexpr Rep kPi = static_cast<Rep>(3.141592653589793238462643383);

class EaseCurve
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
    void setLastPoint(float x, float y) { _lastPoint = {x, y}; solve(); }

    //! Set the "global" radius for the curve circles. This gets copied into every circle.
    void setRadius(float radius) { _radius = radius; setRadius(); }

    //! Set the horizontal stretch factor for the ellipses. A factor of 2 means the horizonal radius is twice as large.
    void setXStretch(float stretch) { _xStretch = stretch; solve(); }

    //! Set the radius of the first point
    void setFirstRadius(float radius) { _firstRadius = radius; solve(); }

    //! Set the radius of the last point
    void setLastRadius(float radius) { _lastRadius = radius; solve(); }

    //! Add an intermediate point at the end (right before the last point) and sets its circle radius.
    void addPoint(float x, float y, float radius);
    //! Add an intermediate point at the end (right before the last point) using the global circle radius.
    void addPoint(float x, float y) { addPoint(x, y, _radius); }

    //! Inserts an intermediate point at a certain index (0 is the first one after the inital (0, 0) point)
    void insertPointAt(int index, float x, float y, float radius);
    //! Inserts an intermediate point at a certain index (0 is the first one after the inital (0, 0) point)
    void insertPointAt(int index, float x, float y) { insertPointAt(index, x, y, _radius); }

    //! Remove the intermediate point at the given index
    void removePointAt(int index);

public:
    //! Evaluate for the given x and return associated y value
    float evaluate(float x) const;
    //! Get the speed of the curve for the given x
    float speed(float x) const;
    //! Get the acceleration of the curve for the given x
    float accel(float x) const;

public: // internal (private)
    void setRadius();
    void solve();

public: // internal (private)
    struct Point
    {
        float x = 0.f;
        float y = 0.f;
    };

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

        float evaluate(float x) const;
        float speed(float x) const;
        float accel(float x) const;

        float initialY() const { return evaluate(initialX); }
        float finalY() const { return evaluate(finalX); }

        float initialX;
        float finalX;
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

#endif // EASE_CURVE
