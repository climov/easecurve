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

    float prevStartTime     = path.startTime;
    float prevEaseDuration  = path.adjustedStartEaseDuration;
    float prevVelocity      = path.startVelocity;

    for (size_t k = 0; k < count; ++k) {
        const bool beforeLast       = k < count - 1;
        const float curEaseDuration = beforeLast ? path.checkpoints[k].adjustedEaseDuration : path.adjustedEndEaseDuration;
        const float curTime         = beforeLast ? path.checkpoints[k].time - curEaseDuration / 2.f : path.endTime - curEaseDuration;
        const float curVelocity     = result.velocities[k];

        // transition before constant velocity
        if (time < prevStartTime + prevEaseDuration) {
            return result.easeInOut.derivative((time - prevStartTime) / prevEaseDuration) * (curVelocity - prevVelocity) / prevEaseDuration;
        }
        // constant velocity
        if (time < curTime) {
            return 0.f;
        }
        prevStartTime       = curTime;
        prevEaseDuration    = curEaseDuration;
        prevVelocity        = curVelocity;
    }
    if (time < prevStartTime + prevEaseDuration) {
        return result.easeInOut.derivative((time - prevStartTime) / prevEaseDuration) * (path.endVelocity - prevVelocity) / prevEaseDuration;
    }

    return 0.f;
}

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

