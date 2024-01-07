
export module main.appstate;

import std;
import alx.va;
import main.easecurve;
import sokol.gfx;
import sokol.color;

using namespace sokol::color;

export struct AppState
{
//    sf::RenderWindow _window;
    EaseCurve _curve {};

    struct CurveData
    {
        std::string _name;
        EaseCurve   _saved {};
        EaseCurve   _current {};
        bool        _modified = false;
    };

    std::vector<CurveData>  _curves;

    alx::va::Vec2f          _mouse = {};

    // settings
    sg_color                _windowBg = sg_black;
    sg_color                _axisColor = sg_green;
    sg_color                _guideColor = sg_green;
    sg_color                _curveColor = sg_white;
    sg_color                _speedColor = sg_yellow;
    sg_color                _accelColor = sg_cyan;
    sg_color                _errorColor = sg_red;

    alx::va::Vec2i          _border {{{ 50, 50 }}};
    int                     _selectedCurve = -1;
    bool                    _showCircles = true;
    bool                    _showSpeed = true;
    bool                    _showAccel = true;
    bool                    _showGuides = true;
    bool                    _showPolyLine = true;
    bool                    _keepAspectRatio = true;

    // calculated
};

export void render(const AppState& app);
