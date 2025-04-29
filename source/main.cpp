#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"

// D3D ��뿡 �ʿ��� ������ϵ��� �����մϴ�.
#include <d3d11.h>
#include <d3dcompiler.h>

// D3D ��뿡 �ʿ��� ���̺귯������ ��ũ�մϴ�.
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#include "Sphere.h"
#include "FMath.h"
#include "FWindowManager.h"
#include "UAudioManager.h"

#include "ULevel.h"
#include "URenderer.h"
#include "UCamera.h"
#include "UUIManager.h"
#include "UBall.h"
#include "UBallSpawner.h"
#include "UGravityBall.h"
#include "UVariables.h"

#include "FCore.h"
#include <stdlib.h>
#include <crtdbg.h>
#include "UTranslucentAerial.h"

#include "UTimeManager.h"
#include "UGameManager.h"
#include "InputManager.h"

#include <vector>

// ----- Variables
//URenderer* renderer;
//UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);
//ID3D11Buffer* vertexBufferSphere;
//
//const FVector3 GRAVITY = FVector3(0, -9.8f, 0);
//
//const float LEFT_BORDER = -1.0f;
//const float RIGHT_BORDER = 1.0f;
//const float TOP_BORDER = 1.0f;
//const float BOTTOM_BORDER = -1.0f;
//
//bool bIsGravity = false;
//bool bIsMagnetic = false;
//bool bIsRotate = false;

// �༺ Ÿ��
//enum Type
//{
//    MARS,
//    EARTH,
//    JUPITER,
//};
bool isShooting = false;

void UpdateCameraOrbitWithIrregularTilt(UCamera* inCamera, float deltaTime)
{
    // �߽��� gravityball�� ��ġ (0,0,0)
    FVector3 center = FVector3();
    const float orbitRadius = 5.0f;
    
    // ����� ���� theta: xy���(�Ǵ� ���ϴ� ���)������ ȸ����
    static float theta = -3.14159265f / 2.0f;  // �ʱⰪ: (0, -5, 0)�� �ش��ϴ� ����
    const float thetaSpeed = 2.f;  // �ʴ� ȸ�� �ӵ� (����)
    //theta += thetaSpeed * deltaTime;
    
    // ���� �ұ�Ģ�� �������� ���� �ð� ���� ����
    static float phiTime = 0.0f;
    phiTime += deltaTime;
    
    // ���� �����ϴ� �Ķ���͵�
    const float phiFrequency = 0.2f; // �⺻ ���ļ�
    const float phiMax = 0.5f;       // �ִ� tilt ���� (����)
    
    // �ܼ� sin ��� sin�� cos�� ������ ����ؼ� �ұ�Ģ�� ������ �ο�
    float phi = phiMax * (0.6f * sinf(phiFrequency * phiTime) + 0.3f * cosf(1.5f * phiTime));
    
    // ���� ��ǥ��κ��� ī�޶��� Cartesian ��ǥ ���
    // x = r * cos(��) * cos(��)
    // y = r * cos(��) * sin(��)
    // z = r * sin(��)
    float x = orbitRadius * cosf(phi) * cosf(theta);
    float y = orbitRadius * cosf(phi) * sinf(theta);
    float z = orbitRadius * sinf(phi);
    
    // ī�޶� ��ġ�� Ÿ�� ������Ʈ
    inCamera->SetPosition(center + FVector3(x, y, z));
    inCamera->SetTarget(center);
}



 //main.cpp �� �̺�Ʈ ��� ����
void RegisterInputEvents(UCamera* inCamera, UBallSpawner* ballSpawner) {
    //const float orbitRadius = 10.0f;      // ī�޶�� gravityball ������ �Ÿ�

    //const float angleDelta = 0.1f;      // ȸ�� ���� (���� ����)

    //// AŰ: z�� �������� +angleDelta ��ŭ ȸ�� (���� ȸ��)
    //InputManager::RegisterEvent("OnLeftArrow", [inCamera, orbitRadius, angleDelta]() {
    //    FVector3 center = FVector3();
    //    FVector3 camPos = inCamera->GetPosition();
    //    FVector3 offset = camPos - center;  // ���� ������ (��: (0, -5, 0))

    //    float cosTheta = cosf(angleDelta);
    //    float sinTheta = sinf(angleDelta);
    //    FVector3 newOffset;
    //    newOffset.x = offset.x * cosTheta - offset.y * sinTheta;
    //    newOffset.y = offset.x * sinTheta + offset.y * cosTheta;
    //    newOffset.z = 0.0f;  // xy��鿡���� z���� 0

    //    // �������� ���̸� orbitRadius�� ���� (�ε��Ҽ��� ���� ����)
    //    float length = sqrtf(newOffset.x * newOffset.x + newOffset.y * newOffset.y);
    //    if (length != 0.0f) {
    //        newOffset.x = newOffset.x / length * orbitRadius;
    //        newOffset.y = newOffset.y / length * orbitRadius;
    //    }
    //    inCamera->SetPosition(center + newOffset);
    //    inCamera->SetTarget(center);  // �׻� gravityball�� �ٶ󺸵���
    //    });

    InputManager::RegisterEvent("OnPressedSpace", [ballSpawner]() {
        if (isShooting == false && UGameManager::getInstance()->IsGameOver() == false) {
            isShooting = true;
            ballSpawner->HeadBall()->Shoot();
            UTimeManager::Invoke([ballSpawner]() {
                ballSpawner->BallCount++;
                }, 2);
            UTimeManager::Invoke([]() {isShooting = false; }, 2.5f);
        };
        });
    InputManager::RegisterEvent("OnPressedR", [ballSpawner]() {
        if (UGameManager::getInstance()->IsGameOver()) {
            ballSpawner->ClearBall();
            UGameManager::getInstance()->Restart();
        }
        });
}

