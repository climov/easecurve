
#include "alx/trace.h"

import std;

import sokol;
import sokol.color;

import imgui;

import alx.assert;
import alx.va;

import main.appstate;

using namespace sokol::color;
namespace im = ImGui;
namespace va = alx::va;

namespace {

void fixAspectRatioByX(AppState& app)
{
    if (app._keepAspectRatio) {
        const float ratio = app._path.endProgress / app._path.endTime;
        const va::Vec2i windowSize = {{sapp_width(), sapp_height()}};
        const int width = windowSize.x() - 2 * app._border.x();
        const int height = static_cast<int>(static_cast<float>(width) * ratio);
        if (height <= windowSize.y()) {
            app._border.y() = (windowSize.y() - height) / 2;
            return;
        }
        const int newWidth = static_cast<int>(static_cast<float>(windowSize.y()) / ratio);
        app._border.x() = (windowSize.x() - newWidth)  / 2;
    }
}

void fixAspectRatioByY(AppState& app)
{
    if (app._keepAspectRatio) {
        const float ratio = app._path.endTime / app._path.endProgress;
        const va::Vec2i windowSize = {{sapp_width(), sapp_height()}};
        const int height = windowSize.y() - 2 * app._border.y();
        const int width = static_cast<int>(static_cast<float>(height) * ratio);
        if (width <= windowSize.x()) {
            app._border.x() = (windowSize.x() - width) / 2;
            return;
        }
        const int newHeight = static_cast<int>(static_cast<float>(windowSize.x()) / ratio);
        app._border.y() = (windowSize.y() - newHeight)  / 2;
    }
}

void fixAspectRatio(AppState& app)
{
    if (app._keepAspectRatio) {
        const va::Vec2i windowSize = {{sapp_width(), sapp_height()}};
        const float xRatio = static_cast<float>(windowSize.x()) / app._path.endTime;
        const float yRatio = static_cast<float>(windowSize.y()) / app._path.endProgress;
        if (xRatio >= yRatio) {
            fixAspectRatioByX(app);
        } else {
            fixAspectRatioByY(app);
        }
    }
}

void init(AppState& app)
{
    //sf::ContextSettings settings;
    //settings.antialiasingLevel = 8;
    ////_window.create(sf::VideoMode{1000, 1000}, "Easing Curve", sf::Style::Default, settings);
    //app._window.create(sf::VideoMode::getDesktopMode(), "Easing Curve", sf::Style::Default, settings);
    ////game._window.create(sf::VideoMode::getFullscreenModes()[0], "Easing Curve", sf::Style::Fullscreen, settings);
    //app._window.setVerticalSyncEnabled(true);
    ////game._window.requestFocus();
    //im::SFML::Init(app._window, false);

    //im::GetIO().Fonts->Clear();
    //im::GetIO().Fonts->AddFontDefault();
    //im::SFML::UpdateFontTexture();

    ////fdbg("Texture max size: {}", sf::Texture::getMaximumSize());
    //app._curve.setLastPoint(10.f, 20.f);
   // app._curve.setRadius(1.f);
    //app._curve.addPoint(0.2f, 0.3f);
    //app._curve.addPoint(0.6f, 0.4f);
    //app._curve.addPoint(0.8f, 0.6f);

    // app._path = {
    //     .startTime          = 0,
    //     .startProgress      = 0,
    //     .startVelocity      = 0,
    //     .startEaseDuration  = 3,
    //     .endTime            = 30,
    //     .endProgress        = 100,
    //     .endVelocity        = 0,
    //     .endEaseDuration    = 3,
    //     .checkpoints        = {
    //         { .time = 8,    .progress = 20.f,   .easeDuration= 5    },
    //         { .time = 14,   .progress = 50.f,   .easeDuration= 3    },
    //         { .time = 21,   .progress = 70.f,   .easeDuration= 3    },
    //     },
    // };
    app._path = {
        .startTime          = 12.786304f,
        .startProgress      = 0.0f,
        .startEaseDuration  = 5.0f,
        .endTime            = 65.780289f,
        .endProgress        = 1.0f,
        .endEaseDuration    = 5.0f,
        .checkpoints = {
            { .time = 24.17927f,    .progress = 0.158143f,  .easeDuration= 5.0f },
            { .time = 31.903965f,   .progress = 0.332121f,  .easeDuration= 4.4f },
            { .time = 36.018059f,   .progress = 0.388404f,  .easeDuration= 5.0f },
            { .time = 38.461151f,   .progress = 0.433527f,  .easeDuration= 5.0f },
            { .time = 41.602318f,   .progress = 0.481825f,  .easeDuration= 5.0f },
            { .time = 45.8325f,     .progress = 0.543464f,  .easeDuration= 5.0f },
            { .time = 53.95076f,    .progress = 0.686501f,  .easeDuration= 5.0f },
            { .time = 58.137657f,   .progress = 0.865331f,  .easeDuration= 5.0f },
        }
    };
    solve(app, app._resultsLinear, true);
    solve(app, app._resultsSine, true);

    fixAspectRatio(app);
}

bool ColorEdit(const char* label, sg_color& color)
{
    std::array<float, 3> rgb = {color.r, color.g, color.b};
    if (im::ColorEdit3(label, rgb.data())) {
        color.r = rgb[0];
        color.g = rgb[1];
        color.b = rgb[2];
        return true;
    }
    return false;
};

bool ColorEditAlpha(const char* label, sg_color& color)
{
    std::array<float, 4> rgba = {color.r, color.g, color.b, color.a};
    if (im::ColorEdit4(label, rgba.data())) {
        color.r = rgba[0];
        color.g = rgba[1];
        color.b = rgba[2];
        color.a = rgba[3];
        return true;
    }
    return false;
};

//template <typename... Ts>
//void saveValues(std::ostream& os, std::string_view name, const Ts&... values)
//{
//    os << name;
//    ((os << " " << values), ...);
//    os << "\n";
//}

//void saveRgb(std::ostream& os, std::string_view name, sf::Color c)
//{
//    saveValues(os, name, static_cast<int>(c.r), static_cast<int>(c.g), static_cast<int>(c.b));
//}
//void loadRgb(std::istream& is, sf::Color& c)
//{
//    int v = 0;
//    if (!is.eof()) {
//        is >> v;
//        c.r = static_cast<sf::Uint8>(v);
//    }
//    if (!is.eof()) {
//        is >> v;
//        c.g = static_cast<sf::Uint8>(v);
//    }
//    if (!is.eof()) {
//        is >> v;
//        c.b = static_cast<sf::Uint8>(v);
//    }
//}

//void saveRgba(std::ostream& os, std::string_view name, sf::Color c)
//{
//    saveValues(os, name, static_cast<int>(c.r), static_cast<int>(c.g), static_cast<int>(c.b), static_cast<int>(c.a));
//}
//void loadRgba(std::istream& is, sf::Color& c)
//{
//    int v = 0;
//    if (!is.eof()) {
//        is >> v;
//        c.r = static_cast<sf::Uint8>(v);
//    }
//    if (!is.eof()) {
//        is >> v;
//        c.g = static_cast<sf::Uint8>(v);
//    }
//    if (!is.eof()) {
//        is >> v;
//        c.b = static_cast<sf::Uint8>(v);
//    }
//    if (!is.eof()) {
//        is >> v;
//        c.a = static_cast<sf::Uint8>(v);
//    }
//}

//void saveCurve(std::ostream& os, std::string name, const EaseCurve& curve)
//{
//    for (char& c: name) {
//        if (std::isspace(c)) {
//            c = '_';
//        }
//    }
//    saveValues(os, "Curve", name);
//    saveValues(os, "lastPoint",     curve._lastPoint.x, curve._lastPoint.y);
//    saveValues(os, "xStretch",      curve._xStretch);
//    saveValues(os, "radius",        curve._radius);
//    saveValues(os, "firstRadius",   curve._firstRadius);
//    saveValues(os, "lastRadius",    curve._lastRadius);
//    saveValues(os, "minDistance",   curve._minDistance); //
//    saveValues(os, "precision",     curve._precision); //
//    saveValues(os, "autoFlip",      curve._autoFlip);
//    saveValues(os, "reduceRadii",   curve._reduceRadii); //
//    auto size = curve._points.size();
//    for (decltype (size) i = 0; i < size; ++i) {
//        saveValues(os, "addPoint", curve._points[i].x, curve._points[i].y, curve._radii[i]);
//    }
//    os << "\n";
//}
//std::string loadCurve(std::istream& is, std::string& name, EaseCurve& curve)
//{
//    std::string str;
//    if (is.eof()) {
//        return str;
//    }
//    is >> name;
//    for (char& c: name) {
//        if (c == '_') {
//            c = ' ';
//        }
//    }
//    while (!is.eof()) {
//        is >> str;
//        if (str == "Curve")             break;
//        else if (str == "Settings")     break;
//        else if (str == "lastPoint")    is >> curve._lastPoint.x >> curve._lastPoint.y;
//        else if (str == "radius")       is >> curve._radius;
//        else if (str == "xStretch")     is >> curve._xStretch;
//        else if (str == "firstRadius")  is >> curve._firstRadius;
//        else if (str == "lastRadius")   is >> curve._lastRadius;
//        else if (str == "minDistance")  is >> curve._minDistance;
//        else if (str == "precision")    is >> curve._precision;
//        else if (str == "autoFlip")     is >> curve._autoFlip;
//        else if (str == "reduceRadii")  is >> curve._reduceRadii;
//        else if (str == "addPoint") {
//            float x = 0;
//            float y = 0;
//            float r = 0;
//            is >> x >> y >> r;
//            curve.addPoint(x, y, r);
//        }
//        str.clear();
//    }
//    curve.solve();
//    return str;
//}

//void save(const AppState& app)
//{
//    std::ofstream os {"easecurve.state", std::ios::binary};
//    if (!os) {
//        return;
//    }

//    os << std::boolalpha;
//    os << "Settings\n";

//    saveRgb (os, "windowBg",   app._windowBg);
//    saveRgba(os, "axisColor",  app._axisColor);
//    saveRgba(os, "guideColor", app._guideColor);
//    saveRgba(os, "curveColor", app._curveColor);
//    saveRgba(os, "speedColor", app._speedColor);
//    saveRgba(os, "accelColor", app._accelColor);
//    saveRgba(os, "errorColor", app._errorColor);

//    saveValues(os, "border",            app._border.x, app._border.y);
//    saveValues(os, "selectedCurve",     app._selectedCurve);
//    saveValues(os, "showCircles",       app._showCircles);
//    saveValues(os, "showSpeed",         app._showSpeed);
//    saveValues(os, "showAccel",         app._showAccel);
//    saveValues(os, "showGuides",        app._showGuides);
//    saveValues(os, "showPolyLine",      app._showPolyLine);
//    saveValues(os, "keepAspectRatio",   app._keepAspectRatio);

//    os << "\n";

//    if (app._curves.empty()) {
//        saveCurve(os, "Untitled", app._curve);
//    } else {
//        for (const AppState::CurveData& curve: app._curves) {
//            saveCurve(os, curve._name, curve._saved);
//        }
//    }
//}
//void load(AppState& app)
//{
//    std::ifstream is {"easecurve.state", std::ios::binary};
//    if (!is) {
//        return;
//    }
//    is >> std::boolalpha;

//    std::string str;
//    if (!is.eof()) {
//        is >> str;
//        do {
//            if (str == "Settings") {
//                // load the settings;
//                while (true) {
//                    if (is.eof()) {
//                        break;
//                    }
//                    is >> str;
//                    if (str == "Curve") break;
//                    else if (str == "windowBg")         loadRgb(is, app._windowBg);
//                    else if (str == "axisColor")        loadRgba(is, app._axisColor);
//                    else if (str == "guideColor")       loadRgba(is, app._guideColor);
//                    else if (str == "curveColor")       loadRgba(is, app._curveColor);
//                    else if (str == "speedColor")       loadRgba(is, app._speedColor);
//                    else if (str == "accelColor")       loadRgba(is, app._accelColor);
//                    else if (str == "errorColor")       loadRgba(is, app._errorColor);
//                    else if (str == "border")           is >> app._border.x >> app._border.y;
//                    else if (str == "selectedCurve")    is >> app._selectedCurve;
//                    else if (str == "showCircles")      is >> app._showCircles;
//                    else if (str == "showSpeed")        is >> app._showSpeed;
//                    else if (str == "showAccel")        is >> app._showAccel;
//                    else if (str == "showGuides")       is >> app._showGuides;
//                    else if (str == "showPolyLine")     is >> app._showPolyLine;
//                    else if (str == "keepAspectRatio")  is >> app._keepAspectRatio;
//                }
//            }
//            if (str == "Curve") {
//                std::string name;
//                EaseCurve curve;
//                str = loadCurve(is, name, curve);
//                app._curve = curve;
//                //app._curves.push_back({name, curve, curve});
//            }
//        } while (!is.eof());
//    }
//}

//sg_pass_action pass_action = {};
sg_desc desc = {};

void init(void* userData)
{
    AppState& app = *static_cast<AppState*>(userData);
    init(app);
    //load(app);

    // Initialize Sokol GFX.
    desc.context = sapp_sgcontext();
    desc.logger.func = slog_func;
    sg_setup(&desc);
    if(!sg_isvalid()) {
        std::print("Failed to create Sokol GFX context\n");
        std::exit(-1);
    }

    simgui_desc_t imguidesc = {};
    simgui_setup(&imguidesc);

    // Initialize Sokol GP, adjust the size of command buffers for your own use.
    const sgp_desc sgpdesc = {};
    sgp_setup(&sgpdesc);
    if(!sgp_is_valid()) {
        std::print("Failed to create Sokol GP context: {}\n", sgp_get_error_message(sgp_get_last_error()));
        std::exit(-1);
    }
}

void cleanup(void* /*userData*/)
{
    //AppState& app = *static_cast<AppState*>(userData);

    simgui_shutdown();
    sgp_shutdown();
    sg_shutdown();

    //save(app);
}

void event(const sapp_event* ev, void* userData)
{
    AppState& app = *static_cast<AppState*>(userData);

    switch (ev->type) {
    case SAPP_EVENTTYPE_QUIT_REQUESTED:
        sapp_quit();
        break;
    case SAPP_EVENTTYPE_KEY_DOWN:
        switch (ev->key_code) {
        case SAPP_KEYCODE_Q:
            sapp_quit();
            break;
        case SAPP_KEYCODE_C:
            //app._curve = {};
            //app._curve.solve();
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    if (simgui_handle_event(ev)) {
        return;
    }
    //fmt::print("{}\n", ev->type);
    //std::cout.flush();

    switch (ev->type) {
    case SAPP_EVENTTYPE_MOUSE_MOVE:
        app._mouse.x() = ev->mouse_x;
        app._mouse.y() = ev->mouse_y;
        break;
    default:
        break;
    }
}

void frame(void* userData)
{
    [[maybe_unused]] const auto now = std::chrono::high_resolution_clock::now();
    AppState& app = *static_cast<AppState*>(userData);

    // Begin a render pass.
    const va::Vec2i windowSize = {{sapp_width(), sapp_height()}};

    const sg_pass_action pass_action = {};
    sg_begin_default_pass(&pass_action, windowSize.x(), windowSize.y());

    const simgui_frame_desc_t framedesc = {
        .width = sapp_width(),
        .height = sapp_height(),
        .delta_time = sapp_frame_duration(),
        .dpi_scale = sapp_dpi_scale()
    };
    simgui_new_frame(&framedesc);

    im::SetNextWindowSize({0.f, 0.f});
    im::Begin("Info");

    im::Text("%d fps", static_cast<int>(1./sapp_frame_duration()));
    im::Spacing();
    //const float maxRadius = std::min(app._curve._lastPoint.x, app._curve._lastPoint.y);
    //constexpr float maxRadius = 100.f;
    if (im::CollapsingHeader("Curve", ImGuiTreeNodeFlags_DefaultOpen)) {
        //if (app._curve._valid) {
        //    im::TextUnformatted("Curve is valid");
        //} else {
        //    im::PushStyleColor(ImGuiCol_Text, {1.f, 0.f, 0.f, 1.f});
        //    im::TextUnformatted("Curve is invalid");
        //    im::PopStyleColor();
        //}

        im::Spacing();
        //if (im::SliderFloat("Min. Distance", &app._curve._minDistance, 0.f, maxRadius)) {
        //    app._curve.solve();
        //}
        //if (im::SliderFloat("Precision", &app._curve._precision, 0.f, 1.f, "%.6f")) {
        //    app._curve.solve();
        //}
        //if (im::SliderFloat("X Stretch Factor", &app._curve._xStretch, 0.1f, 10.f)) {
        //    app._curve.solve();
        //}
        //im::Separator();
        //if (im::SliderFloat("Radius", &app._curve._radius, 0.f, maxRadius)) {
        //    app._curve.setRadius();
        //}
        //im::Separator();
        //if (im::SliderFloat("First Radius", &app._curve._firstRadius, 0.f, maxRadius)) {
        //    app._curve.solve();
        //}
        //if (im::SliderFloat("Last Radius", &app._curve._lastRadius, 0.f, maxRadius)) {
        //    app._curve.solve();
        //}
        im::Separator();
        if (im::SliderFloat("End Time", &app._path.endTime, 0.f, 100.f)) {
            fixAspectRatioByY(app);
            solve(app, app._resultsLinear, true);
            solve(app, app._resultsSine, true);
        }
        if (im::SliderFloat("End Progress", &app._path.endProgress, 0.f, 100.f)) {
            fixAspectRatioByX(app);
            solve(app, app._resultsLinear, true);
            solve(app, app._resultsSine, true);
        }
        //im::Separator();
        //if (im::Checkbox("Autoflip", &app._curve._autoFlip)) {
        //    app._curve.solve();
        //}
        //if (im::Checkbox("Reduce Radii", &app._curve._reduceRadii)) {
        //    app._curve.solve();
        //}
        //im::Text("Iterations: %d", app._curve._iterations);
        //im::Text("Time: %lld us", app._curve._solveTimeUs);
    }

    im::Spacing();
    if (im::CollapsingHeader("Adjusted ease durations", ImGuiTreeNodeFlags_DefaultOpen)) {
        {
            const float maxVal = std::min(app._path.startEaseDuration, (app._path.checkpoints.empty() ? app._path.endTime : app._path.checkpoints.front().time) - app._path.startTime);
            im::PushID("easeDuration");
            if (im::SliderFloat("Start", &app._path.adjustedStartEaseDuration, 0.f, maxVal)) {
                alignEaseDurations(app._path, -1);
                solve(app, app._resultsLinear, false);
                solve(app, app._resultsSine, false);
            }
        }
        int i = 0;
        float prevTime = app._path.startTime;
        for (Checkpoint& checkpoint: app._path.checkpoints) {
            im::PushID(i);
            const float currentTime = checkpoint.time;
            const float nextTime = i == static_cast<int>(app._path.checkpoints.size()) - 1 ? app._path.endTime : app._path.checkpoints[static_cast<size_t>(i + 1)].time;
            const float maxVal = std::min(checkpoint.easeDuration, std::min(currentTime - prevTime, nextTime - currentTime) * 2.f);
            if (im::SliderFloat(std::format("Checkpoint {}", i).c_str(), &checkpoint.adjustedEaseDuration, 0.f, maxVal)) {
                alignEaseDurations(app._path, i);
                solve(app, app._resultsLinear, false);
                solve(app, app._resultsSine, false);
            }
            ++i;
            prevTime = currentTime;
            im::PopID();
        }
        im::PopID();
        const float maxVal = std::min(app._path.endEaseDuration, app._path.endTime - prevTime);
        if (im::SliderFloat("End", &app._path.adjustedEndEaseDuration, 0.f, maxVal)) {
            alignEaseDurations(app._path, static_cast<int>(app._path.checkpoints.size()));
            solve(app, app._resultsLinear, false);
            solve(app, app._resultsSine, false);
        }
        if (im::Button("Auto calculate (P)")) {
            adjustEaseDurationsP(app._path);
            solve(app, app._resultsLinear, false);
            solve(app, app._resultsSine, false);
        }
        if (im::Button("Auto calculate (C1)")) {
            adjustEaseDurations1(app._path);
            solve(app, app._resultsLinear, false);
            solve(app, app._resultsSine, false);
        }
        if (im::Button("Auto calculate (C2)")) {
            adjustEaseDurations2(app._path);
            solve(app, app._resultsLinear, false);
            solve(app, app._resultsSine, false);
        }
    }
    im::Spacing();
    if (im::CollapsingHeader("Intermediate points", ImGuiTreeNodeFlags_DefaultOpen)) {
        // if (app._curve._points.empty()) {
        //     if (im::Button("Add Intermediate Point")) {
        //         app._curve.addPoint(app._curve._lastPoint.x() / 2, app._curve._lastPoint.y() / 2);
        //     }
        // } else {
        //     auto count = app._curve._points.size();
        //     float min_x = 0.f;
        //     float max_x = 1.f;
        //     float min_y = 0.f;
        //     float max_y = 1.f;
        //     im::PushID("points");
        //     im::Spacing();
        //     for (decltype(count) i = 0; i < count; ++i) {
        //         im::PushID(static_cast<int>(i));
        //         if (i < count - 1) {
        //             max_x = app._curve._points[i+1].x();
        //             max_y = app._curve._points[i+1].y();
        //         } else {
        //             max_x = app._curve._lastPoint.x();
        //             max_y = app._curve._lastPoint.y();
        //         }
        //         if (im::Button("+")) {
        //             app._curve.insertPointAt(static_cast<int>(i), (app._curve._points[i].x() + min_x) / 2, (app._curve._points[i].y() + min_y) / 2);
        //         }
        //         im::Spacing();
        //         if (im::Button("-")) {
        //             app._curve.removePointAt(static_cast<int>(i));
        //         }
        //         im::SameLine();
        //         if (im::SliderFloat("x", &app._curve._points[i].x(), min_x, max_x)) {
        //             app._curve.solve();
        //         }
        //         im::Indent();
        //         if (im::SliderFloat("y", &app._curve._points[i].y(), min_y, max_y)) {
        //             app._curve.solve();
        //         }
        //         if (im::SliderFloat("r", &app._curve._radii[i], 0.f, maxRadius)) {
        //             app._curve.solve();
        //         }
        //         im::Unindent();
        //         min_x = app._curve._points[i].x();
        //         min_y = app._curve._points[i].y();
        //         im::PopID();
        //         im::Spacing();
        //     }
        //     {
        //         im::PushID(static_cast<int>(count));
        //         if (im::Button("+")) {
        //             app._curve.addPoint((max_x + min_x) / 2, (max_y + min_y) / 2);
        //         }
        //         im::PopID();
        //     }
        //     im::PopID();
        // }
    }

    im::Spacing();
    if (im::CollapsingHeader("Show", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (im::Checkbox("Use non-linear (sine) easing", &app._useSineEasing)) {
            app._selectedResults = app._useSineEasing ? &app._resultsSine : &app._resultsLinear;
        }
        int selectedResult = std::min(app._selectedResult, static_cast<int>(app._selectedResults->size()) - 1);
        if (im::SliderInt("Result Step", &selectedResult, 0, static_cast<int>(app._selectedResults->size()) - 1)) {
            app._selectedResult = selectedResult;
        }
        im::Text("Error: %f", (*app._selectedResults)[static_cast<size_t>(selectedResult)].totalErrorAbs);
        //im::Checkbox("Circles", &app._showCircles);
        im::Checkbox("Speed", &app._showSpeed);
        im::Checkbox("Acceleration", &app._showAccel);
        im::Checkbox("Guides", &app._showGuides);
        im::Checkbox("Poly Line", &app._showPolyLine);
    }

    im::Spacing();
    if (im::CollapsingHeader("Colors"/*, ImGuiTreeNodeFlags_DefaultOpen*/)) {
        ColorEdit("Window Bg", app._windowBg);
        if (ColorEditAlpha("Curve", app._curveColor)) {
            app._speedColor = sg_color_lerp(app._curveColor, sg_white, .70f); // sg_yellow;
            app._accelColor = sg_color_lerp(app._curveColor, sg_black, .25f); //sg_cyan;

        }
        ColorEditAlpha("Axis", app._axisColor);
        ColorEditAlpha("Guides", app._guideColor);
        //ColorEditAlpha("Speed", app._speedColor);
        //ColorEditAlpha("Acceleration", app._accelColor);
        ColorEditAlpha("Error", app._errorColor);
    }

    im::Spacing();
    if (im::CollapsingHeader("Viewport", ImGuiTreeNodeFlags_DefaultOpen)) {
        const va::Vec2i viewport {{windowSize.x() - 2 * app._border.x(), windowSize.y() - 2 * app._border.y()}};
        im::Text("Size: %dx%d", viewport.x(), viewport.y());
        im::Spacing();
        if (im::Checkbox("Keep Aspect Ratio", &app._keepAspectRatio)) {
            fixAspectRatio(app);
        }
        if (im::SliderInt("H. Border", &app._border.x(), 0, windowSize.x() / 2)) {
            fixAspectRatioByX(app);
        }
        if (im::SliderInt("V. Border", &app._border.y(), 0, windowSize.y() / 2)) {
            fixAspectRatioByY(app);
        }
    }

    im::Spacing(); im::Separator(); im::Spacing();
    //if (im::Button("[C]lear")) {
        //app._curve = {};
        //app._curve.solve();
    //}
    if (im::Button("[Q]uit")) {
        sapp_quit();
    }

    im::End();

    {
        // Get current window size.
        //const float ratio = sapp_widthf()/sapp_heightf();

        // Begin recording draw commands for a frame buffer of size (width, height).
        sgp_begin(windowSize.x(), windowSize.y());
        // Set frame buffer drawing region to (0,0,width,height).
        sgp_viewport(0, 0, windowSize.x(), windowSize.y());
        // Set drawing coordinate space to (left=-ratio, right=ratio, top=1, bottom=-1).
        //sgp_project(-ratio, ratio, 1.0f, -1.0f);
        sgp_project(0, static_cast<float>(windowSize.x()), 0, static_cast<float>(windowSize.y()));

        render(app, (*app._selectedResults)[static_cast<size_t>(std::min(app._selectedResult, static_cast<int>(app._selectedResults->size()) - 1))]);

        // Dispatch all draw commands to Sokol GFX.
        sgp_flush();
        // Finish a draw command queue, clearing it.
        sgp_end();
    }

    //im::ShowDemoWindow();
    //im::ShowMetricsWindow();

    simgui_render();
    // End render pass.
    sg_end_pass();
    // Commit Sokol render.
    sg_commit();
    //std::println("Frame took: {}", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - now));
}

AppState sApp = {};

    // new input data:
    // initial speed (0)
    // final speed (0)
    // initial progress (0)
    // final progress (1)
    // for each checkpoint: time, progress
} // namespace

extern "C" sapp_desc sokol_main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    return {
        .user_data = &sApp,
        .init_userdata_cb = init,
        .frame_userdata_cb = frame,
        .cleanup_userdata_cb = cleanup,
        .event_userdata_cb = event,
        .width = 0,
        .height = 0,
        .sample_count = 4, // enable anti-aliasing
        //.swap_interval = 20,
        .fullscreen = false,
        .window_title = "Ease Curve",
        .icon = { .sokol_default = true },
    };
}
