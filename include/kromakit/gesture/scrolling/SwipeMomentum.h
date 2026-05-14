//
// Created by code on 4/18/26.
//

#ifndef TEST_IOS_APP_SWIPEMOMENTUM_H
#define TEST_IOS_APP_SWIPEMOMENTUM_H



class SwipeMomentum {
public:
    void Begin();                      // finger down
    void Update(float delta, float dt); // during drag
    void End();                        // finger released

    float Tick(float dt);              // returns movement this frame
    bool IsActive() const;

    void Stop();


    float velocity = 0.0f;
    bool active = false;
    bool dragging = false;

    float timeSinceMovement = 0.0f;

    float minVelocity = 25.0f;
    float friction = 5.5f;

    float releaseStaleTime = 0.08f;   // seconds
    float movementEpsilon = 0.05f;    // units
};



#endif //TEST_IOS_APP_SWIPEMOMENTUM_H