void seedInitialVelocities(Path& path, Result& result)
{
    {
        float prevTime         = path.startTime;
        float prevProgress     = path.startProgress;
        float prevEaseDuration = path.adjustedStartEaseDuration;


        for (const Checkpoint& checkpoint : path.checkpoints) {
            R_ASSERT(prevTime < checkpoint.time);
            R_ASSERT(prevProgress < checkpoint.progress);
            R_ASSERT(checkpoint.time - prevTime >= prevEaseDuration + checkpoint.adjustedEaseDuration / 2);
            prevTime            = checkpoint.time;
            prevProgress        = checkpoint.progress;
            prevEaseDuration    = checkpoint.adjustedEaseDuration / 2;
        }
        R_ASSERT(prevTime < path.endTime);
        R_ASSERT(prevProgress < path.endProgress);
        R_ASSERT(path.endTime - prevTime >= prevEaseDuration + path.adjustedEndEaseDuration);
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

    float progress = path.startProgress;
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

    float prevStartTime     = path.startTime;
    float prevEaseDuration  = path.adjustedStartEaseDuration;
    float prevVelocity      = path.startVelocity;

    for (size_t k = 0; k < count; ++k) {
        const bool beforeLast       = k < count - 1;
        const float curEaseDuration = beforeLast ? path.checkpoints[k].adjustedEaseDuration : path.adjustedEndEaseDuration;
        const float curTime         = beforeLast ? path.checkpoints[k].time - curEaseDuration / 2.f : path.endTime - curEaseDuration;
        const float curVelocity     = result.velocities[k];

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

        prevStartTime       = curTime;
        prevEaseDuration    = curEaseDuration;
        prevVelocity        = curVelocity;
    }
    // Transition to end velocity
    if (time < prevStartTime + prevEaseDuration) {
        return progress + (time - prevStartTime) * prevVelocity + result.easeInOut.antideriv((time - prevStartTime) / prevEaseDuration) * prevEaseDuration * (path.endVelocity - prevVelocity);
    }
    progress += prevEaseDuration * prevVelocity + result.easeInOut.antideriv(1.f) * prevEaseDuration * (path.endVelocity - prevVelocity);

    return progress;
}

void adjustEaseDurations1(Path& path)
{
    constexpr float kEasingGuard = .9999f;
    // Optimizing algorithm that tries to balance the easings of checkpoints so that they don't
    // overlap.
    path.adjustedStartEaseDuration = path.startEaseDuration;
    path.adjustedEndEaseDuration = path.endEaseDuration;
    for (Checkpoint& checkpoint : path.checkpoints) {
        checkpoint.adjustedEaseDuration = checkpoint.easeDuration;
    }

    if (path.checkpoints.empty()) {
        // If we have no checkpoints, the start and end easings should be scaled down to fit within
        // the available run duration.
        const float totalCurrentEasing = path.startEaseDuration + path.endEaseDuration;
        const float runDuration = path.endTime - path.startTime;
        if (totalCurrentEasing > runDuration) {
            const float modifiedDuration = runDuration * kEasingGuard;
            const float scaleFactor = modifiedDuration / totalCurrentEasing;
            path.adjustedStartEaseDuration *= scaleFactor;
            path.adjustedEndEaseDuration *= scaleFactor;
        }
    } else {
        const size_t maxRounds = path.checkpoints.size() + 2;
        size_t round = 0;
        for (; round < maxRounds; ++round) {
            // The overlap, if any, will yield a fraction > 1;
            float minOverlapFraction = std::numeric_limits<float>::max();

            float prevTime = path.startTime;
            float prevEase = path.adjustedStartEaseDuration;
            for (size_t i = 0; i < path.checkpoints.size(); ++i) {
                const float currentTime             = path.checkpoints[i].time;
                const float currentDuration         = currentTime - prevTime;
                const float currentEase             = path.checkpoints[i].adjustedEaseDuration / 2.f;
                const float currentOverlapFraction  = (prevEase + currentEase) / currentDuration;

                if (currentOverlapFraction > 1.f && currentOverlapFraction < minOverlapFraction) {
                    minOverlapFraction = currentOverlapFraction;
                }

                prevTime = currentTime;
                prevEase = currentEase;
            }
            // End point
            {
                const float currentTime             = path.endTime;
                const float currentDuration         = currentTime - prevTime;
                const float currentEase             = path.adjustedEndEaseDuration;
                const float currentOverlapFraction  = (prevEase + currentEase) / currentDuration;

                if (currentOverlapFraction > 1.f && currentOverlapFraction < minOverlapFraction) {
                    minOverlapFraction = currentOverlapFraction;
                }
            }

            if (minOverlapFraction >= std::numeric_limits<float>::max()) {
                // No overlap found, bail out
                break;
            }

            // Found at least one overlap, reduce all ease durations by a fraction
            // that guarantees that all overlaps equal to MinOverlap become non-overlaps
            prevTime = path.startTime;
            prevEase = path.adjustedStartEaseDuration;
            // Start point
            {
                constexpr size_t i                  = 0;
                const float currentTime             = path.checkpoints[i].time;
                const float currentDuration         = currentTime - prevTime;
                const float currentEase             = path.checkpoints[i].adjustedEaseDuration / 2.f;
                const float currentOverlapFraction  = (prevEase + currentEase) / currentDuration;

                if (currentOverlapFraction > 1.f) {
                    path.adjustedStartEaseDuration *= kEasingGuard / currentOverlapFraction;
                }
            }
            // Checkpoints
            for (size_t i = 0; i < path.checkpoints.size(); ++i) {
                const float currentTime             = path.checkpoints[i].time;
                const float leftDuration            = currentTime - prevTime;
                const float currentEase             = path.checkpoints[i].adjustedEaseDuration / 2.f;
                const float leftOverlapFraction     = (prevEase + currentEase) / leftDuration;

                const float nextTime                = i == path.checkpoints.size() - 1 ? path.endTime : path.checkpoints[i + 1].time;
                const float rightDuration           = nextTime - currentTime;
                const float nextEase                = i == path.checkpoints.size() - 1 ? path.adjustedEndEaseDuration : path.checkpoints[i + 1].adjustedEaseDuration / 2.f;
                const float rightOverlapFraction    = (currentEase + nextEase) / rightDuration;

                float smallestOverlapFraction = std::max(1.f, leftOverlapFraction);
                if (rightOverlapFraction > 1.f && (smallestOverlapFraction <= 1.f || rightOverlapFraction < smallestOverlapFraction)) {
                    smallestOverlapFraction = rightOverlapFraction;
                }
                if (smallestOverlapFraction > 1.f) {
                    path.checkpoints[i].adjustedEaseDuration *= kEasingGuard / smallestOverlapFraction;
                }

                prevTime = currentTime;
                prevEase = currentEase;
            }
            // End point
            {
                const float currentTime             = path.endTime;
                const float currentDuration         = currentTime - prevTime;
                const float currentEase             = path.adjustedEndEaseDuration;
                const float currentOverlapFraction  = (prevEase + currentEase) / currentDuration;

                if (currentOverlapFraction > 1.f) {
                    path.adjustedEndEaseDuration *= kEasingGuard / currentOverlapFraction;
                }
            }
        }
    }
}

void adjustEaseDurations2(Path& path)
{
    constexpr float kEasingGuard = .9999f;
    // Optimizing algorithm that tries to balance the easings of checkpoints so that they don't
    // overlap.
    path.adjustedStartEaseDuration = path.startEaseDuration;
    path.adjustedEndEaseDuration = path.endEaseDuration;
    for (Checkpoint& checkpoint : path.checkpoints) {
        checkpoint.adjustedEaseDuration = checkpoint.easeDuration;
    }

    if (path.checkpoints.empty()) {
        // If we have no checkpoints, the start and end easings should be scaled down to fit within
        // the available run duration.
        const float totalCurrentEasing = path.startEaseDuration + path.endEaseDuration;
        const float runDuration = path.endTime - path.startTime;
        if (totalCurrentEasing > runDuration) {
            const float modifiedDuration = runDuration * kEasingGuard;
            const float scaleFactor = modifiedDuration / totalCurrentEasing;
            path.adjustedStartEaseDuration *= scaleFactor;
            path.adjustedEndEaseDuration *= scaleFactor;
        }
    } else {
        const size_t maxRounds = path.checkpoints.size() + 2;
        size_t round = 0;
        for (; round < maxRounds; ++round) {
            // The overlap, if any, will yield a fraction > 1;
            //float minOverlapFraction = std::numeric_limits<float>::max();
            float minOverlapFraction = 0.f;
            float minOverlap = 0.f;
            size_t maxOverlapIndex = 0;

            float prevTime = path.startTime;
            float prevEase = path.adjustedStartEaseDuration;
            for (size_t i = 0; i < path.checkpoints.size(); ++i) {
                const float currentTime             = path.checkpoints[i].time;
                const float currentDuration         = currentTime - prevTime;
                const float currentEase             = path.checkpoints[i].adjustedEaseDuration / 2.f;
                const float currentOverlap          = prevEase + currentEase - currentDuration;
                const float currentOverlapFraction  = (prevEase + currentEase + currentOverlap) / (prevEase + currentEase);

                if (currentOverlapFraction > 1.f && currentOverlap > minOverlap) {
                    minOverlapFraction = currentOverlapFraction;
                    minOverlap = currentOverlap;
                    maxOverlapIndex = i;
                }

                prevTime = currentTime;
                prevEase = currentEase;
            }
            // End point
            {
                const float currentTime             = path.endTime;
                const float currentDuration         = currentTime - prevTime;
                const float currentEase             = path.adjustedEndEaseDuration;
                const float currentOverlap          = prevEase + currentEase - currentDuration;
                const float currentOverlapFraction  = (prevEase + currentEase + currentOverlap) / (prevEase + currentEase);

                if (currentOverlapFraction > 1.f && currentOverlap > minOverlap) {
                    minOverlapFraction = currentOverlapFraction;
                    minOverlap = currentOverlap;
                    maxOverlapIndex = path.checkpoints.size();
                }
            }

            //fdbg("Max overlap: {}", minOverlapFraction);
            //if (minOverlapFraction >= std::numeric_limits<float>::max()) {
            if (minOverlapFraction < 1.f) {
                // No overlap found, bail out
                break;
            }

            // Found at least one overlap, reduce all ease durations by a fraction
            // that guarantees that all overlaps equal to MinOverlap become non-overlaps
            prevTime = path.startTime;
            prevEase = path.adjustedStartEaseDuration;
            // Start point
            {
                constexpr size_t i                  = 0;
                const float currentTime             = path.checkpoints[i].time;
                const float currentDuration         = currentTime - prevTime;
                const float currentEase             = path.checkpoints[i].adjustedEaseDuration / 2.f;
                const float currentOverlapFraction  = (prevEase + currentEase) / currentDuration;

                //if (currentOverlapFraction == minOverlapFraction) {
                if (maxOverlapIndex == 0) {
                    path.adjustedStartEaseDuration *= kEasingGuard / currentOverlapFraction;
                }
            }
            // Checkpoints
            for (size_t i = 0; i < path.checkpoints.size(); ++i) {
                const float currentTime             = path.checkpoints[i].time;
                const float leftDuration            = currentTime - prevTime;
                const float currentEase             = path.checkpoints[i].adjustedEaseDuration / 2.f;
                const float leftOverlapFraction     = (prevEase + currentEase) / leftDuration;

                const float nextTime                = i == path.checkpoints.size() - 1 ? path.endTime : path.checkpoints[i + 1].time;
                const float rightDuration           = nextTime - currentTime;
                const float nextEase                = i == path.checkpoints.size() - 1 ? path.adjustedEndEaseDuration : path.checkpoints[i + 1].adjustedEaseDuration / 2.f;
                const float rightOverlapFraction    = (currentEase + nextEase) / rightDuration;

                float smallestOverlapFraction = std::max(1.f, leftOverlapFraction);
                if (rightOverlapFraction > 1.f && (smallestOverlapFraction <= 1.f || rightOverlapFraction > smallestOverlapFraction)) {
                    smallestOverlapFraction = rightOverlapFraction;
                }
                //if (smallestOverlapFraction == minOverlapFraction) {
                if (maxOverlapIndex == i || maxOverlapIndex - 1 == i) {
                    path.checkpoints[i].adjustedEaseDuration *= kEasingGuard / smallestOverlapFraction;
                }

                prevTime = currentTime;
                prevEase = currentEase;
            }
            // End point
            {
                const float currentTime             = path.endTime;
                const float currentDuration         = currentTime - prevTime;
                const float currentEase             = path.adjustedEndEaseDuration;
                const float currentOverlapFraction  = (prevEase + currentEase) / currentDuration;

                //if (currentOverlapFraction == minOverlapFraction) {
                if (maxOverlapIndex == path.checkpoints.size()) {
                    path.adjustedEndEaseDuration *= kEasingGuard / currentOverlapFraction;
                }
            }
        }
    }
}

[[maybe_unused]]
void adjustEaseDurationsP(Path& path)
{
    // Optimizing algorithm that tries to balance the easings of checkpoints so that they don't
    // overlap. We iterate front to back, and find the easing necessary for each checkpoint to fit
    // hard between its neighboring checkpoints without taking into account whether the neighbors
    // could change. Then we lerp the checkpoints towards this, monitoring the size of the change
    // actually made (i.e. the error). When all checkpoints' easing moves less than a certain
    // threshold, the algorithm has completed.

    constexpr float convergenceSpeed = 0.1f;
    constexpr float errorTarget = 0.0001f;
    constexpr size_t maxRounds = 250;

    std::vector<float> newEasings;
    newEasings.resize(path.checkpoints.size(), 0.f);

    path.adjustedStartEaseDuration = path.startEaseDuration;
    path.adjustedEndEaseDuration = path.endEaseDuration;
    for (Checkpoint& checkpoint : path.checkpoints) {
        checkpoint.adjustedEaseDuration = checkpoint.easeDuration;
    }
    // First do auto-easing on checkpoints. We want to preserve start and end easing for as long as
    // possible, because constrained settings are more likely to be creatively noticeable.

    // SDHIGHLIGHT("Auto easing");
    // for (const auto& Checkpoint : Checkpoints) {
    //     SDBUFFER("%@", Checkpoint.EaseDuration);
    // }
    // SDFLUSH();

    if (!path.checkpoints.empty()) {
        size_t round = 0;

        while (round < maxRounds) {
            ++round;
            float maxRoundError = 0;

            for (size_t index = 0; index < path.checkpoints.size(); ++index) {
                const float leftTime            = (index == 0) ? path.startTime : path.checkpoints[index - 1].time;
                const float leftEase            = (index == 0) ? path.adjustedStartEaseDuration : path.checkpoints[index - 1].adjustedEaseDuration / 2;
                const float leftVacant          = path.checkpoints[index].time - leftTime - leftEase;

                const float rightTime           = (index == path.checkpoints.size() - 1) ? path.endTime : path.checkpoints[index + 1].time;
                const float rightEase           = (index == path.checkpoints.size() - 1) ? path.adjustedEndEaseDuration : path.checkpoints[index + 1].adjustedEaseDuration / 2;
                const float rightVacant         = rightTime - path.checkpoints[index].time - rightEase;

                // Constrain the easing to the available space both on the left and the right side, and
                // then in totality. The total is then multiplied by 2 again to get the new, fully
                // constrained checkpoint ease.
                const float halfCurrentEase     = path.checkpoints[index].adjustedEaseDuration / 2;
                const float leftAllowedEase     = std::min(halfCurrentEase, leftVacant);
                const float rightAllowedEase    = std::min(halfCurrentEase, rightVacant);
                const float totalAllowedEase    = 2 * std::min(leftAllowedEase, rightAllowedEase);

                // Lerp towards the fully constrained value. On the very last round, if we still haven't
                // stabilized, we have to constrain all the way (lerp alpha = 1), or we would trigger an
                // easing assert once the constant velocity solver runs.
                const bool lastRound = (round == maxRounds - 1);
                newEasings[index] = std::lerp(path.checkpoints[index].adjustedEaseDuration, totalAllowedEase, lastRound ? 1.0f : convergenceSpeed);

                const float error = std::abs(path.checkpoints[index].adjustedEaseDuration - totalAllowedEase);
                maxRoundError = std::max(maxRoundError, error);
            }

            if (maxRoundError < errorTarget) {
                break;
            }

            // Copy the newly created easings as the starting point for the next round.
            for (size_t index = 0; index < path.checkpoints.size(); ++index) {
                path.checkpoints[index].adjustedEaseDuration = newEasings[index];
            }
        }

        // Subtract the error target from all checkpoints easings, to ensure that no decimal error can
        // trigger an easing assert in the constant velocity solver.
        for (auto& checkpoint : path.checkpoints) {
            checkpoint.adjustedEaseDuration = std::max(0.f, checkpoint.adjustedEaseDuration - errorTarget);
            //SDBUFFER("%@", Checkpoint.EaseDuration);
        }
        //SDBUFFER("Solved in %@ rounds", round);
        //SDFLUSH();
    }

    // Now truncate start/end easings.

    if (path.checkpoints.empty()) {
        // If we have no checkpoints, the start and end easings should be scaled down to fit within
        // the available run duration.
        const float totalCurrentEasing = path.adjustedStartEaseDuration + path.adjustedEndEaseDuration;
        const float runDuration = path.endTime - path.startTime;
        if (totalCurrentEasing > runDuration) {
            const float modifiedDuration = runDuration * 0.9999f;
            const float scaleFactor = modifiedDuration / totalCurrentEasing;
            path.adjustedStartEaseDuration *= scaleFactor;
            path.adjustedEndEaseDuration *= scaleFactor;
        }
    } else {
        // If we have checkpoints, truncate the start/end easing against the time available after
        // shrinking the checkpoint easings.
        path.adjustedStartEaseDuration = std::min(path.adjustedStartEaseDuration, path.checkpoints[0].time - path.startTime - path.checkpoints[0].adjustedEaseDuration / 2);
        path.adjustedStartEaseDuration = std::min(path.adjustedEndEaseDuration, path.endTime - path.checkpoints[path.checkpoints.size() - 1].time - path.checkpoints[path.checkpoints.size() - 1].adjustedEaseDuration / 2);
    }
}

void solve(AppState& app, std::vector<Result>& results, const bool adjustEase)
{
    results.clear();
    results.emplace_back();
    results.back().easeInOut = (&results == &app._resultsLinear) ? kEaseInOutLinear : kEaseInOutSine;
    results.back().velocities.resize(app._path.checkpoints.size() + 1);

    //std::println("Lowest,Highest,Sum,SumAbs,SumSq,SumPoz,SumNeg,Velocities");
    if (adjustEase) {
        adjustEaseDurationsP(app._path);
    }
    seedInitialVelocities(app._path, results.back());
    tessellateVelocity(app, results.back());
    tessellateProgress(app, results.back());
    tessellateAcceleration(app, results.back());
    [[maybe_unused]] const auto start = std::chrono::high_resolution_clock::now();
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
    [[maybe_unused]] const auto end = std::chrono::high_resolution_clock::now();
    //std::println("Calculation took: {} in {} iterations, with final error of: {}", end-start, results.size(), prevError);
}

void alignEaseDurations(Path& path, const int modifiedIndex)
{
    //constexpr float kEasingGuard = .9999f;
    constexpr float kEasingGuard = 1.f;
    if (modifiedIndex < static_cast<int>(path.checkpoints.size())) {
        float prevEaseDuration = modifiedIndex == -1 ? path.adjustedStartEaseDuration : path.checkpoints[static_cast<size_t>(modifiedIndex)].adjustedEaseDuration / 2.f;
        float prevTime = modifiedIndex == -1 ? path.startTime : path.checkpoints[static_cast<size_t>(modifiedIndex)].time;
        bool forcedExit = false;
        for (int index = modifiedIndex + 1; static_cast<size_t>(index) < path.checkpoints.size(); ++index) {
            Checkpoint& checkpoint = path.checkpoints[static_cast<size_t>(index)];
            const float currentTime = checkpoint.time;
            const float currentDuration = currentTime - prevTime;
            const float remainingDuration = currentDuration - prevEaseDuration;
            if (remainingDuration < 0.f) {
                checkpoint.adjustedEaseDuration = 0.f;
                forcedExit = true;
                break;
            }
            if (remainingDuration * 2.f > checkpoint.easeDuration) {
                if (currentDuration < prevEaseDuration + checkpoint.easeDuration / 2.f) {
                    checkpoint.adjustedEaseDuration = checkpoint.easeDuration * kEasingGuard;
                }
                forcedExit = true;
                break;
            }
            checkpoint.adjustedEaseDuration = remainingDuration * 2.f * kEasingGuard;
            prevEaseDuration = remainingDuration;
            prevTime = currentTime;
        }
        if (!forcedExit) {
            const float currentTime = path.endTime;
            const float currentDuration = currentTime - prevTime;
            const float remainingDuration = currentDuration - prevEaseDuration;
            if (remainingDuration < 0.f) {
                path.adjustedEndEaseDuration = 0.f;
            } else if (remainingDuration > path.endEaseDuration) {
                path.adjustedEndEaseDuration = path.endEaseDuration * kEasingGuard;
            } else {
                path.adjustedEndEaseDuration = remainingDuration * kEasingGuard;
            }
        }
    }
    if (modifiedIndex > -1) {
        float prevEaseDuration = static_cast<size_t>(modifiedIndex) == path.checkpoints.size() ? path.adjustedEndEaseDuration : path.checkpoints[static_cast<size_t>(modifiedIndex)].adjustedEaseDuration / 2.f;
        float prevTime = static_cast<size_t>(modifiedIndex) == path.checkpoints.size() ? path.endTime : path.checkpoints[static_cast<size_t>(modifiedIndex)].time;
        bool forcedExit = false;
        for (int index = modifiedIndex - 1; index > -1; --index) {
            Checkpoint& checkpoint = path.checkpoints[static_cast<size_t>(index)];
            const float currentTime = checkpoint.time;
            const float currentDuration = prevTime - currentTime;
            const float remainingDuration = currentDuration - prevEaseDuration;
            if (remainingDuration < 0.f) {
                checkpoint.adjustedEaseDuration = 0.f;
                forcedExit = true;
                break;
            }
            if (remainingDuration * 2.f > checkpoint.easeDuration) {
                if (currentDuration < prevEaseDuration + checkpoint.easeDuration / 2.f) {
                    checkpoint.adjustedEaseDuration = checkpoint.easeDuration * kEasingGuard;
                }
                forcedExit = true;
                break;
            }
            checkpoint.adjustedEaseDuration = remainingDuration * 2.f * kEasingGuard;
            prevEaseDuration = remainingDuration;
            prevTime = currentTime;
        }
        if (!forcedExit) {
            const float currentTime = path.startTime;
            const float currentDuration = prevTime - currentTime;
            const float remainingDuration = currentDuration - prevEaseDuration;
            if (remainingDuration < 0.f) {
                path.adjustedStartEaseDuration = 0.f;
            } else if (remainingDuration > path.startEaseDuration) {
                path.adjustedStartEaseDuration = path.startEaseDuration * kEasingGuard;
            } else {
                path.adjustedStartEaseDuration = remainingDuration * kEasingGuard;
            }
        }
    }
}
