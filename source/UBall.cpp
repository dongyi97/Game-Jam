#include "UBall.h"
#include "UBallSpawner.h"
#include "URenderer.h"
#include "UVariables.h"
#include "time.h"
#include "UCamera.h"
#include "UGameManager.h"

//#include "main.cpp"
#include <vector>

UBall::UBall() {
    srand(time(0));
    int n = static_cast<int>(rand()) % 3;

    switch (n)
    {
    case 0:
        type = MOON;
        Radius = 0.05;
        Color = FVector3::One * 0.5f;
        break;
    case 1:
        type = VENUS;
        Radius = 0.08;
        Color = FVector3(1, 0.9, 0.7) * 0.6f;
        break;
    case 2:
        type = EARTH;
        Radius = 0.11;
        Color = FVector3(0.1, 0.2, 0.5);
        break;
    }

    Location = FVector3(1000,0,0);
    Velocity = FVector3::RandomVector(minVelocity, maxVelocity);
    Rotation = FVector3::RandomVector(-1, 1);
    RotationSpeed = FVector3::RandomVector(minRotationSpeed, maxRotationSpeed);
    Mass = Radius * Radius * Radius * 4;

    id = counter;
    counter++;

}

UBall::UBall(int type, FVector3 location) {

    switch (type)
    {
    case 1:
        type = VENUS;
        Radius = 0.08;
        Color = FVector3(1, 0.9, 0.7);
        break;
    case 2:
        type = EARTH;
        Radius = 0.11;
        Color = FVector3(0.1, 0.2, 0.5);
        break;
    case 3:
        type = MARS;
        Radius = 0.14;
        Color = FVector3(0.05, 0.6, 0.5);
        break;
    case 4:
        type = JUPITER;
        Radius = 0.17;
        Color = FVector3(0.65, 0.5, 0.4);
        break;
    case 5:
        type = SUN;
        Radius = 0.20;
        Color = FVector3(0.8, 0.3, 0.05);
        break;
    }
    bIsShooted = true;

    Location = location;
    //Velocity = FVector3::RandomVector(minVelocity, maxVelocity);
    Velocity = FVector3(0, 0, 0);
    Rotation = FVector3::RandomVector(-1, 1);
    RotationSpeed = FVector3::RandomVector(minRotationSpeed, maxRotationSpeed);
    Mass = Radius * Radius * Radius * 4;

    id = counter;
    counter++;
}

void UBall::UpdateBalls()
{
    if (IsUpdate == false)
    {
        return;
    }
    Render();
}

void UBall::FrameUpdate(float deltaTime, UBallSpawner* uballspawner, std::vector<std::pair<int,int>> &v) {
    if (IsUpdate == false)
    {
        return;
    }
    if (bIsShooted)
        Move(deltaTime);
    else
        Location = UCamera::MainCamera->GetPosition() * 0.8f - FVector3(0,0,0.4);

    // 충돌 후 원을 나가면 아웃
    if (bIsCollided && (FVector3::Distance(FVector3::Zero, Location) > 1)) //TODO: 매직 넘버 삭제
        UGameManager::getInstance()->GameOver();
    OnCollision(uballspawner, v);
}

bool UBall::CheckCollision() {
    // 다른 공들에 대한 검사
    UBall* curBall = nextBall;
    while (curBall != nullptr) {
        if (CheckBallCollision(curBall)) {
            return true;
        }
        curBall = curBall->nextBall;
    }

    return false;
}

void UBall::MakeNewBall() { // 현재 공보다 작게 랜덤 위치에서 생성
    Location = FVector3::RandomVector(-1, 1);
}

void UBall::Render()
{
    if (IsUpdate == false)
    {
        return;
    }
    UCamera::MainCamera->UpdateCamera();

    URenderer::MainRenderer->UpdateConstant(FVector3::One * Radius, Location, Color,  Rotation);
    URenderer::MainRenderer->RenderPrimitive(vertexBufferSphere, numVerticesSphere);


    UCamera::SubCamera->UpdateCamera();

    URenderer::MainRenderer->UpdateConstant(FVector3::One * Radius, Location, Color, Rotation);
    URenderer::MainRenderer->RenderPrimitive(vertexBufferSphere, numVerticesSphere);
}

//void UBall::ApplyGravity(float deltaTime) {
//    Velocity.y = Velocity.y + GRAVITY.y * deltaTime;
//}

