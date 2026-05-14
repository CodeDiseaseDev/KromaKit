//
// Created by code on 4/19/26.
//

#ifndef TEST_IOS_APP_FLOATEASEANIMATION_H
#define TEST_IOS_APP_FLOATEASEANIMATION_H
#include <kromakit/animation/IAnimation.h>


class FloatEaseAnimation  {
public:
    float a = 0.0f;

    float speed = 1.0f;



    float Get(
        float target,
        float deltaTime
    );
};



#endif //TEST_IOS_APP_FLOATEASEANIMATION_H
