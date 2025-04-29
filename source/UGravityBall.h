#pragma once
#include "FMath.h"

class UBallSpawner;
class UBall;

class GravityBall
{
public:

    FVector3 Location;
    float Radius;
    float Mass;
    FVector3 Rotation;

    GravityBall();
    
    void Update();

    void Gravity(UBallSpawner* ball, float deltatime);


private:
    void Render();

    bool CheckBallCollision(UBall* other);

};
