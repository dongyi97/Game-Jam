#pragma once
#include "FMath.h"


class UTranslucentAerial
{
public:
    // 클래스 이름과, 아래 두개의 변수 이름은 변경하지 않습니다.
    FVector3 Location;
    FVector3 Velocity;
    FVector3 Rotation;
    FVector3 RotationSpeed;
    float Radius = 1.0f;
    float Mass = 0.0f;


    void Render();
};

