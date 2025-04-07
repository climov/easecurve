
module main.appstate;

import std;
import alx.trig;
import sokol.app;
import sokol.gfx;
import sokol.gp;

namespace va = alx::va;
namespace trig = alx::trig;
using namespace trig::degree_literals;

namespace {

inline void setColor(const sg_color color)
{
    sgp_set_color(color.r, color.g, color.b, color.a);
}

sgp_point cast(const va::Vec2f point) { return {point.x(), point.y()}; }
va::Vec2f cast(const sgp_point point) { return {{point.x, point.y}}; }

va::Vec2f mapToScreen(const AppState& app, const va::Vec2f point)
{
    const va::Vec2f windowSize {{sapp_widthf(), sapp_heightf()}};
    const va::Vec2f border {{app._border.x() * 1.f, app._border.y() * 1.f}};
    const va::Vec2f botLeft {{border.x(), windowSize.y() - border.y()}};
    const va::Vec2f topRight = {{windowSize.x() - border.x(), border.y()}};
    const va::Vec2f size = topRight - botLeft;

    return botLeft + va::Vec2f{{point.x() / app._path.endTime * size.x(), point.y() / app._path.endProgress * size.y()}};
}

[[maybe_unused]]
void drawSolidLine(const va::Vec2f start, const va::Vec2f end, const sg_color color)
{
    setColor(color);
    sgp_draw_line(start.x(), start.y(), end.x(), end.y());
}

[[maybe_unused]]
void drawDot(const va::Vec2f point, const sg_color color)
{
    std::array<sgp_line, 4> line;
    line[0].a = cast(point - va::Vec2f{{1.f, 0.f}});
    line[0].b = cast(point + va::Vec2f{{1.f, 0.f}});
    line[1].a = cast(point - va::Vec2f{{0.f, 1.f}});
    line[1].b = cast(point + va::Vec2f{{0.f, 1.f}});
    setColor(color);
    sgp_draw_lines(line.data(), line.size());
}

[[maybe_unused]]
void drawCircle(const AppState& app, const va::Vec2f center, const float radius, const sg_color color, const int segments = 100)
{
    std::vector<sgp_line> vertices;
    const trig::RadF increment = trig::Full<trig::RadF> / segments;
    trig::RadF angle = {};
    for (int i = 0; i < segments; ++i) {
        const va::Vec2f start {{center.x() + radius * cos(angle), center.y() + radius * sin(angle)}};
        angle += increment;
        const va::Vec2f end {{center.x() + radius * cos(angle), center.y() + radius * sin(angle)}};
        const va::Vec2f a = mapToScreen(app, start);
        const va::Vec2f b = mapToScreen(app, end);
        vertices.push_back({cast(a), cast(b)});
    }
    setColor(color);
    sgp_draw_lines(vertices.data(), static_cast<unsigned>(vertices.size()));
}

[[maybe_unused]]
void drawDisc(const AppState& app, const va::Vec2f center, const float radius, const sg_color color, const int segments = 100)
{
    std::vector<sgp_triangle> vertices;
    const trig::RadF increment = trig::Full<trig::RadF> / segments;
    trig::RadF angle = {};
    for (int i = 0; i < segments; ++i) {
        const va::Vec2f start {{center.x() + radius * cos(angle), center.y() + radius * sin(angle)}};
        angle += increment;
        const va::Vec2f end {{center.x() + radius * cos(angle), center.y() + radius * sin(angle)}};
        const va::Vec2f a = mapToScreen(app, center);
        const va::Vec2f b = mapToScreen(app, start);
        const va::Vec2f c = mapToScreen(app, end);
        vertices.push_back({cast(a), cast(b), cast(c)});
    }
    setColor(color);
    sgp_draw_filled_triangles(vertices.data(), static_cast<unsigned>(vertices.size()));
    drawCircle(app, center, radius, color, segments);
}

void drawDottedLine(const va::Vec2f start, const va::Vec2f end, const sg_color color, const int segments = 101)
{
    std::vector<sgp_line> line;
    va::Vec2f diff = end - start;
    va::Vec2f increment {{diff.x() / segments, diff.y() / segments}};
    for (int i = 0; i < segments; i += 2) {
        line.push_back({cast(start + increment * (i + 0.f)), cast(start + increment * (i + 1.f))});
    }
    setColor(color);
    sgp_draw_lines(line.data(), static_cast<unsigned>(line.size()));
}

[[maybe_unused]]
void drawSolidArrow(const va::Vec2f origin, const va::Vec2f vec, const sg_color color)
{
    std::array<sgp_line, 3> arrow;
    arrow[0].a = cast(origin);
    arrow[0].b = cast(origin + vec);
    arrow[1].a = arrow[0].b;
    arrow[1].b = cast(cast(arrow[1].a) - (vec.unit() * 20.f).rotated(30_deg));
    arrow[2].a = arrow[0].b;
    arrow[2].b = cast(cast(arrow[2].a) - (vec.unit() * 20.f).rotated(-30_deg));
    setColor(color);
    sgp_draw_lines(arrow.data(), arrow.size());
};

[[maybe_unused]]
void drawDottedCircle(const AppState& app, const va::Vec2f center, const float radius, const sg_color color, const int segments = 100)
{
    std::vector<sgp_line> vertices;
    //const float increment = 2 * kPi<float> / segments;
    const trig::RadF increment = trig::Full<trig::RadF> / segments;
    //float angle = 0.f;
    trig::RadF angle = {};
    for (int i = 0; i < segments; i += 2) {
        const va::Vec2f start {{center.x() + radius * cos(angle), center.y() + radius * sin(angle)}};
        angle += increment;
        const va::Vec2f end {{center.x() + radius * cos(angle), center.y() + radius * sin(angle)}};
        angle += increment;
        const va::Vec2f a = mapToScreen(app, start);
        const va::Vec2f b = mapToScreen(app, end);
        vertices.push_back({cast(a), cast(b)});
    }
    setColor(color);
    sgp_draw_lines(vertices.data(), static_cast<unsigned>(vertices.size()));
}

[[maybe_unused]]
void drawDottedEllipse(const AppState& app, const va::Vec2f center, const float radius, const float xStretch, const sg_color color, const int segments = 100)
{
    std::vector<sgp_line> vertices;
    const trig::RadF increment = trig::Full<trig::RadF> / segments;
    trig::RadF angle = {};
    for (int i = 0; i < segments; i += 2) {
        const va::Vec2f start {{center.x() + radius * cos(angle) * xStretch, center.y() + radius * sin(angle)}};
        angle += increment;
        const va::Vec2f end = {{center.x() + radius * cos(angle) * xStretch, center.y() + radius * sin(angle)}};
        angle += increment;
        const va::Vec2f a = mapToScreen(app, start);
        const va::Vec2f b = mapToScreen(app, end);
        vertices.push_back({cast(a), cast(b)});
    }
    setColor(color);
    sgp_draw_lines(vertices.data(), static_cast<unsigned>(vertices.size()));
}

void drawCheckpoint(const AppState& app, const float time, const float progress, const float endProgress, const float easeDuration, const float adjustedEaseDuration, const bool easeDurationBefore, const bool easeDurationAfter)
{
    // horiz
    drawDottedLine(mapToScreen(app, {{0, progress}}), mapToScreen(app, {{time, progress}}), app._guideColor);
    // vert
    drawDottedLine(mapToScreen(app, {{time, 0}}), mapToScreen(app, {{time, progress}}), app._guideColor);
    if (easeDurationBefore) {
        drawDottedLine(mapToScreen(app, {{time - adjustedEaseDuration, 0}}), mapToScreen(app, {{time - adjustedEaseDuration, endProgress}}), app._guideColor);
        if (easeDuration > adjustedEaseDuration) {
            drawDottedLine(mapToScreen(app, {{time - easeDuration, 0}}), mapToScreen(app, {{time - easeDuration, endProgress}}), app._errorColor);
        }
    }
    if (easeDurationAfter) {
        drawDottedLine(mapToScreen(app, {{time + adjustedEaseDuration, 0}}), mapToScreen(app, {{time + adjustedEaseDuration, endProgress}}), app._guideColor);
        if (easeDuration > adjustedEaseDuration) {
            drawDottedLine(mapToScreen(app, {{time + easeDuration, 0}}), mapToScreen(app, {{time + easeDuration, endProgress}}), app._errorColor);
        }
    }
};

void drawCoordinates(const AppState& app)
{
    const va::Vec2f windowSize {{sapp_widthf(), sapp_heightf()}};
    const va::Vec2f border {{app._border.x() * 1.f, app._border.y() * 1.f}};
    const va::Vec2f botLeft {{border.x(), windowSize.y() - border.y()}};
    //const va::Vec2f topRight = {windowSize.x() - border.x(), border.y()};

    // horiz axis
    drawSolidArrow({{0.f, botLeft.y()}}, {{windowSize.x() * 1.f, 0.f}}, app._axisColor);
    // vert axis
    drawSolidArrow({{botLeft.x(), windowSize.y() * 1.f}}, {{0.f, windowSize.y() * -1.f}}, app._axisColor);

    if (app._showGuides) {
        // start point
        drawCheckpoint(app, app._path.startTime, app._path.startProgress, app._path.endProgress, app._path.startEaseDuration, app._path.adjustedStartEaseDuration, false, true);
        // end point
        drawCheckpoint(app, app._path.endTime, app._path.endProgress, app._path.endProgress, app._path.endEaseDuration, app._path.adjustedEndEaseDuration, true, false);
        // checkpoints
        for (const Checkpoint& checkpoint: app._path.checkpoints) {
            drawCheckpoint(app, checkpoint.time, checkpoint.progress, app._path.endProgress, checkpoint.easeDuration / 2.f, checkpoint.adjustedEaseDuration / 2.f, true, true);
        }
    }
}

void drawCurve(const AppState& app)
{
    constexpr int linesPerSegment = 10000;
    const float xIncrement = app._path.endTime / linesPerSegment;
    const float xStep = xIncrement / 100;
    float prevX = app._path.startTime;
    float prevY = 0;
    for (int i = 1; i <= linesPerSegment; ++i) {
        const float x = i * xIncrement;
        const float y = prevY + progressBetween(app._path, prevX, x, xStep);
        drawSolidLine(mapToScreen(app, {{prevX, prevY}}), mapToScreen(app, {{x, y}}), app._curveColor);
        prevX = x;
        prevY = y;
    }
}

void drawSpeed(const AppState& app)
{
    ///velocityAt(app._path, 10);
    constexpr int linesPerSegment = 10000;
    if (!app._showSpeed) {
        return;
    }
    const float xIncrement = app._path.endTime / linesPerSegment;
    float prevX = app._path.startTime;
    float prevY = velocityAt(app._path, prevX);
    for (int i = 1; i <= linesPerSegment; ++i) {
        const float x = i * xIncrement;
        const float y = velocityAt(app._path, x);
        drawSolidLine(mapToScreen(app, {{prevX, prevY}}), mapToScreen(app, {{x, y}}), app._speedColor);
        prevX = x;
        prevY = y;
    }
}

void drawAccel(const AppState& app)
{
    constexpr int linesPerSegment = 10000;
    if (!app._showAccel) {
        return;
    }
    const float xIncrement = app._path.endTime / linesPerSegment;
    float prevX = app._path.startTime;
    float prevY = accelAt(app._path, prevX);
    for (int i = 1; i <= linesPerSegment; ++i) {
        const float x = i * xIncrement;
        const float y = accelAt(app._path, x);
        drawSolidLine(mapToScreen(app, {{prevX, prevY}}), mapToScreen(app, {{x, y}}), app._accelColor);
        prevX = x;
        prevY = y;
    }
}

// void drawCircles(const AppState& app)
// {
//     if (!app._showCircles) {
//         return;
//     }
//
//     const float xStretch = app._curve._xStretch;
//     // first circle
//     {
//         const va::Vec2f center = mapToScreen(app, {{app._curve._scaledFirstCenter.x() * xStretch, app._curve._scaledFirstCenter.y()}});
//
//         // radius
//         drawDottedLine(center, mapToScreen(app, EaseCurve::kFirstPoint), app._curveColor, 21);
//         //drawSolidLine(center, mapToScreen(app, EaseCurve::kStartPoint), app._curveColor);
//
//         // center
//         drawDisc(app, {{app._curve._scaledFirstCenter.x() * xStretch, app._curve._scaledFirstCenter.y()}}, .05f, app._curveColor);
//         //drawDot(center, app._curveColor);
//
//         // circle
//         drawDottedEllipse(app, {{app._curve._scaledFirstCenter.x() * xStretch, app._curve._scaledFirstCenter.y()}}, app._curve._firstRadius, xStretch, app._curveColor);
//     }
//
//     // last circle
//     {
//         const va::Vec2f center = mapToScreen(app, {{app._curve._scaledLastCenter.x() * xStretch, app._curve._scaledLastCenter.y()}});
//
//         // radius
//         drawDottedLine(center, mapToScreen(app, app._curve._lastPoint), app._curveColor, 21);
//         //drawSolidLine(app._window, center, mapToScreen(app, app._curve._lastPoint), app._curveColor);
//
//         // center
//         drawDisc(app, {{app._curve._scaledLastCenter.x() * xStretch, app._curve._scaledLastCenter.y()}}, .05f, app._curveColor);
//         //drawDot(center, app._curveColor);
//
//         // circle
//         drawDottedEllipse(app, {{app._curve._scaledLastCenter.x() * xStretch, app._curve._scaledLastCenter.y()}}, app._curve._lastRadius, xStretch, app._curveColor);
//     }
//
//     // intermediate circles
//     const auto count = app._curve._points.size();
//     for (size_t i = 0; i < count; ++i) {
//         const va::Vec2f center = mapToScreen(app, {{app._curve._scaledCenters[i].x() * xStretch, app._curve._scaledCenters[i].y()}});
//
//         // radius
//         drawDottedLine(center, mapToScreen(app, app._curve._points[i]), app._curveColor, 21);
//         //drawDisc(app._window, center, 2.f, app._curveColor);
//
//         // center
//         drawDisc(app, {{app._curve._scaledCenters[i].x() * xStretch, app._curve._scaledCenters[i].y()}}, .05f, app._curve._sides[i] == app._curve._originalSides[i] ? app._curveColor : app._errorColor);
//
//         // circle
//         drawDottedEllipse(app, {{app._curve._scaledCenters[i].x() * xStretch, app._curve._scaledCenters[i].y()}}, app._curve._radii[i], xStretch, app._curveColor);
//     }
// }

void drawPoly(const AppState& app)
{
    if (!app._showPolyLine) {
        return;
    }
    va::Vec2f start {{app._path.startTime, app._path.startProgress}};
    //auto i = app._path.checkpoints.size(); // app._curve._points.size();
    //i = 0;
    //for (const EaseCurve::Point& point: app._curve._points) {
    for (const Checkpoint& checkpoint: app._path.checkpoints) {
        const va::Vec2f point {{checkpoint.time, checkpoint.progress}};
        //const bool valid = app._curve._pointsXValid[i] && app._curve._pointsYValid[i];
        //const sg_color color = valid ? app._curveColor : app._errorColor;
        const sg_color color = app._curveColor;
        drawDottedLine(mapToScreen(app, start), mapToScreen(app, point), color);
        start = point;
        //++i;
    }
    {
        drawDottedLine(mapToScreen(app, start), mapToScreen(app, {{app._path.endTime, app._path.endProgress}} /* app._curve._lastPoint*/), app._curveColor);
    }
}

void drawMouseCursor(const AppState& app)
{
    const va::Vec2f pos = app._mouse;
    std::array<sgp_line, 4> whiteLines;
    std::array<sgp_line, 2> blackLines;
    // 2 horizontal white lines
    whiteLines[0].a = cast(pos + va::Vec2f{{-3.f, -1.f}});
    whiteLines[0].b = cast(pos + va::Vec2f{{+3.f, -1.f}});
    whiteLines[1].a = cast(pos + va::Vec2f{{-3.f, +1.f}});
    whiteLines[1].b = cast(pos + va::Vec2f{{+3.f, +1.f}});
    // 2 vertical white lines
    whiteLines[2].a = cast(pos + va::Vec2f{{-1.f, -3.f}});
    whiteLines[2].b = cast(pos + va::Vec2f{{-1.f, +3.f}});
    whiteLines[3].a = cast(pos + va::Vec2f{{+1.f, -3.f}});
    whiteLines[3].b = cast(pos + va::Vec2f{{+1.f, +3.f}});
    // 1 horizontal black line
    blackLines[0].a = cast(pos + va::Vec2f{{-3.f, +0.f}});
    blackLines[0].b = cast(pos + va::Vec2f{{+3.f, +0.f}});
    // 1 vertical black line
    blackLines[1].a = cast(pos + va::Vec2f{{+0.f, -3.f}});
    blackLines[1].b = cast(pos + va::Vec2f{{+0.f, +3.f}});
    setColor(sg_color {1.f, 1.f, 1.f, 1.f}); // white
    sgp_draw_lines(whiteLines.data(), whiteLines.size());
    setColor(sg_color {0.f, 0.f, 0.f, 1.f}); // black
    sgp_draw_lines(blackLines.data(), blackLines.size());
    //sapp_show_mouse(false);
}

} // namespace

void render(const AppState& app)
{
    // Clear the frame buffer.
    setColor(app._windowBg);
    sgp_clear();

    drawCoordinates(app);
    drawPoly(app);
    //drawCircles(app);
    drawCurve(app);
    drawSpeed(app);
    drawAccel(app);

    drawMouseCursor(app);
}
