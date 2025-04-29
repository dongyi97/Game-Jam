#ifndef UBALL_H
#define UBALL_H

#include "FMath.h"

// �༺ Ÿ��
enum EBallType
{
    MOON = 0,
    VENUS = 1,
    EARTH = 2,
    MARS = 3,
    JUPITER = 4,
    SUN = 5
};

// ���� ����
class UBallSpawner;

class UBall
{
public:
    // Ŭ���� �̸���, �Ʒ� �ΰ��� ���� �̸��� �������� �ʽ��ϴ�.
    FVector3 Location;
    FVector3 Velocity;
    FVector3 Rotation;
    FVector3 RotationSpeed;
    FVector3 Color;
    float Radius;
    float Mass;
    EBallType type;
    
    static int counter;
    int id;

    float frictionCoefficient = 0.1f;

    UBall* nextBall = nullptr;

    const float minRadius = 0.05f, maxRadius = 0.1f;
    const float minVelocity = -0.1f, maxVelocity = 0.1f;
    const float minRotationSpeed = -1.0f, maxRotationSpeed = 1.0f;

private:
    bool bIsShooted = false;
    bool bIsCollided = false;

public:
    UBall(); //�ʱ� ������

    UBall(int type, FVector3 location); //�ռ�

    void UpdateBalls();

    //void FrameUpdate(float deltaTime, UBallSpawner* uballspawner);
    void FrameUpdate(float deltaTime, UBallSpawner* uballspawner, std::vector<std::pair<int, int>> &v);

    bool CheckCollision();

    void MakeNewBall();

    void Shoot() { bIsShooted = true; }

    bool IsCollided() { return bIsCollided; } void SetIsCollided(bool value) { bIsCollided = value; }
    
private:
    void Render();

    void Move(float deltaTime);

    void Rotate(float deltaTime);

    void OnBallCollision(UBall* other, UBallSpawner* uballspawner);

    //void OnCollision(UBallSpawner* uballspawner);
    void OnCollision(UBallSpawner* uballspawner, std::vector<std::pair<int,int>> &v);

public:

    bool CheckBallCollision(UBall* other);

	bool IsUpdate = true;
};

#endif