//
// Created by code on 4/19/26.
//

#include <kromakit/gesture/SwipeInteraction.h>

int sign(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

SwipeInteraction::SwipeInteraction() {
    dir = Direction::Vertical;
    threshold = 100;
    maxOffset = 200;
    velocityThreshold = 3;
    Reset();
}

void SwipeInteraction::Configure(
    Direction direction,
    float newMaxOffset,
    float newThreshold) {

    dir = direction;
    maxOffset = std::max(1.0f, newMaxOffset);
    threshold = std::max(0.0f, newThreshold);
}

void SwipeInteraction::SetCompletionTarget(
    CompletionTarget target,
    float fixedOffset) {

    completionTarget = target;
    completionFixedOffset = std::abs(fixedOffset);
}

void SwipeInteraction::Reset() {
    offset = 0;
    startOffset = 0;
    targetOffset = 0;
    velocity = 0;
    dragging = false;
    state = State::Idle;

    currentPoint =
    startPoint =
    lastPoint = {0,0};
}

void SwipeInteraction::TouchStart(
    DUIPoint touchPoint) {

    currentPoint = touchPoint;
    startPoint = touchPoint;
    lastPoint = touchPoint;
    startOffset = offset;

    dragging = true;
    velocity = 0;
}

void SwipeInteraction::TouchMove(
    DUIPoint touchPoint) {

    currentPoint = touchPoint;
    DUIPoint delta = touchPoint - startPoint;

    if (dir == Direction::Horizontal)
        offset = std::clamp(startOffset + delta.x, -maxOffset, maxOffset);
    else
        offset = std::clamp(startOffset + delta.y, -maxOffset, maxOffset);
}

void SwipeInteraction::TouchEnd(
    DUIPoint touchPoint) {

    currentPoint = touchPoint;
    dragging = false;

    if (std::abs(offset) > threshold || std::abs(velocity) > velocityThreshold) {
        // COMMIT (delete / close)
        int directionSign = sign(offset);
        if (directionSign == 0)
            directionSign = sign(velocity);

        targetOffset = directionSign * ResolveCompletionMagnitude();
        state = State::Completing;
    } else {
        // SNAP BACK
        targetOffset = 0;
        state = State::Returning;
    }
}

void SwipeInteraction::Tick(float deltaTime) {
    if (deltaTime <= 0.0f) {
        lastPoint = currentPoint;
        return;
    }

    // --- velocity tracking (for flick detection)
    if (dragging) {
        DUIPoint delta = currentPoint - lastPoint;

        if (dir == Direction::Horizontal)
            velocity = delta.x / deltaTime;
        else
            velocity = delta.y / deltaTime;
    }

    lastPoint = currentPoint;

    // --- animation (only when NOT dragging)
    if (!dragging) {
        float diff = targetOffset - offset;

        // simple smooth spring (feels nice)
        offset += diff * 0.2f;

        // snap when close enough
        if (abs(diff) < 0.5f) {
            offset = targetOffset;

            if (state == State::Returning) {
                // fully reset
                offset = 0;
                velocity = 0;
                state = State::Idle;
            }

            if (state == State::Completing) {
                state = State::Completed;
                // you can trigger delete/close externally here
            }
        }
    }
}

float SwipeInteraction::ResolveCompletionMagnitude() const {
    float targetMagnitude = maxOffset;

    switch (completionTarget) {
        default:
        case CompletionTarget::MaxOffset:
            targetMagnitude = maxOffset;
            break;

        case CompletionTarget::Threshold:
            targetMagnitude = threshold;
            break;

        case CompletionTarget::FixedOffset:
            targetMagnitude = completionFixedOffset;
            break;
    }

    return std::clamp(targetMagnitude, 0.0f, maxOffset);
}