int UBall::counter = 1;
std::vector<std::pair<int, int>> v;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


// ���� �޽����� ó���� �Լ�
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return InputManager::WndProc(hWnd, message, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
     
	//_CrtSetBreakAlloc(291); // ������� �޸� ��� ��ȣ
    // 
    // 
    // ���� �����ϴ� �ڵ带 ���⿡ �߰��մϴ�.
	// ������ Ŭ���� �̸�
	WCHAR WindowClass[] = L"JungleWindowClass";

	// ������ Ÿ��Ʋ�ٿ� ǥ�õ� �̸�
	WCHAR Title[] = L"Game Tech Lab";

	// ���� �޽����� ó���� �Լ��� WndProc�� �Լ� �����͸� WindowClass ����ü�� �ִ´�.
	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

	// ������ Ŭ���� ���
	RegisterClassW(&wndclass);

	// 1024 x 1024 ũ�⿡ ������ ����
	HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024,
		nullptr, nullptr, hInstance, nullptr);

	FWindowManager::SetHWND(hWnd);

	FCore::Start(hWnd);
    UTimeManager::Init();

	ULevel* level = new ULevel();
    level->Start();

    // �׽�Ʈ�� ���� �������� �ۼ��߾���
    MouseInput mouseInput;

    mouseInput.Initialize(hInstance, FWindowManager::GetHWND(), 1024, 1024);


    // ���⿡�� ImGui�� �����մϴ�.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init((void*)hWnd);
    ImGui_ImplDX11_Init(URenderer::MainRenderer->Device, URenderer::MainRenderer->DeviceContext);

    // Renderer�� Shader ���� ���Ŀ� ���ؽ� ���۸� �����մϴ�.

    std::pair<std::vector<FVertexSimple>, std::vector<unsigned int>> Sphere = FMath::CreateSphereWithIndices(1.0f, 32,FVector3(0.f,0.8f,0.6f) );

    numVerticesSphere = sizeof(FVertexSimple) * Sphere.first.size();

    vertexBufferSphere = URenderer::MainRenderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
    //vertexBufferSphere = URenderer::MainRenderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));

    vertexBufferSphereTrans = URenderer::MainRenderer->CreateVertexBuffer(&Sphere.first[0], sizeof(FVertexSimple) * Sphere.first.size());
    indexBufferSphere = URenderer::MainRenderer->CreateIndexBuffer(&Sphere.second[0], sizeof(unsigned int) * Sphere.second.size());

    // FPS ������ ���� ����
    const int targetFPS = 30;
    const double targetFrameTime = 1.0 / targetFPS; // �� �������� ��ǥ �ð� (�и��� ����)
    double frameAccumulatedTime = 0.0f;

    // ���� Ÿ�̸� �ʱ�ȭ
    LARGE_INTEGER frequency;
    LARGE_INTEGER currentTime, previousTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&previousTime);

    // ----- Variables
    bool bIsExit = false;

    UBallSpawner* ballSpawner = new UBallSpawner();

    RegisterInputEvents(UCamera::MainCamera, ballSpawner);

    //GravityBall ����
    GravityBall gravityball;

	UTranslucentAerial translucentAerial;

	// Main Loop (Quit Message�� ������ ������ �Ʒ� Loop�� ������ �����ϰ� ��)
    while (bIsExit == false)
    {
        QueryPerformanceCounter(&currentTime);

        // ��Ÿ Ÿ�� ��� (�� ����)
        double deltaTime = static_cast<double>(currentTime.QuadPart - previousTime.QuadPart) / frequency.QuadPart;
        previousTime = currentTime;

        // �ð� ����
        frameAccumulatedTime += deltaTime;
        UTimeManager::AddDeltaTime(deltaTime);

        UpdateCameraOrbitWithIrregularTilt(UCamera::MainCamera, static_cast<float>(deltaTime));

        MSG msg;

        // ó���� �޽����� �� �̻� ������ ���� ����
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // Ű �Է� �޽����� ����
            TranslateMessage(&msg);

            // �޽����� ������ ������ ���ν����� ����, �޽����� ������ ����� WndProc ���� ���޵�
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }

        // ���콺 �Է� ó��
        mouseInput.Frame();
        int mouseDeltaX, mouseDeltaY;
        mouseInput.GetMouseDelta(mouseDeltaX, mouseDeltaY);
		UCamera::MainCamera->AddCameraHorizontalAngle(mouseDeltaX * 0.1f);
		UCamera::MainCamera->AddCameraVerticalAngle(-mouseDeltaY * 0.1f);

        UBall* curBall = ballSpawner->HeadBall();
        while (curBall != nullptr) {
            curBall->FrameUpdate(deltaTime, ballSpawner, v);
            curBall = curBall->nextBall;
        }
        gravityball.Gravity(ballSpawner, deltaTime);

		//UAudioManager::MainAudioManger.Update(targetFrameTime);

        ////////////////////////////////////////////
        // �Ź� ����Ǵ� �ڵ带 ���⿡ �߰��մϴ�.
        // �غ� �۾�
        if (frameAccumulatedTime >= targetFrameTime) {
            level->AllRender(0);

            ballSpawner->UpdateBallCount();

            UBall* curBall = ballSpawner->HeadBall();

            // ImGui Frame ����
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // ���� ImGui UI ��Ʈ�� �߰��� ImGui::NewFrame()�� ImGui::Render() ������ ���⿡ ��ġ�մϴ�.

            ImGui::Begin("Jungle Property Window");
            ImGui::Text("Hello Jungle World!");
            ImGui::Text("%f", UCamera::MainCamera->CameraHorizontalAngle());
            ImGui::Text("%f", UCamera::MainCamera->CameraVerticalAngle());

            for (std::pair<int, int> x : v)
            {
                ImGui::Text("(%d, %d)\n", x.first, x.second);
            }
            //ImGui::Text("(%d, %d)\n", 4, 5);


            ImGui::Checkbox("Gravity", &bIsGravity);
            ImGui::Checkbox("Magnetic", &bIsMagnetic);
            ImGui::Checkbox("Rotate", &bIsRotate);
            //ImGui::Text("ballcount : %d", ballSpawner.ListCount());
            ImGui::Text("%f", deltaTime);
            ImGui::Text("game over: %d", UGameManager::getInstance()->IsGameOver());
            bool temp;
            ImGui::Checkbox("Projection", &temp);
            temp ? UCamera::MainCamera->SetProjectionType(ECameraProjectionType::Perspective) : UCamera::MainCamera->SetProjectionType(ECameraProjectionType::Orthographic);
            ImGui::InputInt("Number of Balls", &ballSpawner->BallCount);
            if (ImGui::Button("Quit this app"))
            {
                // ���� �����쿡 Quit �޽����� �޽��� ť�� ����
                PostMessage(hWnd, WM_QUIT, 0, 0);
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // UUIManager�� ���� �߰� UI �ռ� (��: ScoreBoard, �̹��� ��)
            UUIManager::MainUUIManger->RenderText(UGameManager::getInstance()->GetScore());
            UUIManager::MainUUIManger->RenderImage();
            if(UGameManager::getInstance()->IsGameOver())
                UUIManager::MainUUIManger->RenderGameOverText();
            ///������ render


            gravityball.Update();

            for (int i = 0; i < ballSpawner->ListCount(); i++) {
                if (curBall == nullptr) break;

                curBall->UpdateBalls();
                curBall = curBall->nextBall;
            }

            translucentAerial.Render();

            // �� �׷����� ���۸� ��ȯ
            URenderer::MainRenderer->SwapBuffer();

            frameAccumulatedTime -= targetFrameTime;
            ////////////////////////////////////////////
        }
    }
	// �Ҹ��ϴ� �ڵ带 ���⿡ �߰��մϴ�.
    // ���⿡�� ImGui �Ҹ�
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    // ���ؽ� ���� �Ҹ��� Renderer �Ҹ����� ó���մϴ�.
    URenderer::MainRenderer->ReleaseVertexBuffer(vertexBufferSphere);
    // ������ �Ҹ� ������ ���̴��� �Ҹ� ��Ű�� �Լ��� ȣ���մϴ�.
    URenderer::MainRenderer->ReleaseConstantBuffer();
    URenderer::MainRenderer->ReleaseShader();
    URenderer::MainRenderer->Release();

    _CrtDumpMemoryLeaks();


    //UUIManager::MainUUIManger->Release();
    //delete UUIManager::MainUUIManger;

	return 1;
}