#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"

// D3D 사용에 필요한 헤더파일들을 포함합니다.
#include <d3d11.h>
#include <d3dcompiler.h>

// D3D 사용에 필요한 라이브러리들을 링크합니다.
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

// 행성 타입
//enum Type
//{
//    MARS,
//    EARTH,
//    JUPITER,
//};
bool isShooting = false;

void UpdateCameraOrbitWithIrregularTilt(UCamera* inCamera, float deltaTime)
{
    // 중심은 gravityball의 위치 (0,0,0)
    FVector3 center = FVector3();
    const float orbitRadius = 5.0f;
    
    // 원운동을 위한 theta: xy평면(또는 원하는 평면)에서의 회전각
    static float theta = -3.14159265f / 2.0f;  // 초기값: (0, -5, 0)에 해당하는 각도
    const float thetaSpeed = 2.f;  // 초당 회전 속도 (라디안)
    //theta += thetaSpeed * deltaTime;
    
    // φ의 불규칙한 움직임을 위한 시간 누적 변수
    static float phiTime = 0.0f;
    phiTime += deltaTime;
    
    // φ를 결정하는 파라미터들
    const float phiFrequency = 0.2f; // 기본 주파수
    const float phiMax = 0.5f;       // 최대 tilt 각도 (라디안)
    
    // 단순 sin 대신 sin과 cos의 조합을 사용해서 불규칙한 움직임 부여
    float phi = phiMax * (0.6f * sinf(phiFrequency * phiTime) + 0.3f * cosf(1.5f * phiTime));
    
    // 구면 좌표계로부터 카메라의 Cartesian 좌표 계산
    // x = r * cos(φ) * cos(θ)
    // y = r * cos(φ) * sin(θ)
    // z = r * sin(φ)
    float x = orbitRadius * cosf(phi) * cosf(theta);
    float y = orbitRadius * cosf(phi) * sinf(theta);
    float z = orbitRadius * sinf(phi);
    
    // 카메라 위치와 타깃 업데이트
    inCamera->SetPosition(center + FVector3(x, y, z));
    inCamera->SetTarget(center);
}



 //main.cpp 내 이벤트 등록 예시
