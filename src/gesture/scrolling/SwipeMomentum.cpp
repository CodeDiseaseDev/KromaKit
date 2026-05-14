#include <kromakit/gesture/scrolling/SwipeMomentum.h>
#include <algorithm>
#include <cmath>

void SwipeMomentum::Begin() {
    velocity = 0.0f;
    active = false;
    dragging = true;
    timeSinceMovement = 0.0f;
}

void SwipeMomentum::Update(float delta, float dt) {
    if (dt <= 0.0001f) return;

    dragging = true;

    if (std::abs(delta) <= movementEpsilon) {
        timeSinceMovement += dt;

        // If the pointer has basically stopped while still held,
        // bleed velocity down so releasing after braking doesn't fling.
        velocity *= std::exp(-friction * dt);

        if (std::abs(velocity) < minVelocity) {
            velocity = 0.0f;
        }

        return;
    }

    timeSinceMovement = 0.0f;

    const float instantVelocity = delta / dt;

    // More responsive than a long whole-gesture average.
    // This follows recent movement but still filters frame spikes.
    const float response = 0.35f;
    velocity += (instantVelocity - velocity) * response;
}

void SwipeMomentum::End() {
    dragging = false;

    // If the user stopped before releasing, don't fling.
    if (timeSinceMovement > releaseStaleTime) {
        velocity = 0.0f;
        active = false;
        return;
    }

    if (std::abs(velocity) > minVelocity) {
        active = true;
    } else {
        velocity = 0.0f;
        active = false;
    }
}

float SwipeMomentum::Tick(float dt) {
    if (!active || dragging || dt <= 0.0001f)
        return 0.0f;

    velocity *= std::exp(-friction * dt);

    if (std::abs(velocity) < minVelocity) {
        velocity = 0.0f;
        active = false;
        return 0.0f;
    }

    return velocity * dt;
}

bool SwipeMomentum::IsActive() const {
    return active;
}

void SwipeMomentum::Stop() {
    velocity = 0.0f;
    active = false;
    dragging = false;
    timeSinceMovement = 0.0f;
}