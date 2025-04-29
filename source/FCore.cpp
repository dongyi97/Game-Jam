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

    // D3D11 �����ϴ� �Լ��� ȣ���մϴ�.
    renderer->Create(FWindowManager::GetHWND());

    // ������ ���� ���Ŀ� ���̴��� �����ϴ� �Լ��� ȣ���մϴ�.
    renderer->CreateShader();
    renderer->CreateConstantBuffer();

    URenderer::MainRenderer = renderer;

    // UUIManager ���� �� �ʱ�ȭ
    UUIManager::MainUUIManger = new UUIManager();
    UUIManager* uiManager = UUIManager::MainUUIManger;

    uiManager->Initialize(hWnd);

    UAudioManager* audioManager = &UAudioManager::MainAudioManager;

    // UAudioaManager ���� �� �ʱ�ȭ
    audioManager->Initialize();
}

void FCore::Update()
{
}
