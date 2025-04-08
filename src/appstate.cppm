
module;

export module main.appstate;

import std;

import sokol.gfx;
import sokol.color;

import alx.va;

using namespace sokol::color;
namespace va = alx::va;

export struct Checkpoint
{
    float time                 = 0;
    float progress             = 0;
    float easeDuration         = 0;
    float adjustedEaseDuration = 0; // calculated
};

export struct Path
{
    float                   startTime                   = 0;
    float                   startProgress               = 0;
    float                   startVelocity               = 0;
    float                   startEaseDuration           = 0;
    float                   endTime                     = 0;
    float                   endProgress                 = 0;
    float                   endVelocity                 = 0;
    float                   endEaseDuration             = 0;
    std::vector<Checkpoint> checkpoints                 = {};
    float                   adjustedStartEaseDuration   = 0;    // calculated
    float                   adjustedEndEaseDuration     = 0;    // calculated
    std::vector<float>      velocities                  = {};   // calculated
};

export struct AppState
{
//    sf::RenderWindow _window;
    //EaseCurve _curve {};

    //struct CurveData
    //{
    //    std::string _name;
    //    EaseCurve   _saved {};
    //    EaseCurve   _current {};
    //    bool        _modified = false;
    //};
    //std::vector<CurveData>  _curves;

    Path                    _path               = {};

    va::Vec2f               _mouse              = {};

    // settings
    sg_color                _windowBg           = sg_black;
    sg_color                _axisColor          = sg_green;
    sg_color                _guideColor         = sg_green;
    sg_color                _curveColor         = sg_white;
    sg_color                _speedColor         = sg_yellow;
    sg_color                _accelColor         = sg_cyan;
    sg_color                _errorColor         = sg_red;

    va::Vec2i               _border             = {{{ 50, 50 }}};
    int                     _selectedCurve      = -1;
    bool                    _showCircles        = true;
    bool                    _showSpeed          = true;
    bool                    _showAccel          = true;
    bool                    _showGuides         = true;
    bool                    _showPolyLine       = true;
    bool                    _keepAspectRatio    = false;

    // calculated
    std::vector<va::Vec2f>  tessellatedVelocity = {};
    std::vector<va::Vec2f>  tessellatedProgress = {};
    std::vector<va::Vec2f>  tessellatedAccel    = {};
};

export void solve(AppState& app);
export void render(const AppState& app);
