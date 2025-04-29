#include "UCamera.h"
#include "URenderer.h"
#include "FWindowManager.h"
#include "DirectXMath.h"

UCamera* UCamera::MainCamera = nullptr;

UCamera* UCamera::TranslucentCamera = nullptr;
UCamera* UCamera::SubCamera = nullptr;



void UCamera::Start()
{
    



    
}

void UCamera::UpdateCamera() {
    renderer = URenderer::MainRenderer; // TODO: �ٸ� ������ �� �� �Ҵ����ֱ� or Renderer���� �� ī�޶� �׸��� ���� �Ҵ����ֱ�
    if (renderer == nullptr) return;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    renderer->SwapChain->GetDesc(&swapChainDesc);

    renderer->SetCustomViewport(viewportInfo.topLeftX, viewportInfo.topLeftY, viewportInfo.width, viewportInfo.height);

    // View Matrix ����
    /*float x = 0.3f * sinf(cameraHorizontalAngle * DEG_TO_RAD);
    float y = 0.3f * sinf(cameraVerticalAngle * DEG_TO_RAD);
    float z = 0.3f * cosf(cameraHorizontalAngle * DEG_TO_RAD);*/

    DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(Transform.Pos.x, Transform.Pos.y, Transform.Pos.z, 1.0f);

    DirectX::XMVECTOR target = DirectX::XMVectorSet(Target.x, Target.y, Target.z, 1.0f);

    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    ViewMatrix = DirectX::XMMatrixLookAtLH(cameraPos, target, up);

    if (projectionType == ECameraProjectionType::Perspective) {
        // ���� ��� ����
        float fovAngleY = DirectX::XM_PIDIV4;
        float aspectRatio = swapChainDesc.BufferDesc.Width / swapChainDesc.BufferDesc.Height;
        ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, Near, Far);
    }
    else if (projectionType == ECameraProjectionType::Orthographic) {
        ProjectionMatrix = DirectX::XMMatrixOrthographicLH(OrthoWidth, OrthoHeight, Near, Far);
    }

    // ��� ���� ������Ʈ
    D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

    renderer->DeviceContext->Map(renderer->TransformConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
    URenderer::FTransformConstants* constants = (URenderer::FTransformConstants*)constantbufferMSR.pData;
    {
        constants->worldViewProj = XMMatrixTranspose(ViewMatrix * ProjectionMatrix);
    }
    renderer->DeviceContext->Unmap(renderer->TransformConstantBuffer, 0);

}