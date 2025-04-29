#include "UGravityBall.h"
#include "URenderer.h"
#include "UVariables.h"
#include "UBall.h"
#include "UBallSpawner.h"
#include "UCamera.h"

GravityBall::GravityBall()
{
    Location = { 0,0,0 };
    Radius = 0.2f;
    Mass = Radius * Radius * Radius;
    Rotation = Rotation = FVector3::RandomVector(-1, 1);
}

void GravityBall::Update()
{
    Render();
}

void GravityBall::Gravity(UBallSpawner* ball, float deltatime)
{
    UBall* curBall = ball->HeadBall();
    while(curBall != nullptr)
    {
        if (CheckBallCollision(curBall))
        {
            //curBall->Velocity = FVector3(0, 0, 0);

            // 블랙홀 방향 벡터
            FVector3 directionToGravityBall = Location - curBall->Location;
            float distance = directionToGravityBall.Length();

            // 공의 운동량 보존 : 공이 블랙홀에 가까워지면 속도 벡터를 블랙홀 방향과 수직으로 조정
            FVector3 velocityPerpendicularToBlackHole = curBall->Velocity - directionToGravityBall * curBall->Velocity.DotProduct(directionToGravityBall, curBall->Velocity);

            curBall->Velocity = velocityPerpendicularToBlackHole;


            //curBall->Velocity = curBall->Velocity * (1.0f - curBall->frictionCoefficient) * deltatime;
            curBall->Velocity = curBall->Velocity - (curBall->Velocity * 0.999 *deltatime);

            if (curBall->Velocity.Length() < 0.0001f)
            {
                curBall->Velocity = FVector3(0, 0, 0);
            }


            // 충돌 후 위치 보정 (서로 겹치지 않게)
            // 
            // 거리 벡터
            FVector3 collisionVector = Location - curBall->Location;
            collisionVector.Normalize();

            float overlap = (Radius + curBall->Radius) - FVector3::Distance(Location, curBall->Location);
            FVector3 correctionVector = collisionVector * (overlap / 2);
            curBall->Location = curBall->Location - correctionVector;

            curBall->SetIsCollided(true);
        }
        else
        {
            FVector3 dir = curBall->Location - Location;
            curBall->Velocity = curBall->Velocity -  ( (dir * (9.8 * Mass / (dir.Length() * dir.Length()))) * deltatime * 20);
        }

        curBall = curBall->nextBall;
    }
}

void GravityBall::Render()
{

    UCamera::MainCamera->UpdateCamera();
    URenderer::MainRenderer->UpdateConstant(FVector3::One * Radius, Location,FVector3(1.0f,1.0f,1.0f) * -1, Rotation);
    URenderer::MainRenderer->RenderPrimitive(vertexBufferSphere, numVerticesSphere);

    UCamera::SubCamera->UpdateCamera();
    URenderer::MainRenderer->UpdateConstant(FVector3::One * Radius, Location,FVector3(1.0f,1.0f,1.0f) * -1, Rotation);
    URenderer::MainRenderer->RenderPrimitive(vertexBufferSphere, numVerticesSphere);
}

bool GravityBall::CheckBallCollision(UBall* other) {

    float distance = sqrtf((Location.x - other->Location.x) * (Location.x - other->Location.x)
        + (Location.y - other->Location.y) * (Location.y - other->Location.y)) + (Location.z - other->Location.z) * (Location.z - other->Location.z);

    return distance < (Radius + other->Radius);
};