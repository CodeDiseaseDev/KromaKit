//
// Created by code on 4/19/26.
//

#ifndef TEST_IOS_APP_SWIPEINTERACTION_H
#define TEST_IOS_APP_SWIPEINTERACTION_H
#include <kromakit/graphics/Graphics.h>
#include <algorithm>
#include <cmath>


class SwipeInteraction {
public:
    enum class Direction { Horizontal, Vertical };
    enum class State { Completing, Returning, Idle, Completed };
    enum class CompletionTarget { MaxOffset, Threshold, FixedOffset };

    float velocityThreshold = 3;
    float offset = 0;        // how far dragged (px)
    float velocity = 0;      // for flicks
    bool dragging = false;
    float startOffset = 0;   // baseline offset captured at touch start

    Direction dir;
    State state;

    float targetOffset;

    float threshold = 100;   // px to commit
    float maxOffset = 200;   // clamp
    CompletionTarget completionTarget = CompletionTarget::MaxOffset;
    float completionFixedOffset = 0.0f;

    DUIPoint startPoint, currentPoint, lastPoint;

    SwipeInteraction();

    void Configure(Direction direction, float newMaxOffset, float newThreshold);
    void Reset();
    void SetCompletionTarget(CompletionTarget target, float fixedOffset = 0.0f);

    void TouchStart(DUIPoint touchPoint);
    void TouchMove(DUIPoint touchPoint);
    void TouchEnd(DUIPoint touchPoint);

    void Tick(float deltaTime);

    bool IsCompleting() const {
        return state == State::Completing || state == State::Completed;
    }

    float GetSignedProgress() const {
        return std::clamp(offset / maxOffset, -1.0f, 1.0f);
    }

    float GetProgress01() const {
        return std::clamp(std::abs(offset) / maxOffset, 0.0f, 1.0f);
    }

private:
    float ResolveCompletionMagnitude() const;
};



#endif //TEST_IOS_APP_SWIPEINTERACTION_H
