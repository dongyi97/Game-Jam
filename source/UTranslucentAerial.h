#pragma once
#include "FMath.h"


class UTranslucentAerial
{
public:
    // Ŭ���� �̸���, �Ʒ� �ΰ��� ���� �̸��� �������� �ʽ��ϴ�.
    FVector3 Location;
    FVector3 Velocity;
    FVector3 Rotation;
    FVector3 RotationSpeed;
    float Radius = 1.0f;
    float Mass = 0.0f;


    void Render();
};

