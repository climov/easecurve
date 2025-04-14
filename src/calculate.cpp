module;

#include "alx/rassert.h"
#include "alx/trace.h"

module main.appstate;

import alx.assert;
import alx.trig;

namespace {

[[maybe_unused]]
float easeInOutSine(const float t)
{
    return 0.5f * (1.f + std::sin(alx::trig::pi_v<float> * (t - 0.5f)));
}

[[maybe_unused]]
float easeInOutSineDerivative(const float t)
{
    return 0.5f * alx::trig::pi_v<float> * std::cos(alx::trig::pi_v<float> * (t - 0.5f));
}

[[maybe_unused]]
float easeInOutSineIntegral(const float t)
{
    return 0.5f * (t - std::cos(alx::trig::pi_v<float> * (t - .5f)) / alx::trig::pi_v<float>);
}

[[maybe_unused]]
constexpr EaseInOut kEaseInOutSine { &easeInOutSine, &easeInOutSineDerivative, &easeInOutSineIntegral };

[[maybe_unused]]
constexpr float easeInOutLinear(const float t)
{
    return t;
}

[[maybe_unused]]
constexpr float easeInOutLinearDerivative([[maybe_unused]] const float t)
{
    return 1.f;
}

[[maybe_unused]]
constexpr float easeInOutLinearIntegral(const float t)
{
    return .5f * t * t;
}

[[maybe_unused]]
constexpr EaseInOut kEaseInOutLinear { &easeInOutLinear, &easeInOutLinearDerivative, &easeInOutLinearIntegral };

//constexpr EaseInOut kEaseInOut = kEaseInOutLinear;
//constexpr EaseInOut kEaseInOut = kEaseInOutSine;

[[maybe_unused]]
float velocityAt(const Path& path, const Result& result, const float time)
{
    const size_t count = result.velocities.size();
    if (time <= path.startTime) {
        return path.startVelocity;
    }

    float prevStartTime             = path.startTime;
    float prevEaseDuration          = path.adjustedStartEaseDuration;
    float prevVelocity              = path.startVelocity;

    for (size_t k = 0; k < count; ++k) {
        const bool beforeLast               = k < count - 1;
        const float curEaseDuration         = beforeLast ? path.checkpoints[k].adjustedEaseDuration : path.adjustedEndEaseDuration;
        const float curTime                 = beforeLast ? path.checkpoints[k].time - curEaseDuration / 2.f : path.endTime - curEaseDuration;
        const float curVelocity             = result.velocities[k];

        // transition before constant velocity
        if (time < prevStartTime + prevEaseDuration) {
            return std::lerp(prevVelocity, curVelocity, result.easeInOut((time - prevStartTime) / prevEaseDuration));
        }
        // constant velocity
        if (time < curTime) {
            return curVelocity;
        }
        prevStartTime           = curTime;
        prevEaseDuration        = curEaseDuration;
        prevVelocity            = curVelocity;
    }
    if (time < prevStartTime + prevEaseDuration) {
        return std::lerp(prevVelocity, path.endVelocity, result.easeInOut((time - prevStartTime) / prevEaseDuration));
    }

    return path.endVelocity;
}

[[maybe_unused]]
float accelAt(const Path& path, const Result& result, const float time)
{
    const size_t count = result.velocities.size();
    if (time <= path.startTime) {
        return 0;
    }

    float prevStartTime             = path.startTime;
    float prevEaseDuration          = path.adjustedStartEaseDuration;
    float prevVelocity              = path.startVelocity;

    for (size_t k = 0; k < count; ++k) {
        const bool beforeLast               = k < count - 1;
        const float curEaseDuration         = beforeLast ? path.checkpoints[k].adjustedEaseDuration : path.adjustedEndEaseDuration;
        const float curTime                 = beforeLast ? path.checkpoints[k].time - curEaseDuration / 2.f : path.endTime - curEaseDuration;
        const float curVelocity             = result.velocities[k];

        // transition before constant velocity
        if (time < prevStartTime + prevEaseDuration) {
            return result.easeInOut.derivative((time - prevStartTime) / prevEaseDuration) * (curVelocity - prevVelocity) / prevEaseDuration;
        }
        // constant velocity
        if (time < curTime) {
            return 0.f;
        }
        prevStartTime           = curTime;
        prevEaseDuration        = curEaseDuration;
        prevVelocity            = curVelocity;
    }
    if (time < prevStartTime + prevEaseDuration) {
        return result.easeInOut.derivative((time - prevStartTime) / prevEaseDuration) * (path.endVelocity - prevVelocity) / prevEaseDuration;
    }

    return 0.f;
}

// float progressBetween(const Path& path, const float prevTime, const float time, const float step)
// {
//     float curTime = prevTime + step;
//     float result = 0;
//     while (curTime < time) {
//         result += velocityAt(path, curTime) * step;
//         curTime += step;
//     }
//     return result;
// }

void tessellateVelocity(AppState& app, Result& result)
{
    constexpr int linesPerSegment = 1000;
    result.tessellatedVelocity.clear();
    result.tessellatedVelocity.resize(linesPerSegment + 1);
    const float xIncrement = (app._path.endTime - app._path.startTime) / linesPerSegment;
    for (size_t i = 0; i <= linesPerSegment; ++i) {
        const float x = app._path.startTime + i * xIncrement;
        const float y = velocityAt(app._path, result, x);
        result.tessellatedVelocity[i] = {{x, y}};
    }
}

void tessellateProgress(AppState& app, Result& result)
{
    constexpr size_t linesPerSegment = 1000;
    const float xIncrement = (app._path.endTime - app._path.startTime) / linesPerSegment;
    result.tessellatedProgress.clear();
    result.tessellatedProgress.resize(linesPerSegment + 1);
    for (size_t i = 0; i <= linesPerSegment; ++i) {
        const float x = app._path.startTime + i * xIncrement;
        const float y = progressAt(app._path, result, x);
        result.tessellatedProgress[i] = {{x, y}};
    }
}

void tessellateAcceleration(AppState& app, Result& result)
{
    constexpr size_t linesPerSegment = 1000;
    result.tessellatedAccel.clear();
    result.tessellatedAccel.resize(linesPerSegment + 1);
    const float xIncrement = (app._path.endTime - app._path.startTime) / linesPerSegment;
    for (size_t i = 1; i <= linesPerSegment; ++i) {
        const float x = app._path.startTime + i * xIncrement;
        const float y = accelAt(app._path, result, x);
        result.tessellatedAccel[i] = {{x, y}};
    }
}

void adjustEaseDurations(Path& path)
{
    float prevTime                 = path.startTime;
    float prevProgress             = path.startProgress;
    path.adjustedStartEaseDuration  = path.startEaseDuration;
    float* prevEaseDuration        = &path.adjustedStartEaseDuration;
    float prevEaseDurationWeight   = 1;

    for (Checkpoint& checkpoint : path.checkpoints) {
        R_ASSERT(prevTime       < checkpoint.time);
        R_ASSERT(prevProgress   < checkpoint.progress);
        checkpoint.adjustedEaseDuration = checkpoint.easeDuration;
        const float deltaTime = checkpoint.time - prevTime;
        if (deltaTime < *prevEaseDuration * prevEaseDurationWeight + checkpoint.adjustedEaseDuration * .5f) {
            const float totalWeight = prevEaseDurationWeight + 1.f + .5f;
            *prevEaseDuration = deltaTime * prevEaseDurationWeight / totalWeight;
            checkpoint.adjustedEaseDuration = deltaTime * .5f / totalWeight;
        }
        prevTime                = checkpoint.time;
        prevProgress            = checkpoint.progress;
        prevEaseDuration        = &checkpoint.adjustedEaseDuration;
        prevEaseDurationWeight  = .5;
    }
    R_ASSERT(prevTime       < path.endTime);
    R_ASSERT(prevProgress   < path.endProgress);
    path.adjustedEndEaseDuration = path.endEaseDuration;
    const float deltaTime = path.endTime - prevTime;
    if (deltaTime < *prevEaseDuration * prevEaseDurationWeight + path.adjustedEndEaseDuration * 1.f) {
        const float totalWeight = prevEaseDurationWeight + 1.f + 1.f;
        *prevEaseDuration = deltaTime * prevEaseDurationWeight / totalWeight;
        path.adjustedEndEaseDuration = deltaTime * 1.f / totalWeight;
    }
}

void seedInitialVelocities(Path& path, Result& result)
{
    {
        float prevTime         = path.startTime;
        float prevProgress     = path.startProgress;
        float prevEaseDuration = path.adjustedStartEaseDuration;


        for (const Checkpoint& checkpoint : path.checkpoints) {
            R_ASSERT(prevTime < checkpoint.time);
            R_ASSERT(prevProgress < checkpoint.progress);
            R_ASSERT(checkpoint.time - prevTime > prevEaseDuration + checkpoint.adjustedEaseDuration / 2);
            prevTime            = checkpoint.time;
            prevProgress        = checkpoint.progress;
            prevEaseDuration    = checkpoint.adjustedEaseDuration / 2;
        }
        R_ASSERT(prevTime < path.endTime);
        R_ASSERT(prevProgress < path.endProgress);
        R_ASSERT(path.endTime - prevTime > prevEaseDuration + path.adjustedEndEaseDuration);
    }

    const size_t count = path.checkpoints.size() + 1;
    result.velocities.clear();
    result.velocities.resize(count, 0);

    if (count == 1) {
        const float totalProgress  = path.endProgress - path.startProgress - path.startVelocity * path.adjustedStartEaseDuration / 2 - path.endVelocity * path.adjustedEndEaseDuration / 2;
        const float totalTime      = path.endTime - path.startTime - path.adjustedStartEaseDuration / 2 - path.adjustedEndEaseDuration / 2;
        result.velocities[0]       = totalProgress / totalTime;
    } else {
        float prevProgress = path.startProgress;
        float prevTime = path.startTime;
        for (size_t k = 0; k < count - 1; ++k) {
            result.velocities[k]    = (path.checkpoints[k].progress - prevProgress) / (path.checkpoints[k].time - prevTime);
            prevProgress            = path.checkpoints[k].progress;
            prevTime                = path.checkpoints[k].time;
            prevTime                = path.checkpoints[k].time;
        }
        result.velocities[count - 1] = (path.endProgress - prevProgress) / (path.endTime - prevTime);
    }
};

[[maybe_unused]]
float refineVelocities(Path& path, Result& result)
{
    const size_t count = result.velocities.size();

    float sumErrorAbs                       = 0;
    float largestError                      = 0;
    float largestErrorAbs                   = 0;
    size_t largestErrorIndex                = 0;
    float largestErrorSegmentDuration       = 0;

    [[maybe_unused]] float progress = path.startProgress;
    // initial transition before constant velocity
    progress += path.adjustedStartEaseDuration * path.startVelocity + result.easeInOut.antideriv(1.f) * path.adjustedStartEaseDuration * (result.velocities[0] - path.startVelocity);

    float prevStartTime             = path.startTime;
    float prevEaseDuration          = path.adjustedStartEaseDuration;

    for (size_t k = 0; k < count; ++k) {
        const bool beforeLast               = k < count - 1;
        const float curEaseDurationTotal    = beforeLast ? path.checkpoints[k].adjustedEaseDuration : path.adjustedEndEaseDuration;
        const float curEaseDurationFraction = beforeLast ? .5f : 1.f;
        const float curEaseDuration         = curEaseDurationTotal * curEaseDurationFraction;
        const float curVelocity             = result.velocities[k];
        const float nextVelocity            = beforeLast ? result.velocities[k + 1] : path.endVelocity;
        const float checkPointTime          = beforeLast ? path.checkpoints[k].time : path.endTime;
        const float checkPointProgress      = beforeLast ? path.checkpoints[k].progress : path.endProgress;

        // constant velocity
        progress += curVelocity * (checkPointTime - prevStartTime - prevEaseDuration - curEaseDuration);
        // checkpoint
        const float calculatedProgress      = progress + curEaseDuration * curVelocity + result.easeInOut.antideriv(curEaseDurationFraction) * curEaseDurationTotal * (nextVelocity - curVelocity);
        const float progressError           = checkPointProgress - calculatedProgress;
        const float progressErrorAbs        = std::abs(progressError);
        if (progressErrorAbs > largestErrorAbs) {
            largestError                    = progressError;
            largestErrorAbs                 = progressErrorAbs;
            largestErrorIndex               = k;
            largestErrorSegmentDuration     = checkPointTime - prevStartTime;
        }

        sumErrorAbs                         += progressErrorAbs;

        // transition after constant velocity
        progress += curEaseDurationTotal * curVelocity + result.easeInOut.antideriv(1.f) * curEaseDurationTotal * (nextVelocity - curVelocity);

        prevStartTime           = checkPointTime;
        prevEaseDuration        = curEaseDuration;
    }

    result.velocities[largestErrorIndex]      += largestError / largestErrorSegmentDuration;
    result.totalErrorAbs = static_cast<double>(sumErrorAbs);
    return sumErrorAbs;
}

} // namespace

