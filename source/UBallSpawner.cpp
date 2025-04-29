#include "UBallSpawner.h"
#include "UBall.h"
#include "UGameManager.h"

UBallSpawner::~UBallSpawner() {
    // �Ҹ��ڿ��� ���ҽ� ���� �� ���� �۾� ����
    UBall* curBall = headBall;
    while (curBall != nullptr) {
        UBall* nextBall = curBall->nextBall; // ���� ��带 ����
        delete curBall; // ���� ��� ����
        curBall = nextBall; // ���� ���� �̵�
    }
    headBall = nullptr;
}

UBall* UBallSpawner::HeadBall() { 
    return headBall; 
}

int UBallSpawner::ListCount() const { 
    return listCount; 
}

void UBallSpawner::UpdateBallCount() {
    if (BallCount < 0) BallCount = 0;

    while (listCount != BallCount) {
        if (listCount > BallCount) {
            //int randomValue = rand() % listCount;
            //DestroyBall(GetIndexedBall(randomValue));
            listCount--;
        }
        else if (listCount < BallCount) {
            SpawnBall();
        }
        /*if (listCount < BallCount)
        {
            SpawnBall();
        }*/
    }
}

void UBallSpawner::ClearBall() {
    UBall* curBall = headBall->nextBall;
    while (curBall != nullptr) {
        curBall->SetIsCollided(false);
        curBall->Location = FVector3(100 * curBall->id,0,0);
        curBall = curBall->nextBall;
    }
}

void UBallSpawner::SpawnBall() {
    UBall* newBall = new UBall();

    if (headBall == nullptr)
        headBall = newBall;
    else {
        newBall->nextBall = headBall;
        headBall = newBall;
    }

    //������� �����Ǿ��ִ� ������� ��ħ �˻�
    int count = 0;
    const int maxTry = 300;
    while (newBall->CheckCollision()) {
        newBall->MakeNewBall();
        count++;
        if (count > maxTry) {
            BallCount = listCount;
            DestroyBall(newBall);
            listCount++;
            return;
        }
    }

    listCount++;
}

void UBallSpawner::SpawnBiggerBall(FVector3 spawnlocation, int type)
{
    if (UGameManager::getInstance()->IsGameOver()) return;
    type++;
    if (type == 6) return;

    UBall* newBall = new UBall(type, spawnlocation);

    if (headBall == nullptr)
        headBall = newBall;
    else {
        newBall->nextBall = headBall;
        headBall = newBall;
    }

    UBall *curBall = headBall->nextBall;

    FVector3 dir = spawnlocation;
    //dir.Normalize();


    while (curBall != nullptr)
    {
        while (headBall->CheckBallCollision(curBall))
        {
            headBall->Location = headBall->Location + dir * 0.01f;
        }

        curBall = curBall->nextBall;
    }

    while (headBall->Location.Length() < 0.2 + headBall->Radius)
    {
        headBall->Location = headBall->Location + dir * 0.01f;
    }
    
    listCount++;
    BallCount++;

}


void UBallSpawner::DestroyBall(UBall* ball) {
    if (ball == nullptr) return;

    if (headBall == ball) {
        headBall = ball->nextBall;
        delete ball;
        listCount--;
        return;
    }

    return;

    UBall* curBall = headBall;
    while (curBall->nextBall != ball) {
        curBall = curBall->nextBall;
    }

    curBall->nextBall = ball->nextBall;
    delete ball;
    listCount--;
}

UBall* UBallSpawner::GetIndexedBall(int index) {
    UBall* result = headBall;
    for (int i = 0; i < index; i++) {
        result = result->nextBall;
    }

    return result;
}