void UBall::Move(float deltaTime) {
    if (IsUpdate == false)
    {
        return;
    }
    Location.x += Velocity.x * deltaTime;
    Location.y += Velocity.y * deltaTime;
    Location.z += Velocity.z * deltaTime;
}

void UBall::Rotate(float deltaTime) {
    if (IsUpdate == false)
    {
        return;
    }
    Rotation = Rotation + RotationSpeed * deltaTime;
}

void UBall::OnBallCollision(UBall* other, UBallSpawner* uballspawner) {

    if (IsUpdate == false)
    {
        return;
    }
    // 거리 벡터
    FVector3 collisionVector = Location - other->Location;
    collisionVector.Normalize();

    // 상대 속도
    FVector3 relativeVelocity = Velocity - other->Velocity;

    // 속도 변화량 계산
    float velocityAlongCollision = FVector3::DotProduct(relativeVelocity, collisionVector);

    // 충돌 각 공에 대한 반응 속도
    float impulse = 2 * velocityAlongCollision / (Mass + other->Mass);

    // 각 공의 새 속도 계산
    Velocity = Velocity - collisionVector * other->Mass * impulse;
    other->Velocity = other->Velocity + collisionVector * impulse * Mass;

    // 2. 각운동량 처리 추가
// 충돌점까지의 방향 벡터 계산 (공의 반지름만큼 떨어진 지점)
    FVector3 r1 = collisionVector * Radius;
    FVector3 r2 = collisionVector * -other->Radius;

    // 충돌에 의한 각속도 변화량 계산
    // 충격량과 충돌점까지의 거리를 이용해 각속도 변화 계산
    float angularImpulse1 = (FVector3::CrossProduct(r1, collisionVector * impulse)).Length() * 0.1f;
    float angularImpulse2 = (FVector3::CrossProduct(r2, collisionVector * impulse)).Length() * 0.1f;

    // 회전축 계산 (충돌 방향의 수직 방향)
    FVector3 rotationAxis = FVector3(-collisionVector.y, collisionVector.x, 0.0f);

    // 각속도 업데이트
    RotationSpeed = RotationSpeed + rotationAxis * angularImpulse1;
    other->RotationSpeed = other->RotationSpeed - rotationAxis * angularImpulse2;

    // 충돌 후 위치 보정 (서로 겹치지 않게)
    float overlap = (Radius + other->Radius) - FVector3::Distance(Location, other->Location);
    FVector3 correctionVector = collisionVector * (overlap / 2);
    Location = Location + correctionVector;
    other->Location = other->Location - correctionVector;

    bIsCollided = true;
}

void UBall::OnCollision(UBallSpawner* uballspawner, std::vector<std::pair<int,int>> &v) {

    if (IsUpdate == false)
    {
		return;
    }
    // 다른 공들에 대한 검사
    UBall* curBall = nextBall;
    while (curBall != nullptr) {
        if (IsUpdate == false or curBall->IsUpdate == false)
        {
            return;
        }

        if (CheckBallCollision(curBall)) {
            if (curBall->type == this->type) {
                //uballspawner->DestroyBall(this);
                //uballspawner->DestroyBall(curBall);

                FVector3 spawnlocation = (curBall->Location + this->Location) / 2;
                //FVector3 spawnlocation = (curBall->Location + this->Location) * 5;
				IsUpdate = false;


                bIsCollided = false;
                curBall->SetIsCollided(false);
                Location = FVector3(1000*uballspawner->ListCount(), 0, 0);
                curBall->Location = FVector3(0, 0, 1000 * uballspawner->ListCount());
                //curBall = nullptr;

                v.push_back(std::make_pair(this->id, curBall->id));

                //FVector3 spawnlocation = (curBall->Location + this->Location) / 2;

                uballspawner->SpawnBiggerBall(spawnlocation, static_cast<int> (this->type));



            }
            OnBallCollision(curBall, uballspawner);
            
        }
        
        curBall = curBall->nextBall;
    }
}

bool UBall::CheckBallCollision(UBall* other) {

    float distance = sqrtf((Location.x - other->Location.x) * (Location.x - other->Location.x)
        + (Location.y - other->Location.y) * (Location.y - other->Location.y) + (Location.z - other->Location.z) * (Location.z - other->Location.z));

    return distance < (Radius + other->Radius);
};