[[maybe_unused]]
float progressAt(const Path& path, const Result& result, const float time)
{
    const size_t count = result.velocities.size();
    float progress = path.startProgress;
    if (time <= path.startTime) {
        return progress;
    }

    float prevStartTime             = path.startTime;
    float prevEaseDuration          = path.adjustedStartEaseDuration;
    float prevVelocity              = path.startVelocity;

    for (size_t k = 0; k < count; ++k) {
        const bool beforeLast               = k < count - 1;
        const float curEaseDuration         = beforeLast ? path.checkpoints[k].adjustedEaseDuration : path.adjustedEndEaseDuration;
        const float curTime                 = beforeLast ? path.checkpoints[k].time - curEaseDuration / 2.f : path.endTime - curEaseDuration;
        const float curVelocity             = result.velocities[k];

        // transition before constant velocity
        if (time < prevStartTime + prevEaseDuration) {
            return progress + (time - prevStartTime) * prevVelocity + result.easeInOut.antideriv((time - prevStartTime) / prevEaseDuration) * prevEaseDuration * (curVelocity - prevVelocity);
        }
        progress += prevEaseDuration * prevVelocity + result.easeInOut.antideriv(1.f) * prevEaseDuration * (curVelocity - prevVelocity);
        // constant velocity
        if (time < curTime) {
            return progress + curVelocity * (time - prevStartTime - prevEaseDuration);
        }
        progress += curVelocity * (curTime - prevStartTime - prevEaseDuration);

        prevStartTime           = curTime;
        prevEaseDuration        = curEaseDuration;
        prevVelocity            = curVelocity;
    }
    if (time < prevStartTime + prevEaseDuration) {
        return progress + (time - prevStartTime) * prevVelocity + result.easeInOut.antideriv((time - prevStartTime) / prevEaseDuration) * prevEaseDuration * (path.endVelocity - prevVelocity);
    }
    progress += prevEaseDuration * prevVelocity + result.easeInOut.antideriv(1.f) * prevEaseDuration * (path.endVelocity - prevVelocity);

    return progress;
}

