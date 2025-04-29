#ifndef UBALLSPAWNER_H
#define UBALLSPAWNER_H

#include "FMath.h"
#include "FEnum.h"
class UBall;

class UBallSpawner
{
public:
    ~UBallSpawner();

    int BallCount = 1;
    UBall* HeadBall();
    int ListCount() const;

    void UpdateBallCount();
    void ClearBall();

private:
    UBall* headBall = nullptr;
    int listCount = 0;

    void SpawnBall();

public:
    void SpawnBiggerBall(FVector3 spawnlocation, int type);

public:
    void DestroyBall(UBall* ball);

    UBall* GetIndexedBall(int index);
};

#endif