#include "UTranslucentAerial.h"
#include "UCamera.h"
#include "URenderer.h"
#include "UVariables.h"

void UTranslucentAerial::Render()
{
    UCamera::MainCamera->UpdateCamera();

    URenderer::MainRenderer->UpdateConstant(FVector3::One * Radius, Location, FVector3(0.0f, 0.0f, 0.0f), Rotation, true);
    URenderer::MainRenderer->RenderPrimitive(vertexBufferSphereTrans, numVerticesSphere, indexBufferSphere);


    UCamera::SubCamera->UpdateCamera();

    URenderer::MainRenderer->UpdateConstant(FVector3::One * Radius, Location, FVector3(0.0f, 0.0f, 0.0f), Rotation, true);
    URenderer::MainRenderer->RenderPrimitive(vertexBufferSphereTrans, numVerticesSphere, indexBufferSphere);
}
