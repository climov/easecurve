#ifndef APPSTATE_H
#define APPSTATE_H

#include "easecurve.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

struct AppState
{
    sf::RenderWindow _window;
    sf::Font _font;
    EaseCurve _curve {};

    // settings
    sf::Color _windowBg = sf::Color::Black;
    sf::Color _xAxisColor = sf::Color::Green;
    sf::Color _yAxisColor = sf::Color::Green;
    sf::Color _topColor = sf::Color::Green;
    sf::Color _rightColor = sf::Color::Green;
    sf::Color _curveColor = sf::Color::White;
    sf::Color _speedColor = sf::Color::Yellow;
    sf::Color _accelColor = sf::Color::Cyan;

    sf::Vector2i _border { 50, 50 };
    bool         _showCircles = true;
    bool         _showSpeed = true;
    bool         _showAccel = true;
    bool         _keepAspectRatio = true;

    // calculated
};

void render(AppState& app);

#endif // APPSTATE_H
