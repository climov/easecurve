module;

#include "alx/rassert.h"

module main.appstate;

import alx.assert;
import alx.trig;

namespace {

[[maybe_unused]]
float easeInOutCubic(const float t)
{
    if (t < 0.5f) {
        return 4.f * t * t * t;
    }
    const float v = -2.f * t + 2.f;
    return 1.f - (v * v * v) / 2.f;
}

[[maybe_unused]]
float easeInOutQuadratic(const float t)
{
    if (t < 0.5f) {
        return 2.f * t * t;
    }
    const float v = -2.f * t + 2;
    return 1 - v * v / 2.f;
}

[[maybe_unused]]
float easeInOutCircle(const float t)
{
    return t < 0.5f ? (1.f - std::sqrt(1 - std::pow(2.f * t, 2.f))) / 2.f : (std::sqrt(1.f - std::pow(-2.f * t + 2.f, 2.f)) + 1.f) / 2.f;
}

[[maybe_unused]]
float easeInOutSine(const float t) {
    return 0.5f * (1.f + std::sin(alx::trig::pi_v<float> * (t - 0.5f)));
}

[[maybe_unused]]
float easeInOutLinear(const float t)
{
    return t;
}

float easeInOut(const float t, [[maybe_unused]] const float factor)
{
    const float absFactor = std::abs(factor);
    const float delta = (factor > 0.f && factor < 1.f) ? 0.5f : 0.f;
    return (easeInOutSine(t * absFactor + delta) - delta) / absFactor;
}

float velocityAt(const Path& path, const float time)
{
    if (time <= path.startTime) {
        return path.startVelocity;
    }
    float prevStartTime             = path.startTime;
    float prevEaseDuration          = path.adjustedStartEaseDuration;
    float prevEaseDurationFactor    = 1.f;
    float prevVelocity              = path.startVelocity;

    if (!path.checkpoints.empty()) {
        for (size_t k = 0; k < path.checkpoints.size(); ++k) {
            const float curTime                 = path.checkpoints[k].time;
            const float curEaseDuration         = path.checkpoints[k].adjustedEaseDuration;
            const float curEaseDurationFactor   = .5f;
            const float curVelocity             = path.velocities[k];
            const float nextVelocity            = path.velocities[k+1];

            if (time < prevStartTime + prevEaseDuration * prevEaseDurationFactor) {
                return std::lerp(prevVelocity, curVelocity, easeInOut((time - prevStartTime) / (prevEaseDuration * prevEaseDurationFactor), prevEaseDurationFactor));
            }
            if (time < curTime - curEaseDuration * curEaseDurationFactor) {
                return curVelocity;
            }
            if (time < curTime) {
                return std::lerp(curVelocity, (curVelocity + nextVelocity) / 2, easeInOut((time - curTime + curEaseDuration * curEaseDurationFactor) / (curEaseDuration * curEaseDurationFactor), -curEaseDurationFactor));
            }
            prevStartTime           = curTime;
            prevEaseDuration        = curEaseDuration;
            prevEaseDurationFactor  = curEaseDurationFactor;
            prevVelocity            = (curVelocity + nextVelocity) / 2;
        }
    }
    {
        const size_t k                      = path.checkpoints.size();
        const float curTime                 = path.endTime;
        const float curEaseDuration         = path.adjustedEndEaseDuration;
        const float curEaseDurationFactor   = 1.f;
        const float curVelocity             = path.velocities[k];
        const float nextVelocity            = path.endVelocity;

        if (time < prevStartTime + prevEaseDuration * prevEaseDurationFactor) {
            return std::lerp(prevVelocity, curVelocity, easeInOut((time - prevStartTime) / (prevEaseDuration * prevEaseDurationFactor), prevEaseDurationFactor));
        }
        if (time < curTime - curEaseDuration) {
            return curVelocity;
        }
        if (time < curTime) {
            return std::lerp(curVelocity, nextVelocity, easeInOut((time - curTime + curEaseDuration * curEaseDurationFactor) / (curEaseDuration * curEaseDurationFactor), -curEaseDurationFactor));
        }
    }

    return path.endVelocity;
}

float progressBetween(const Path& path, const float prevTime, const float time, const float step)
{
    float curTime = prevTime + step;
    float result = 0;
    while (curTime < time) {
        result += velocityAt(path, curTime) * step;
        curTime += step;
    }
    return result;
}

float accelAt(const Path& path, const float time)
{
    constexpr float deltaX = 1e-5f;
    return (velocityAt(path, time + deltaX / 2) - velocityAt(path, time - deltaX / 2)) / deltaX;
}

void tessellateVelocity(AppState& app)
{
    constexpr int linesPerSegment = 1000;
    app.tessellatedVelocity.clear();
    app.tessellatedVelocity.resize(linesPerSegment + 1);
    const float xIncrement = (app._path.endTime - app._path.startTime) / linesPerSegment;
    for (size_t i = 0; i <= linesPerSegment; ++i) {
        const float x = app._path.startTime + i * xIncrement;
        const float y = velocityAt(app._path, x);
        app.tessellatedVelocity[i] = {{x, y}};
    }
}

void tessellateProgress(AppState& app)
{
    constexpr size_t linesPerSegment = 1000;
    app.tessellatedProgress.clear();
    app.tessellatedProgress.resize(linesPerSegment + 1);
    const float xIncrement = (app._path.endTime - app._path.startTime) / linesPerSegment;
    const float xStep = xIncrement / 100;
    float prevX = app._path.startTime;
    float prevY = 0;
    for (size_t i = 0; i <= linesPerSegment; ++i) {
        const float x = app._path.startTime + i * xIncrement;
        const float y = prevY + progressBetween(app._path, prevX, x, xStep);
        app.tessellatedProgress[i] = {{x, y}};
        prevX = x;
        prevY = y;
    }
}

void tessellateAcceleration(AppState& app)
{
    constexpr size_t linesPerSegment = 1000;
    app.tessellatedAccel.clear();
    app.tessellatedAccel.resize(linesPerSegment + 1);
    const float xIncrement = (app._path.endTime - app._path.startTime) / linesPerSegment;
    for (size_t i = 1; i <= linesPerSegment; ++i) {
        const float x = app._path.startTime + i * xIncrement;
        const float y = accelAt(app._path, x);
        app.tessellatedAccel[i] = {{x, y}};
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

void seedInitialVelocities(Path& path)
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
    path.velocities.clear();
    path.velocities.resize(count, 0);

    if (count == 1) {
        const float totalProgress  = path.endProgress - path.startProgress - path.startVelocity * path.adjustedStartEaseDuration / 2 - path.endVelocity * path.adjustedEndEaseDuration / 2;
        const float totalTime      = path.endTime - path.startTime - path.adjustedStartEaseDuration / 2 - path.adjustedEndEaseDuration / 2;
        path.velocities[0]          = totalProgress / totalTime;
    } else {
        float prevProgress = path.startProgress;
        float prevTime = path.startTime;
        for (size_t k = 0; k < count - 1; ++k) {
            path.velocities[k]  = (path.checkpoints[k].progress - prevProgress) / (path.checkpoints[k].time - prevTime);
            prevProgress        = path.checkpoints[k].progress;
            prevTime            = path.checkpoints[k].time;
            prevTime            = path.checkpoints[k].time;
        }
        path.velocities[count - 1] = (path.endProgress - prevProgress) / (path.endTime - prevTime);
    }
};

float refineVelocities(Path& path)
{
    const size_t count = path.velocities.size();
    //R_ASSERT(count > 1);

    float sumErrorAbs                       = 0;
    float largestError                      = 0;
    float largestErrorAbs                   = 0;
    size_t largestErrorIndex                = 0;
    float largestErrorSegmentDuration       = 0;

    float prevTime                          = path.startTime;
    float prevProgress                      = path.startProgress;
    float prevEaseDuration                  = path.adjustedStartEaseDuration;
    float prevVelocity                      = path.startVelocity;

    // first
    {
        const float segmentDuration             = path.checkpoints[0].time - prevTime;
        const float segmentProgress             = path.checkpoints[0].progress - prevProgress;
        const float prevVelocityDuration        = prevEaseDuration / 2;
        const float nextVelocityDuration        = path.checkpoints[0].adjustedEaseDuration / 8;
        const float currentVelocityDuration     = segmentDuration - prevVelocityDuration - nextVelocityDuration;
        const float prevVelocityProgress        = prevVelocity  * prevVelocityDuration;
        const float currentVelocityProgress     = path.velocities[0] * currentVelocityDuration;
        const float nextVelocityProgress        = path.velocities[1] * nextVelocityDuration;
        const float calculatedProgress          = prevVelocityProgress + currentVelocityProgress + nextVelocityProgress;
        const float progressError               = segmentProgress - calculatedProgress;
        const float progressErrorAbs            = std::abs(progressError);
        largestError                            = progressError;
        largestErrorAbs                         = progressErrorAbs;
        largestErrorSegmentDuration             = segmentDuration;

        sumErrorAbs                             += progressErrorAbs;

        prevTime                                = path.checkpoints[0].time;
        prevProgress                            = path.checkpoints[0].progress;
        prevEaseDuration                        = path.checkpoints[0].adjustedEaseDuration;
        prevVelocity                            = path.velocities[0];
    }

    // middle
    for (size_t k = 1; k < count - 1; ++k) {
        const float segmentDuration             = path.checkpoints[k].time - prevTime;
        const float segmentProgress             = path.checkpoints[k].progress - prevProgress;
        const float prevVelocityDuration        = prevEaseDuration / 8;
        const float nextVelocityDuration        = path.checkpoints[k].adjustedEaseDuration / 8;
        const float currentVelocityDuration     = segmentDuration - prevVelocityDuration - nextVelocityDuration;
        const float prevVelocityProgress        = prevVelocity    * prevVelocityDuration;
        const float currentVelocityProgress     = path.velocities[k]   * currentVelocityDuration;
        const float nextVelocityProgress        = path.velocities[k+1] * nextVelocityDuration;
        const float calculatedProgress          = prevVelocityProgress + currentVelocityProgress + nextVelocityProgress;
        const float progressError               = segmentProgress - calculatedProgress;
        const float progressErrorAbs            = std::abs(progressError);
        if (progressErrorAbs > largestErrorAbs) {
            largestError                        = progressError;
            largestErrorAbs                     = progressErrorAbs;
            largestErrorIndex                   = k;
            largestErrorSegmentDuration         = segmentDuration;
        }

        sumErrorAbs                             += progressErrorAbs;

        prevTime                                = path.checkpoints[k].time;
        prevProgress                            = path.checkpoints[k].progress;
        prevEaseDuration                        = path.checkpoints[k].adjustedEaseDuration;
        prevVelocity                            = path.velocities[k];
    }

    // last
    {
        const size_t k                          = count - 1;
        const float segmentDuration             = path.endTime - prevTime;
        const float segmentProgress             = path.endProgress - prevProgress;
        const float prevVelocityDuration        = prevEaseDuration / 8;
        const float nextVelocityDuration        = path.adjustedEndEaseDuration / 2;
        const float currentVelocityDuration     = segmentDuration - prevVelocityDuration - nextVelocityDuration;
        const float prevVelocityProgress        = prevVelocity      * prevVelocityDuration;
        const float currentVelocityProgress     = path.velocities[k]     * currentVelocityDuration;
        const float nextVelocityProgress        = path.endVelocity * nextVelocityDuration;
        const float calculatedProgress          = prevVelocityProgress + currentVelocityProgress + nextVelocityProgress;
        const float progressError               = segmentProgress - calculatedProgress;
        const float progressErrorAbs            = std::abs(progressError);
        if (progressErrorAbs > largestErrorAbs) {
            largestError                        = progressError;
            largestErrorAbs                     = progressErrorAbs;
            largestErrorIndex                   = k;
            largestErrorSegmentDuration         = segmentDuration;
        }

        sumErrorAbs                             += progressErrorAbs;
    }

    // std::print("{},{},{},{},{},{},{},", lowestErrorAbs, largestErrorAbs, sumError, sumErrorAbs, sumErrorSq, sumErrorPoz, sumErrorNeg);
    // for (const Checkpoint& checkpoint : path.checkpoints) {
    //     std::print("{},", checkpoint.error);
    // }
    // for (const float velocity: velocities) {
    //     std::print("{},", velocity);
    // }
    // std::println("");
    path.velocities[largestErrorIndex]          += largestError / largestErrorSegmentDuration;
    return sumErrorAbs;
};

} // namespace

void solve(AppState& app)
{
    app._path.velocities.resize(app._path.checkpoints.size() + 1);

    //std::println("Lowest,Highest,Sum,SumAbs,SumSq,SumPoz,SumNeg,Velocities");
    adjustEaseDurations(app._path);
    seedInitialVelocities(app._path);
    const auto start = std::chrono::high_resolution_clock::now();
    int i = 0;
    float prevError = 0;
    if (app._path.velocities.size() > 1) {
        while (true) {
            ++i;
            const float error = refineVelocities(app._path);
            const float errDelta = std::abs(error - prevError);
            //std::println("Error delta: {}", errDelta);
            if (errDelta <= 1e-5f) {
                break;
            }
            prevError = error;
        }
    }
    tessellateVelocity(app);
    tessellateProgress(app);
    tessellateAcceleration(app);
    const auto end = std::chrono::high_resolution_clock::now();
    std::println("Calculation took: {} in {} iterations, with final error of: {}", end-start, i, prevError);
}
