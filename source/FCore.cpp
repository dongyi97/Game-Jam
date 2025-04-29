#include "FCore.h"
#include "ULevel.h"
#include "UCamera.h"
#include "FWindowManager.h"
#include "URenderer.h"
#include "UUIManager.h"
#include "UAudioManager.h"

void FCore::Start(HWND hWnd)
{

	ULevel* level = new ULevel();
	level->Start();


	UCamera::MainCamera = new UCamera(level);
	UCamera::MainCamera->Start();


    URenderer* renderer = new URenderer();

    // D3D11 생성하는 함수를 호출합니다.
    renderer->Create(FWindowManager::GetHWND());

    // 렌더러 생성 직후에 쉐이더를 생성하는 함수를 호출합니다.
    renderer->CreateShader();
    renderer->CreateConstantBuffer();

    URenderer::MainRenderer = renderer;

    // UUIManager 생성 및 초기화
    UUIManager::MainUUIManger = new UUIManager();
    UUIManager* uiManager = UUIManager::MainUUIManger;

    uiManager->Initialize(hWnd);

    UAudioManager* audioManager = &UAudioManager::MainAudioManager;

    // UAudioaManager 생성 및 초기화
    audioManager->Initialize();
}

void FCore::Update()
{
}
