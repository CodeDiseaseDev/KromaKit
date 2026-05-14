//
// Created by code on 4/19/26.
//

#include <kromakit/animation/FloatEaseAnimation.h>
#include <cmath>

#include <kromakit/Utils.h>

float FloatEaseAnimation::Get(
    float target, float deltaTime) {

    float t = DirectUIUtils::clamp(
        speed * deltaTime, 0.0f, 1.0f);

    a += (target - a) * t;
    return a;
}