void RegisterInputEvents(UCamera* inCamera, UBallSpawner* ballSpawner) {
    //const float orbitRadius = 10.0f;      // 카메라와 gravityball 사이의 거리

    //const float angleDelta = 0.1f;      // 회전 각도 (라디안 단위)

    //// A키: z축 기준으로 +angleDelta 만큼 회전 (좌측 회전)
    //InputManager::RegisterEvent("OnLeftArrow", [inCamera, orbitRadius, angleDelta]() {
    //    FVector3 center = FVector3();
    //    FVector3 camPos = inCamera->GetPosition();
    //    FVector3 offset = camPos - center;  // 현재 오프셋 (예: (0, -5, 0))

    //    float cosTheta = cosf(angleDelta);
    //    float sinTheta = sinf(angleDelta);
    //    FVector3 newOffset;
    //    newOffset.x = offset.x * cosTheta - offset.y * sinTheta;
    //    newOffset.y = offset.x * sinTheta + offset.y * cosTheta;
    //    newOffset.z = 0.0f;  // xy평면에서는 z값은 0

    //    // 오프셋의 길이를 orbitRadius로 보정 (부동소수점 오차 보정)
    //    float length = sqrtf(newOffset.x * newOffset.x + newOffset.y * newOffset.y);
    //    if (length != 0.0f) {
    //        newOffset.x = newOffset.x / length * orbitRadius;
    //        newOffset.y = newOffset.y / length * orbitRadius;
    //    }
    //    inCamera->SetPosition(center + newOffset);
    //    inCamera->SetTarget(center);  // 항상 gravityball을 바라보도록
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


// 각종 메시지를 처리할 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return InputManager::WndProc(hWnd, message, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
     
	//_CrtSetBreakAlloc(291); // 디버깅할 메모리 블록 번호
    // 
    // 
    // 각종 생성하는 코드를 여기에 추가합니다.
	// 윈도우 클래스 이름
	WCHAR WindowClass[] = L"JungleWindowClass";

	// 윈도우 타이틀바에 표시될 이름
	WCHAR Title[] = L"Game Tech Lab";

	// 각종 메시지를 처리할 함수인 WndProc의 함수 포인터를 WindowClass 구조체에 넣는다.
	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

	// 윈도우 클래스 등록
	RegisterClassW(&wndclass);

	// 1024 x 1024 크기에 윈도우 생성
	HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024,
		nullptr, nullptr, hInstance, nullptr);

	FWindowManager::SetHWND(hWnd);

	FCore::Start(hWnd);
    UTimeManager::Init();

	ULevel* level = new ULevel();
    level->Start();

    // 테스트를 위해 전역으로 작성했었음
    MouseInput mouseInput;

    mouseInput.Initialize(hInstance, FWindowManager::GetHWND(), 1024, 1024);


    // 여기에서 ImGui를 생성합니다.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init((void*)hWnd);
    ImGui_ImplDX11_Init(URenderer::MainRenderer->Device, URenderer::MainRenderer->DeviceContext);

    // Renderer와 Shader 생성 이후에 버텍스 버퍼를 생성합니다.

    std::pair<std::vector<FVertexSimple>, std::vector<unsigned int>> Sphere = FMath::CreateSphereWithIndices(1.0f, 32,FVector3(0.f,0.8f,0.6f) );

    numVerticesSphere = sizeof(FVertexSimple) * Sphere.first.size();

    vertexBufferSphere = URenderer::MainRenderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
    //vertexBufferSphere = URenderer::MainRenderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));

    vertexBufferSphereTrans = URenderer::MainRenderer->CreateVertexBuffer(&Sphere.first[0], sizeof(FVertexSimple) * Sphere.first.size());
    indexBufferSphere = URenderer::MainRenderer->CreateIndexBuffer(&Sphere.second[0], sizeof(unsigned int) * Sphere.second.size());

    // FPS 제한을 위한 설정
    const int targetFPS = 30;
    const double targetFrameTime = 1.0 / targetFPS; // 한 프레임의 목표 시간 (밀리초 단위)
    double frameAccumulatedTime = 0.0f;

    // 고성능 타이머 초기화
    LARGE_INTEGER frequency;
    LARGE_INTEGER currentTime, previousTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&previousTime);

    // ----- Variables
    bool bIsExit = false;

    UBallSpawner* ballSpawner = new UBallSpawner();

    RegisterInputEvents(UCamera::MainCamera, ballSpawner);

    //GravityBall 생성
    GravityBall gravityball;

	UTranslucentAerial translucentAerial;

	// Main Loop (Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨)
    while (bIsExit == false)
    {
        QueryPerformanceCounter(&currentTime);

        // 델타 타임 계산 (초 단위)
        double deltaTime = static_cast<double>(currentTime.QuadPart - previousTime.QuadPart) / frequency.QuadPart;
        previousTime = currentTime;

        // 시간 누적
        frameAccumulatedTime += deltaTime;
        UTimeManager::AddDeltaTime(deltaTime);

        UpdateCameraOrbitWithIrregularTilt(UCamera::MainCamera, static_cast<float>(deltaTime));

        MSG msg;

        // 처리할 메시지가 더 이상 없을때 까지 수행
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 키 입력 메시지를 번역
            TranslateMessage(&msg);

            // 메시지를 적절한 윈도우 프로시저에 전달, 메시지가 위에서 등록한 WndProc 으로 전달됨
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }

        // 마우스 입력 처리
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
        // 매번 실행되는 코드를 여기에 추가합니다.
        // 준비 작업
        if (frameAccumulatedTime >= targetFrameTime) {
            level->AllRender(0);

            ballSpawner->UpdateBallCount();

            UBall* curBall = ballSpawner->HeadBall();

            // ImGui Frame 생성
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // 이후 ImGui UI 컨트롤 추가는 ImGui::NewFrame()과 ImGui::Render() 사이인 여기에 위치합니다.

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
                // 현재 윈도우에 Quit 메시지를 메시지 큐로 보냄
                PostMessage(hWnd, WM_QUIT, 0, 0);
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // UUIManager를 통한 추가 UI 합성 (예: ScoreBoard, 이미지 등)
            UUIManager::MainUUIManger->RenderText(UGameManager::getInstance()->GetScore());
            UUIManager::MainUUIManger->RenderImage();
            if(UGameManager::getInstance()->IsGameOver())
                UUIManager::MainUUIManger->RenderGameOverText();
            ///렌더링 render


            gravityball.Update();

            for (int i = 0; i < ballSpawner->ListCount(); i++) {
                if (curBall == nullptr) break;

                curBall->UpdateBalls();
                curBall = curBall->nextBall;
            }

            translucentAerial.Render();

            // 다 그렸으면 버퍼를 교환
            URenderer::MainRenderer->SwapBuffer();

            frameAccumulatedTime -= targetFrameTime;
            ////////////////////////////////////////////
        }
    }
	// 소멸하는 코드를 여기에 추가합니다.
    // 여기에서 ImGui 소멸
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    // 버텍스 버퍼 소멸은 Renderer 소멸전에 처리합니다.
    URenderer::MainRenderer->ReleaseVertexBuffer(vertexBufferSphere);
    // 렌더러 소멸 직전에 쉐이더를 소멸 시키는 함수를 호출합니다.
    URenderer::MainRenderer->ReleaseConstantBuffer();
    URenderer::MainRenderer->ReleaseShader();
    URenderer::MainRenderer->Release();

    _CrtDumpMemoryLeaks();


    //UUIManager::MainUUIManger->Release();
    //delete UUIManager::MainUUIManger;

	return 1;
}