void solve(AppState& app, std::vector<Result>& results)
{
    results.clear();
    results.emplace_back();
    results.back().easeInOut = (&results == &app._resultsLinear) ? kEaseInOutLinear : kEaseInOutSine;
    results.back().velocities.resize(app._path.checkpoints.size() + 1);

    //std::println("Lowest,Highest,Sum,SumAbs,SumSq,SumPoz,SumNeg,Velocities");
    adjustEaseDurations(app._path);
    seedInitialVelocities(app._path, results.back());
    tessellateVelocity(app, results.back());
    tessellateProgress(app, results.back());
    tessellateAcceleration(app, results.back());
    const auto start = std::chrono::high_resolution_clock::now();
    float prevError = 0;
    if (results.back().velocities.size() > 1) {
        while (true) {
            Result& result = results.emplace_back(results.back());
            const float error = refineVelocities(app._path, result);
            tessellateVelocity(app, result);
            tessellateProgress(app, result);
            tessellateAcceleration(app, result);
            const float errDelta = std::abs(error - prevError);
            //std::println("Error delta: {}", errDelta);
            if (errDelta <= 1e-5f) {
                break;
            }
            prevError = error;
        }
    }
    app._selectedResult = static_cast<int>(results.size()) - 1;
    const auto end = std::chrono::high_resolution_clock::now();
    std::println("Calculation took: {} in {} iterations, with final error of: {}", end-start, results.size(), prevError);
}
