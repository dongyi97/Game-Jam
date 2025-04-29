#include "ULevel.h"
#include "UCamera.h"
#include "URenderer.h"
#include "FEnum.h"

UCamera* ULevel::CreateCamera(int _Order, int _CameraOrder)
{
	
	UCamera* NewCamera = new UCamera(this);
	NewCamera->Start();
	Cameras.insert(std::make_pair(_CameraOrder, NewCamera));
	return NewCamera;
}
#include "UAudioManager.h"


void ULevel::Start()
{


	UCamera::TranslucentCamera = CreateCamera(0, ECAMERAORDER::Translucent);

	UCamera::MainCamera = CreateCamera(0, ECAMERAORDER::Main);

	UCamera::SubCamera = CreateCamera(0, ECAMERAORDER::Sub);


	UCamera::MainCamera->SetPosition(0, -10, 0);
	UCamera::MainCamera->SetTarget(FVector3::Zero);
	UCamera::MainCamera->SetViewPort(0, 0, 1, 1);

	UCamera::SubCamera->SetPosition(0, 0.00001f, 10.f);
	UCamera::SubCamera->SetTarget(FVector3::Zero);
	UCamera::SubCamera->SetViewPort(0.7, 0.7, 0.3, 0.3);


	UCamera::TranslucentCamera->SetPosition(0, -10, 0);
	UCamera::TranslucentCamera->SetTarget(FVector3::Zero);
	UCamera::TranslucentCamera->SetViewPort(0, 0, 1, 1);
}
void ULevel::AllRender(float _Delta)
{
	URenderer::MainRenderer->Prepare();
	URenderer::MainRenderer->PrepareShader();

	for (std::pair<const int, UCamera*>& _Pair : Cameras)
	{
		URenderer::MainRenderer->SetCamera(_Pair.second); // 렌더러 카메라 지정

		_Pair.second->UpdateCamera(); // 카메라 행렬 적용

		for (std::pair<const int, std::list<UObject*>>& _Pair : Childs)
		{

			std::list<UObject*>& Group = _Pair.second;
			for (UObject* _Child : Group)
			{
				if (false == _Child->IsUpdate())
				{
					continue;
				}
				_Child->Render();
			}
		}
	}
}

void ULevel::Update(float _Delta)
{

}

//
//
//void ULevel::Release()
//{
//}
//
//void ULevel::AllReleaseCheck()
//{
//}
//


