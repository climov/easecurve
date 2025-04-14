
module;

export module main.appstate;

import std;

import sokol.gfx;
import sokol.color;

import alx.va;

using namespace sokol::color;
namespace va = alx::va;

using FloatToFloatFunc = float (*)(float);
struct EaseInOut
{
    FloatToFloatFunc func;
    FloatToFloatFunc derivative;
    FloatToFloatFunc antideriv;

    constexpr float operator()(const float x) const noexcept { return func(x); }
};

export struct Checkpoint
{
    float time                 = 0;
    float progress             = 0;
    float easeDuration         = 0;
    float adjustedEaseDuration = 0; // calculated
};

export struct Result
{
    EaseInOut               easeInOut;
    std::vector<float>      velocities          = {};
    std::vector<va::Vec2f>  tessellatedVelocity = {};
    std::vector<va::Vec2f>  tessellatedProgress = {};
    std::vector<va::Vec2f>  tessellatedAccel    = {};
    double                  totalErrorAbs       = 0.;
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
    std::vector<Result>     _resultsLinear      = {};
    std::vector<Result>     _resultsSine        = {};
    std::vector<Result>*    _selectedResults    = &_resultsSine;

    va::Vec2f               _mouse              = {};

    // settings
    sg_color                _windowBg           = sg_black;
    sg_color                _axisColor          = sg_white;
    sg_color                _guideColor         = sg_white;
    sg_color                _curveColor         = sg_green; //sg_magenta; //sg_white;
    sg_color                _speedColor         = sg_color_lerp(_curveColor, sg_white, .70f); // sg_yellow;
    sg_color                _accelColor         = sg_color_lerp(_curveColor, sg_black, .25f); //sg_cyan;
    sg_color                _errorColor         = sg_red;

    va::Vec2i               _border             = {{{ 50, 50 }}};
    int                     _selectedResult     = 0;
    int                     _selectedCurve      = -1;
    //bool                    _showCircles        = true;
    bool                    _useSineEasing      = true;
    bool                    _showSpeed          = true;
    bool                    _showAccel          = true;
    bool                    _showGuides         = true;
    bool                    _showPolyLine       = false;
    bool                    _keepAspectRatio    = false;
};

export void solve(AppState& app, std::vector<Result>& results);
export void render(const AppState& app, const Result& result);
float progressAt(const Path& path, const Result& result, const float time);
