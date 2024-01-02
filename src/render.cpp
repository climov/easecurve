
#include "appstate.h"

#include "valgebra.h"
#include "rassert.h"
#include "trace.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Mouse.hpp>

#include <array>

using namespace trig::literals;

namespace {

sf::Vector2f mapToScreen(const AppState& app, const EaseCurve::Point point)
{
    const sf::Vector2u windowSize = app._window.getSize();
    const sf::Vector2f border {app._border.x * 1.f, app._border.y * 1.f};
    const sf::Vector2f botLeft {border.x, windowSize.y - border.y};
    const sf::Vector2f topRight = {windowSize.x - border.x, border.y};
    const sf::Vector2f size = topRight - botLeft;

    return botLeft + sf::Vector2f{point.x / app._curve._lastPoint.x * size.x, point.y / app._curve._lastPoint.y * size.y};
}

[[maybe_unused]]
void drawSolidLine(sf::RenderWindow& window, const sf::Vector2f& start, const sf::Vector2f& end, sf::Color color)
{
    std::array<sf::Vertex, 2> line;
    line[0].position = start;
    line[0].color = color;
    line[1].position = end;
    line[1].color = color;
    window.draw(line.data(), line.size(), sf::Lines);
}

[[maybe_unused]]
void drawDot(sf::RenderWindow& window, const sf::Vector2f& point, sf::Color color)
{
    std::array<sf::Vertex, 4> line;
    line[0].position = point - sf::Vector2f{1.f, 0.f};
    line[0].color = color;
    line[1].position = point + sf::Vector2f{1.f, 0.f};
    line[1].color = color;
    line[2].position = point - sf::Vector2f{0.f, 1.f};
    line[2].color = color;
    line[3].position = point + sf::Vector2f{0.f, 1.f};
    line[3].color = color;
    window.draw(line.data(), line.size(), sf::Lines);
}

//[[maybe_unused]]
//void drawCircle(sf::RenderWindow& window, const sf::Vector2f& center, float radius, sf::Color color)
//{
//    sf::CircleShape shape {radius};
//    shape.setPosition(center - sf::Vector2f{radius, radius});
//    shape.setOutlineColor(color);
//    shape.setOutlineThickness(1.f);
//    shape.setFillColor(sf::Color::Transparent);
//    window.draw(shape);
//}

[[maybe_unused]]
void drawDisc(sf::RenderWindow& window, const sf::Vector2f& center, float radius, sf::Color color)
{
    sf::CircleShape shape {radius};
    shape.setPosition(center - sf::Vector2f{radius, radius});
    shape.setFillColor(color);
    window.draw(shape);
}

void drawDottedLine(sf::RenderWindow& window, const sf::Vector2f& start, const sf::Vector2f& end, sf::Color color, int segments = 101)
{
    std::vector<sf::Vertex> line;
    sf::Vector2f diff = end - start;
    sf::Vector2f increment {diff.x / segments, diff.y / segments};
    for (int i = 0; i < segments; i += 2) {
        line.push_back({{start + increment * (i + 0.f)}, color});
        line.push_back({{start + increment * (i + 1.f)}, color});
    }
    window.draw(line.data(), line.size(), sf::Lines);
}

void drawSolidArrow(sf::RenderWindow& window, const sf::Vector2f& origin, const sf::Vector2f& vec, sf::Color color)
{
    std::array<sf::Vertex, 6> arrow;
    arrow[0].position = origin;
    arrow[0].color = color;
    arrow[1].position = origin + vec;
    arrow[1].color = color;
    arrow[2].position = arrow[1].position;
    arrow[2].color = color;
    arrow[3].position = arrow[2].position - va::rotated(va::unit(vec) * 20.f, 30_degf);
    arrow[3].color = color;
    arrow[4].position = arrow[1].position;
    arrow[4].color = color;
    arrow[5].position = arrow[4].position - va::rotated(va::unit(vec) * 20.f, -30_degf);
    arrow[5].color = color;
    window.draw(arrow.data(), arrow.size(), sf::Lines);
};

//void drawDottedCircle(AppState& app, const EaseCurve::Point& center, float radius, sf::Color color, int segments = 100)
//{
//    std::vector<sf::Vertex> vertices;
//    const float increment = 2 * kPi<float> / segments;
//    float angle = 0.f;
//    for (int i = 0; i < segments; i += 2) {
//        EaseCurve::Point start = {center.x + radius * std::cos(angle), center.y + radius * std::sin(angle)};
//        angle += increment;
//        EaseCurve::Point end = {center.x + radius * std::cos(angle), center.y + radius * std::sin(angle)};
//        angle += increment;
//        vertices.push_back({mapToScreen(app, start), color});
//        vertices.push_back({mapToScreen(app, end), color});
//    }
//    app._window.draw(vertices.data(), vertices.size(), sf::Lines);
//}

void drawDottedEllipse(AppState& app, const EaseCurve::Point& center, float radius, float xStretch, sf::Color color, int segments = 100)
{
    std::vector<sf::Vertex> vertices;
    const float increment = 2 * kPi<float> / segments;
    float angle = 0.f;
    for (int i = 0; i < segments; i += 2) {
        EaseCurve::Point start = {center.x + radius * std::cos(angle) * xStretch, center.y + radius * std::sin(angle)};
        angle += increment;
        EaseCurve::Point end = {center.x + radius * std::cos(angle) * xStretch, center.y + radius * std::sin(angle)};
        angle += increment;
        vertices.push_back({mapToScreen(app, start), color});
        vertices.push_back({mapToScreen(app, end), color});
    }
    app._window.draw(vertices.data(), vertices.size(), sf::Lines);
}

void drawCoordinates(AppState& app)
{
    const sf::Vector2u windowSize = app._window.getSize();
    const sf::Vector2f border {app._border.x * 1.f, app._border.y * 1.f};
    const sf::Vector2f botLeft {border.x, windowSize.y - border.y};
    const sf::Vector2f topRight = {windowSize.x - border.x, border.y};

    // horiz axis
    drawSolidArrow(app._window, {0.f, botLeft.y}, {windowSize.x * 1.f, 0.f}, app._axisColor);
    // vert axis
    drawSolidArrow(app._window, {botLeft.x, windowSize.y * 1.f}, {0.f, windowSize.y * -1.f}, app._axisColor);

    if (app._showGuides) {
        // segments
        const float xStretch = app._curve._xStretch;
        for (const EaseCurve::Segment& segment: app._curve._scaledSegments) {
            const float initalY = segment.evaluate(segment.initialX);
            const float finalY = segment.evaluate(segment.finalX);
            const sf::Color vcolor = (segment.finalX >= segment.initialX) ? app._guideColor : app._errorColor;
            const sf::Color hcolor = (finalY >= initalY) ? app._guideColor : app._errorColor;
            // horiz
            drawDottedLine(app._window, mapToScreen(app, {0.f, finalY}), mapToScreen(app, {segment.finalX * xStretch, finalY}), hcolor);
            // vert
            drawDottedLine(app._window, mapToScreen(app, {segment.finalX * xStretch, 0}), mapToScreen(app, {segment.finalX * xStretch, finalY}), vcolor);
        }
    }
}

void drawCurve(AppState& app)
{
    constexpr int linesPerSegment = 1000;
    const float xIncrement = app._curve._lastPoint.x / linesPerSegment;
    float prevX = 0.f;
    float prevY = app._curve.evaluate(prevX);
    for (int i = 1; i <= linesPerSegment; ++i) {
        const float x = i * xIncrement;
        const float y = app._curve.evaluate(x);
        drawSolidLine(app._window, mapToScreen(app, {prevX, prevY}), mapToScreen(app, {x, y}), app._curveColor);
        prevX = x;
        prevY = y;
    }
}

void drawSpeed(AppState& app)
{
    constexpr int linesPerSegment = 1000;
    if (!app._showSpeed) {
        return;
    }
    const float xIncrement = app._curve._lastPoint.x / linesPerSegment;
    float prevX = 0.f;
    float prevY = app._curve.speed(prevX);
    for (int i = 1; i <= linesPerSegment; ++i) {
        const float x = i * xIncrement;
        const float y = app._curve.speed(x);
        drawSolidLine(app._window, mapToScreen(app, {prevX, prevY}), mapToScreen(app, {x, y}), app._speedColor);
        prevX = x;
        prevY = y;
    }
}

void drawAccel(AppState& app)
{
    constexpr int linesPerSegment = 1000;
    if (!app._showAccel) {
        return;
    }
    const float xIncrement = app._curve._lastPoint.x / linesPerSegment;
    float prevX = 0.f;
    float prevY = app._curve.accel(prevX);
    for (int i = 1; i <= linesPerSegment; ++i) {
        const float x = i * xIncrement;
        const float y = app._curve.accel(x);
        drawSolidLine(app._window, mapToScreen(app, {prevX, prevY}), mapToScreen(app, {x, y}), app._accelColor);
        prevX = x;
        prevY = y;
    }
}

void drawCircles(AppState& app)
{
    if (!app._showCircles) {
        return;
    }

    const float xStretch = app._curve._xStretch;
    // first circle
    {
        const sf::Vector2f center = mapToScreen(app, {app._curve._scaledFirstCenter.x * xStretch, app._curve._scaledFirstCenter.y});

        // radius
        drawDottedLine(app._window, center, mapToScreen(app, EaseCurve::kFirstPoint), app._curveColor, 21);
        //drawSolidLine(app._window, center, mapToScreen(app, EaseCurve::kStartPoint), app._curveColor);

        // center
        drawDisc(app._window, center, 2.f, app._curveColor);
        //drawDot(app._window, center, app._curveColor);

        // circle
        drawDottedEllipse(app, {app._curve._scaledFirstCenter.x * xStretch, app._curve._scaledFirstCenter.y}, app._curve._firstRadius, xStretch, app._curveColor);
    }

    // last circle
    {
        const sf::Vector2f center = mapToScreen(app, {app._curve._scaledLastCenter.x * xStretch, app._curve._scaledLastCenter.y});

        // radius
        drawDottedLine(app._window, center, mapToScreen(app, app._curve._lastPoint), app._curveColor, 21);
        //drawSolidLine(app._window, center, mapToScreen(app, app._curve._lastPoint), app._curveColor);

        // center
        drawDisc(app._window, center, 2.f, app._curveColor);
        //drawDot(app._window, center, app._curveColor);

        // circle
        drawDottedEllipse(app, {app._curve._scaledLastCenter.x * xStretch, app._curve._scaledLastCenter.y}, app._curve._lastRadius, xStretch, app._curveColor);
    }

    // intermediate circles
    auto count = app._curve._points.size();
    for (decltype(count) i = 0; i < count; ++i) {
        const sf::Vector2f center = mapToScreen(app, {app._curve._scaledCenters[i].x * xStretch, app._curve._scaledCenters[i].y});

        // radius
        drawDottedLine(app._window, center, mapToScreen(app, app._curve._points[i]), app._curveColor, 21);
        //drawDisc(app._window, center, 2.f, app._curveColor);

        // center
        drawDisc(app._window, center, 2.f, app._curve._sides[i] == app._curve._originalSides[i] ? app._curveColor : app._errorColor);

        // circle
        drawDottedEllipse(app, {app._curve._scaledCenters[i].x * xStretch, app._curve._scaledCenters[i].y}, app._curve._radii[i], xStretch, app._curveColor);
    }
}

void drawPoly(AppState& app)
{
    if (!app._showPolyLine) {
        return;
    }
    EaseCurve::Point start = EaseCurve::kFirstPoint;
    auto i = app._curve._points.size();
    i = 0;
    for (EaseCurve::Point point: app._curve._points) {
        const bool valid = app._curve._pointsXValid[i] && app._curve._pointsYValid[i];
        const sf::Color color = valid ? app._curveColor : app._errorColor;
        drawDottedLine(app._window, mapToScreen(app, start), mapToScreen(app, point), color);
        start = point;
        ++i;
    }
    {
        drawDottedLine(app._window, mapToScreen(app, start), mapToScreen(app, app._curve._lastPoint), app._curveColor);
    }
}

void drawMouseCursor(AppState& app)
{
    sf::Vector2i pos = sf::Mouse::getPosition(app._window);
    std::array<sf::Vertex, 12> vertices;
    // 2 horizontal white lines
    vertices[0].position = sf::Vector2f{pos.x - 3.f, pos.y - 1.f};
    vertices[0].color = sf::Color::White;
    vertices[1].position = sf::Vector2f{pos.x + 3.f, pos.y - 1.f};
    vertices[1].color = sf::Color::White;
    vertices[2].position = sf::Vector2f{pos.x - 3.f, pos.y + 1.f};
    vertices[2].color = sf::Color::White;
    vertices[3].position = sf::Vector2f{pos.x + 3.f, pos.y + 1.f};
    vertices[3].color = sf::Color::White;
    // 2 vertical white lines
    vertices[4].position = sf::Vector2f{pos.x - 1.f, pos.y - 3.f};
    vertices[4].color = sf::Color::White;
    vertices[5].position = sf::Vector2f{pos.x - 1.f, pos.y + 3.f};
    vertices[5].color = sf::Color::White;
    vertices[6].position = sf::Vector2f{pos.x + 1.f, pos.y - 3.f};
    vertices[6].color = sf::Color::White;
    vertices[7].position = sf::Vector2f{pos.x + 1.f, pos.y + 3.f};
    vertices[7].color = sf::Color::White;
    // 1 horizontal black line
    vertices[8].position = sf::Vector2f{pos.x - 3.f, pos.y + 0.f};
    vertices[8].color = sf::Color::Black;
    vertices[9].position = sf::Vector2f{pos.x + 3.f, pos.y + 0.f};
    vertices[9].color = sf::Color::Black;
    // 1 vertical black line
    vertices[10].position = sf::Vector2f{pos.x + 0.f, pos.y - 3.f};
    vertices[10].color = sf::Color::White;
    vertices[11].position = sf::Vector2f{pos.x + 0.f, pos.y + 3.f};
    vertices[11].color = sf::Color::White;
    app._window.draw(vertices.data(), vertices.size(), sf::Lines);
    //app._window.setMouseCursorVisible(false);
}

} // namespace

void render(AppState& app)
{
    app._window.clear(app._windowBg);
    drawCoordinates(app);
    drawPoly(app);
    drawCircles(app);
    drawCurve(app);
    drawSpeed(app);
    drawAccel(app);

    drawMouseCursor(app);
}
