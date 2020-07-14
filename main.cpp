
#include "appstate.h"

//#include "filesystem.h"

//#include <fmt/format.h>

#include <SFML/Window/Event.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

namespace im = ImGui;

namespace {

void fixAspectRatioByX(AppState& app)
{
    if (app._keepAspectRatio) {
        const float ratio = app._curve._lastPoint.y / app._curve._lastPoint.x;
        const sf::Vector2u windowSizeU = app._window.getSize();
        const sf::Vector2i windowSize = {static_cast<int>(windowSizeU.x), static_cast<int>(windowSizeU.y)};
        const int width = windowSize.x - 2 * app._border.x;
        const int height = static_cast<int>(width * ratio);
        if (height <= windowSize.y) {
            app._border.y = (windowSize.y - height) / 2;
            return;
        }
        const int newWidth = static_cast<int>(windowSize.y / ratio);
        app._border.x = (windowSize.x - newWidth)  / 2;
    }
}

void fixAspectRatioByY(AppState& app)
{
    if (app._keepAspectRatio) {
        const float ratio = app._curve._lastPoint.x / app._curve._lastPoint.y;
        const sf::Vector2u windowSizeU = app._window.getSize();
        const sf::Vector2i windowSize = {static_cast<int>(windowSizeU.x), static_cast<int>(windowSizeU.y)};
        const int height = windowSize.y - 2 * app._border.y;
        const int width = static_cast<int>(height * ratio);
        if (width <= windowSize.x) {
            app._border.x = (windowSize.x - width) / 2;
            return;
        }
        const int newHeight = static_cast<int>(windowSize.x / ratio);
        app._border.y = (windowSize.y - newHeight)  / 2;
    }
}

void fixAspectRatio(AppState& app)
{
    if (app._keepAspectRatio) {
        const sf::Vector2u windowSizeU = app._window.getSize();
        const sf::Vector2i windowSize = {static_cast<int>(windowSizeU.x), static_cast<int>(windowSizeU.y)};
        const float xRatio = windowSize.x / app._curve._lastPoint.x;
        const float yRatio = windowSize.y / app._curve._lastPoint.y;
        if (xRatio >= yRatio) {
            fixAspectRatioByX(app);
        } else {
            fixAspectRatioByY(app);
        }
    }
}

void init(AppState& app)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    //_window.create(sf::VideoMode{1000, 1000}, "Easing Curve", sf::Style::Default, settings);
    app._window.create(sf::VideoMode::getDesktopMode(), "Easing Curve", sf::Style::Default, settings);
    //game._window.create(sf::VideoMode::getFullscreenModes()[0], "Easing Curve", sf::Style::Fullscreen, settings);
    app._window.setVerticalSyncEnabled(true);
    //game._window.requestFocus();
    im::SFML::Init(app._window, false);

    im::GetIO().Fonts->Clear();
    im::GetIO().Fonts->AddFontDefault();
    im::SFML::UpdateFontTexture();

    //fdbg("Texture max size: {}", sf::Texture::getMaximumSize());
    app._curve.setLastPoint(10.f, 20.f);
    app._curve.setRadius(1.f);
    //app._curve.addPoint(0.2f, 0.3f);
    //app._curve.addPoint(0.6f, 0.4f);
    //app._curve.addPoint(0.8f, 0.6f);

    fixAspectRatio(app);
}

bool ColorEdit(const char* label, sf::Color& color)
{
    float rgb[3];
    rgb[0] = color.r / 255.0f;
    rgb[1] = color.g / 255.0f;
    rgb[2] = color.b / 255.0f;
    if (im::ColorEdit3(label, rgb)) {
        color.r = static_cast<sf::Uint8>(rgb[0] * 255);
        color.g = static_cast<sf::Uint8>(rgb[1] * 255);
        color.b = static_cast<sf::Uint8>(rgb[2] * 255);
        return true;
    }
    return false;
};

bool ColorEditAlpha(const char* label, sf::Color& color)
{
    float rgba[4];
    rgba[0] = color.r / 255.0f;
    rgba[1] = color.g / 255.0f;
    rgba[2] = color.b / 255.0f;
    rgba[3] = color.a / 255.0f;
    if (im::ColorEdit4(label, rgba)) {
        color.r = static_cast<sf::Uint8>(rgba[0] * 255);
        color.g = static_cast<sf::Uint8>(rgba[1] * 255);
        color.b = static_cast<sf::Uint8>(rgba[2] * 255);
        color.a = static_cast<sf::Uint8>(rgba[3] * 255);
        return true;
    }
    return false;
};

void exec(AppState& app)
{
    sf::Clock imguiClock;

    sf::Clock fpsClock;
    int frameCount = 0;
    int fps = 0;

    while (true) {
        sf::Event event;
        while (app._window.pollEvent(event)) {
            im::SFML::ProcessEvent(event);
            switch (event.type) {
            case sf::Event::Closed:
                app._window.close();
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                case sf::Keyboard::Num0:
                    break;
                case sf::Keyboard::Equal:
                    break;
                case sf::Keyboard::Z:
                    break;
                case sf::Keyboard::X:
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        if (fpsClock.getElapsedTime().asSeconds() > 5) {
            fps = static_cast<int>(frameCount / fpsClock.restart().asSeconds());
            frameCount = 0;
        }

        im::SFML::Update(app._window, imguiClock.restart());
        const bool hasFocus = app._window.hasFocus();
        if (hasFocus) {
            if (/*sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || */sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                app._window.close();
            }
        }

        const sf::Vector2u windowSize = app._window.getSize();
        im::Begin("Info");

        im::Text("%d fps", fps);
        im::Spacing();
        //const float maxRadius = std::min(app._curve._lastPoint.x, app._curve._lastPoint.y);
        const float maxRadius = 100.f;
        if (im::CollapsingHeader("Curve", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (app._curve._valid) {
                im::TextUnformatted("Curve is valid");
            } else {
                im::PushStyleColor(ImGuiCol_Text, {1.f, 0.f, 0.f, 1.f});
                im::TextUnformatted("Curve is invalid");
                im::PopStyleColor();
            }

            im::Spacing();
            //if (im::SliderFloat("Min. Distance", &app._curve._minDistance, 0.f, maxRadius)) {
            //    solve(app._curve);
            //}
            if (im::SliderFloat("X Stretch Factor", &app._curve._xStretch, 0.1f, 10.f)) {
                app._curve.solve();
            }
            im::Separator();
            if (im::SliderFloat("Radius", &app._curve._radius, 0.f, maxRadius)) {
                app._curve.setRadius();
            }
            im::Separator();
            if (im::SliderFloat("First Radius", &app._curve._firstRadius, 0.f, maxRadius)) {
                app._curve.solve();
            }
            if (im::SliderFloat("Last Radius", &app._curve._lastRadius, 0.f, maxRadius)) {
                app._curve.solve();
            }
            im::Separator();
            if (im::SliderFloat("Last Point X", &app._curve._lastPoint.x, 0.f, 100.f)) {
                fixAspectRatioByY(app);
                app._curve.solve();
            }
            if (im::SliderFloat("Last Point Y", &app._curve._lastPoint.y, 0.f, 100.f)) {
                fixAspectRatioByX(app);
                app._curve.solve();
            }
        }

        im::Spacing();
        if (im::CollapsingHeader("Intermediate points", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (app._curve._points.empty()) {
                if (im::Button("Add Intermediate Point")) {
                    app._curve.addPoint(app._curve._lastPoint.x / 2, app._curve._lastPoint.y / 2);
                }
            } else {
                auto count = app._curve._points.size();
                float min_x = 0.f;
                float max_x = 1.f;
                float min_y = 0.f;
                float max_y = 1.f;
                im::PushID("points");
                im::Spacing();
                for (decltype(count) i = 0; i < count; ++i) {
                    im::PushID(static_cast<int>(i));
                    if (i < count - 1) {
                        max_x = app._curve._points[i+1].x;
                        max_y = app._curve._points[i+1].y;
                    } else {
                        max_x = app._curve._lastPoint.x;
                        max_y = app._curve._lastPoint.y;
                    }
                    if (im::Button("+")) {
                        app._curve.insertPointAt(static_cast<int>(i), (app._curve._points[i].x + min_x) / 2, (app._curve._points[i].y + min_y) / 2);
                    }
                    im::Spacing();
                    if (im::Button("-")) {
                        app._curve.removePointAt(static_cast<int>(i));
                    }
                    im::SameLine();
                    if (im::SliderFloat("x", &app._curve._points[i].x, min_x, max_x)) {
                        app._curve.solve();
                    }
                    im::Indent();
                    if (im::SliderFloat("y", &app._curve._points[i].y, min_y, max_y)) {
                        app._curve.solve();
                    }
                    if (im::SliderFloat("r", &app._curve._radii[i], 0.f, maxRadius)) {
                        app._curve.solve();
                    }
                    im::Unindent();
                    min_x = app._curve._points[i].x;
                    min_y = app._curve._points[i].y;
                    im::PopID();
                    im::Spacing();
                }
                {
                    im::PushID(static_cast<int>(count));
                    if (im::Button("+")) {
                        app._curve.addPoint((max_x + min_x) / 2, (max_y + min_y) / 2);
                    }
                    im::PopID();
                }
                im::PopID();
            }
        }

        im::Spacing();
        if (im::CollapsingHeader("Show", ImGuiTreeNodeFlags_DefaultOpen)) {
            im::Checkbox("Circles", &app._showCircles);
            im::Checkbox("Speed", &app._showSpeed);
            im::Checkbox("Acceleration", &app._showAccel);
        }

        im::Spacing();
        if (im::CollapsingHeader("Colors"/*, ImGuiTreeNodeFlags_DefaultOpen*/)) {
            ColorEdit("Window Bg", app._windowBg);
            ColorEditAlpha("Curve", app._curveColor);
            ColorEditAlpha("X Axis", app._xAxisColor);
            ColorEditAlpha("Y Axis", app._yAxisColor);
            ColorEditAlpha("X guides", app._topColor);
            ColorEditAlpha("Y guides", app._rightColor);
            ColorEditAlpha("Speed", app._speedColor);
            ColorEditAlpha("Acceleration", app._accelColor);
        }

        im::Spacing();
        if (im::CollapsingHeader("Viewport", ImGuiTreeNodeFlags_DefaultOpen)) {
            const sf::Vector2i viewport {static_cast<int>(windowSize.x) - 2 * app._border.x, static_cast<int>(windowSize.y) - 2 * app._border.y};
            im::Text("Size: %dx%d", viewport.x, viewport.y);
            im::Spacing();
            if (im::Checkbox("Keep Aspect Ratio", &app._keepAspectRatio)) {
                fixAspectRatio(app);
            }
            if (im::SliderInt("H. Border", &app._border.x, 0, windowSize.x / 2)) {
                fixAspectRatioByX(app);
            }
            if (im::SliderInt("V. Border", &app._border.y, 0, windowSize.y / 2)) {
                fixAspectRatioByY(app);
            }
        }

        im::Spacing(); im::Separator(); im::Spacing();
        if (im::Button("[Q]uit")) {
            app._window.close();
        }

        im::End();

        if (!app._window.isOpen()) {
            break;
        }

        render(app);

        //im::ShowDemoWindow();
        //im::ShowMetricsWindow();

        im::SFML::Render(app._window);

        app._window.display();
        ++frameCount;
    }
    im::SFML::Shutdown();
}

} // namespace

int main()
{
    AppState app;
    init(app);
    exec(app);

    return 0;
}
