#pragma once

// D3D 사용에 필요한 헤더파일들을 포함합니다.
#include <d3d11.h>
#include <d3dcompiler.h>

// D3D 사용에 필요한 라이브러리들을 링크합니다.
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include "FMath.h"
#include "UObject.h"
#include "DirectXMath.h"

#include "string"
#include "UVariables.h"

const float DEG_TO_RAD = PI / 180;


#define MsgBoxAssert(Text) \
std::string Value = Text; \
MessageBoxA(nullptr, Value.c_str(), "Error", MB_OK); assert(false);

class UCamera;

class URenderer
{
public:
    static URenderer* MainRenderer;

    //static URenderer* UIRenderer;

    void SetCamera(UCamera* _camera) { camera = _camera; }

private:
    UCamera* camera;


public:
    struct FVertexConstants
    {
        FVector3 Offset;
        float OffsetPad;
        FVector3 Scale;
        float ScalePad;
        FVector3 Color;
        float ColorPad;
        FVector3 Rotate;
        float RotatePad;
        int Trans = 0;
    };

    struct FTransformConstants {
        DirectX::XMMATRIX worldViewProj;
    };

    struct FLightConstants {
        FVector3 LightDirection; // 빛의 방향
        float Padding; // 정렬을 맞추기 위한 패딩
        FVector3 LightColor; // 빛의 색상
    };

    // Direct3D 11 장치(Device)와 장치 컨텍스트(Device Context) 및 스왑 체인(Swap Chain)을 관리하기 위한 포인터들
    ID3D11Device* Device = nullptr; // GPU와 통신하기 위한 Direct3D 장치
    ID3D11DeviceContext* DeviceContext = nullptr; // GPU 명령 실행을 담당하는 컨텍스트
    IDXGISwapChain* SwapChain = nullptr; // 프레임 버퍼를 교체하는 데 사용되는 스왑 체인
    // 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
    ID3D11Texture2D* FrameBuffer = nullptr; // 화면 출력용 텍스처
    ID3D11RenderTargetView* FrameBufferRTV = nullptr; // 텍스처를 렌더 타겟으로 사용하는 뷰
    ID3D11RasterizerState* RasterizerState = nullptr; // 래스터라이저 상태(컬링, 채우기 모드 등 정의)
    ID3D11Buffer* VertexConstantBuffer = nullptr; // 쉐이더에 vertex 데이터를 전달하기 위한 상수 버퍼
    ID3D11Buffer* TransformConstantBuffer = nullptr; // 쉐이더에 transform 데이터를 전달하기 위한 상수 버퍼
    ID3D11Buffer* LightConstantBuffer = nullptr; // 쉐이더에 광원 정보를 전달하기 위한 상수 버퍼

	ID3D11BlendState* AlphaBlendState = nullptr; // 블렌더 상태  (알파 블렌딩 등 정의)

    FLOAT ClearColor[4] = { 0.f, 0.f, 0.f, 1.0f }; // 화면을 초기화(clear)할 때 사용할 색상 (RGBA)
    D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보

    ID3D11DepthStencilView* DepthStencilView;
    ID3D11Texture2D* DepthStencilBuffer;
    ID3D11DepthStencilState* DepthStencilState;

    ID3D11VertexShader* SimpleVertexShader;
    ID3D11PixelShader* SimplePixelShader;
    ID3D11InputLayout* SimpleInputLayout;
    unsigned int Stride;

public:
    // 렌더러 초기화 함수
    void Create(HWND hWindow)
    {
        // Direct3D 장치 및 스왑 체인 생성
        CreateDeviceAndSwapChain(hWindow);

        // 프레임 버퍼 생성
        CreateFrameBuffer();

        // 깊이 스텐실 버퍼 생성
        CreateStencilBuffer();

        // 래스터라이저 상태 생성
        CreateRasterizerState();

        // 블렌드 상태 생성 (추가)
        CreateBlendState();

        // 깊이 스텐실 버퍼 및 블렌드 상태는 이 코드에서는 다루지 않음
    }

    // 블렌드 상태 생성 함수 추가
    void CreateBlendState()
    {
        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;

        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        Device->CreateBlendState(&blendDesc, &AlphaBlendState);
    }

    // 블렌드 상태 해제 함수 추가
    void ReleaseBlendState()
    {
        if (AlphaBlendState)
        {
            AlphaBlendState->Release();
            AlphaBlendState = nullptr;
        }
    }

    // Direct3D 장치 및 스왑 체인을 생성하는 함수
    void CreateDeviceAndSwapChain(HWND hWindow)
    {
        // 지원하는 Direct3D 기능 레벨을 정의
        D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

        // 스왑 체인 설정 구조체 초기화
        DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
        swapchaindesc.BufferDesc.Width = 0; // 창 크기에 맞게 자동으로 설정
        swapchaindesc.BufferDesc.Height = 0; // 창 크기에 맞게 자동으로 설정
        swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 색상 포맷
        swapchaindesc.SampleDesc.Count = 1; // 멀티 샘플링 비활성화
        swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 렌더 타겟으로 사용
        swapchaindesc.BufferCount = 2; // 더블 버퍼링
        swapchaindesc.OutputWindow = hWindow; // 렌더링할 창 핸들
        swapchaindesc.Windowed = TRUE; // 창 모드
        swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스왑 방식
        


        // Direct3D 장치와 스왑 체인을 생성
        HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
            &swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

        // 생성된 스왑 체인의 정보 가져오기
        SwapChain->GetDesc(&swapchaindesc);

        // 뷰포트 정보 설정
        ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
    }

    // Direct3D 장치 및 스왑 체인을 해제하는 함수
    void ReleaseDeviceAndSwapChain()
    {
        if (DeviceContext)
        {
            DeviceContext->Flush(); // 남아있는 GPU 명령 실행
        }

        if (SwapChain)
        {
            SwapChain->Release();
            SwapChain = nullptr;
        }

        if (Device)
        {
            Device->Release();
            Device = nullptr;
        }

        if (DeviceContext)
        {
            DeviceContext->Release();
            DeviceContext = nullptr;
        }
    }

    // 프레임 버퍼를 생성하는 함수
    void CreateFrameBuffer()
    {
        // 스왑 체인으로부터 백 버퍼 텍스처 가져오기
        SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

        // 렌더 타겟 뷰 생성
        D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
        framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // 색상 포맷
        framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

        Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
    }

    // 프레임 버퍼를 해제하는 함수
    void ReleaseFrameBuffer()
    {
        if (FrameBuffer)
        {
            FrameBuffer->Release();
            FrameBuffer = nullptr;
        }

        if (FrameBufferRTV)
        {
            FrameBufferRTV->Release();
            FrameBufferRTV = nullptr;
        }
    }

    // 래스터라이저 상태를 생성하는 함수
    void CreateRasterizerState()
    {
        D3D11_RASTERIZER_DESC rasterizerdesc = {};
        rasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
        rasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링

        Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
    }

    // 래스터라이저 상태를 해제하는 함수
    void ReleaseRasterizerState()
    {
        if (RasterizerState)
        {
            RasterizerState->Release();
            RasterizerState = nullptr;
        }
    }

    // 렌더러에 사용된 모든 리소스를 해제하는 함수
    void Release()
    {
        RasterizerState->Release();

        // 렌더 타겟을 초기화
        DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

        ReleaseFrameBuffer();
        ReleaseDeviceAndSwapChain();

        ReleaseBlendState();
    }

    void CreateShader()
    {
        ID3DBlob* vertexshaderCSO;
        ID3DBlob* pixelshaderCSO;

        D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

        Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

        D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

        Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

        Stride = sizeof(FVertexSimple);

        vertexshaderCSO->Release();
        pixelshaderCSO->Release();
    }

    void ReleaseShader()
    {
        if (SimpleInputLayout)
        {
            SimpleInputLayout->Release();
            SimpleInputLayout = nullptr;
        }

        if (SimplePixelShader)
        {
            SimplePixelShader->Release();
            SimplePixelShader = nullptr;
        }

        if (SimpleVertexShader)
        {
            SimpleVertexShader->Release();
            SimpleVertexShader = nullptr;
        }
    }
    ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth)
    {
        // 2. Create a vertex buffer
        D3D11_BUFFER_DESC vertexbufferdesc = {};
        vertexbufferdesc.ByteWidth = byteWidth;
        vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
        vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

        ID3D11Buffer* vertexBuffer;

        Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

        return vertexBuffer;
    }

    ID3D11Buffer* CreateIndexBuffer(const void* _Data, UINT byteWidth)
    {
        // 2. Create a vertex buffer
        D3D11_BUFFER_DESC vertexbufferdesc = {};
        vertexbufferdesc.ByteWidth = byteWidth;
        vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT; // will never be updated 
        vertexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA IndexbufferSRD = { _Data };

        ID3D11Buffer* IndexBuffer;

        // Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

        if (S_OK != Device->CreateBuffer(&vertexbufferdesc, &IndexbufferSRD, &IndexBuffer))
        {
            MsgBoxAssert("버텍스 버퍼 생성에 실패했습니다.");
            return nullptr;
        }

        return IndexBuffer;
    }

    void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer)
    {
        vertexBuffer->Release();
    }

    void CreateConstantBuffer()
    {
        D3D11_BUFFER_DESC vertexconstantbufferdesc = {};
        vertexconstantbufferdesc.ByteWidth = sizeof(FVertexConstants) + 0xf & 0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
        vertexconstantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
        vertexconstantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vertexconstantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        Device->CreateBuffer(&vertexconstantbufferdesc, nullptr, &VertexConstantBuffer);

        D3D11_BUFFER_DESC transformconstantbufferdesc = {};
        transformconstantbufferdesc.ByteWidth = sizeof(FTransformConstants) + 0xf & 0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
        transformconstantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
        transformconstantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        transformconstantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        Device->CreateBuffer(&transformconstantbufferdesc, nullptr, &TransformConstantBuffer);

        D3D11_BUFFER_DESC lightconstantbufferdesc = {};
        lightconstantbufferdesc.ByteWidth = sizeof(FLightConstants) + 0xf & 0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
        lightconstantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
        lightconstantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        lightconstantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        Device->CreateBuffer(&lightconstantbufferdesc, nullptr, &LightConstantBuffer);
    }

    void ReleaseConstantBuffer()
    {
        if (VertexConstantBuffer)
        {
            VertexConstantBuffer->Release();
            VertexConstantBuffer = nullptr;
        }

        if (TransformConstantBuffer)
        {
            TransformConstantBuffer->Release();
            TransformConstantBuffer = nullptr;
        }

        if (LightConstantBuffer) {
            LightConstantBuffer->Release();
            LightConstantBuffer = nullptr;
        }
    }

    void CreateStencilBuffer() {
        // 깊이 스텐실 버퍼 설명 설정
        D3D11_TEXTURE2D_DESC depthStencilDesc;
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        SwapChain->GetDesc(&swapChainDesc);
        depthStencilDesc.Width = swapChainDesc.BufferDesc.Width;
        depthStencilDesc.Height = swapChainDesc.BufferDesc.Height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;

        Device->CreateTexture2D(&depthStencilDesc, nullptr, &DepthStencilBuffer);

        // 깊이 스텐실 뷰 생성
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
        depthStencilViewDesc.Format = depthStencilDesc.Format;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        Device->CreateDepthStencilView(DepthStencilBuffer, &depthStencilViewDesc, &DepthStencilView);

        // 깊이 스텐실 상태 설정
        D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
        depthStencilStateDesc.DepthEnable = TRUE;
        depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilStateDesc.StencilEnable = FALSE;

        Device->CreateDepthStencilState(&depthStencilStateDesc, &DepthStencilState);
    }

    void ReleaseStencilBuffer() {
        if (DepthStencilView) {
            DepthStencilView->Release();
            DepthStencilView = nullptr;
        }
        if (DepthStencilBuffer) {
            DepthStencilBuffer->Release();
            DepthStencilBuffer = nullptr;
        }
        if (DepthStencilState) {
            DepthStencilState->Release();
            DepthStencilState = nullptr;
        }
    }

    void SetCustomViewport(float topLeftX, float topLeftY, float width, float height) {
        // 뷰포트 구조체 설정

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        SwapChain->GetDesc(&swapChainDesc);

        ViewportInfo.TopLeftX = swapChainDesc.BufferDesc.Width * topLeftX;    // 화면의 왼쪽 상단 X 좌표
        ViewportInfo.TopLeftY = swapChainDesc.BufferDesc.Height * topLeftY;    // 화면의 왼쪽 상단 Y 좌표
        ViewportInfo.Width = swapChainDesc.BufferDesc.Width * width;          // 뷰포트 너비
        ViewportInfo.Height = swapChainDesc.BufferDesc.Height * height;        // 뷰포트 높이
        ViewportInfo.MinDepth = 0.0f;        // 최소 깊이 값
        ViewportInfo.MaxDepth = 1.0f;        // 최대 깊이 값

        // 뷰포트 설정
        DeviceContext->RSSetViewports(1, &ViewportInfo);
    }

    void PrepareShader()
    {
        DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
        DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
        DeviceContext->IASetInputLayout(SimpleInputLayout);

        // 셰이더에 상수 버퍼 바인딩
        DeviceContext->VSSetConstantBuffers(0, 1, &VertexConstantBuffer);
        DeviceContext->VSSetConstantBuffers(1, 1, &TransformConstantBuffer);
        DeviceContext->PSSetConstantBuffers(2, 1, &LightConstantBuffer);

        DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);
        DeviceContext->IASetInputLayout(SimpleInputLayout);
    }

    void Prepare()
    {
        DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);
        DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        DeviceContext->RSSetViewports(1, &ViewportInfo);
        DeviceContext->RSSetState(RasterizerState);

        DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);

        // nullptr 대신 생성한 블렌드 상태 사용
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        DeviceContext->OMSetBlendState(AlphaBlendState, blendFactor, 0xffffffff);
    }

    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices)
    {
        UINT offset = 0;
        DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);

        DeviceContext->Draw(numVertices, 0);
    }

    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices, ID3D11Buffer* pindexBuffer)
    {
        UINT offset = 0;
        DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);
        UINT Indexoffset = 0;

		DeviceContext->IASetIndexBuffer(pindexBuffer, DXGI_FORMAT_R32_UINT, Indexoffset);
        DeviceContext->DrawIndexed(numVertices, 0, 0);
    }

    void UpdateConstant(FVector3 Scale, FVector3 Offset, FVector3 Color, FVector3 Rotate, bool Trans = false)
    {
        if (VertexConstantBuffer)
        {
            D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

            DeviceContext->Map(VertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
            FVertexConstants* constants = (FVertexConstants*)constantbufferMSR.pData;
            {
                constants->Offset = Offset;
                constants->Scale = Scale;
                constants->Color = Color;
               // constants->Color = -1;
                constants->Rotate = Rotate;
                constants->Trans = static_cast<int>(Trans);
            }
            DeviceContext->Unmap(VertexConstantBuffer, 0);
        }
    }

    void UpdateLight(float time) {
        // 상수 버퍼 업데이트
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

        //해의 궤적
        float sunX = 10.0 * std::cos(PI / 12.0 * time - PI / 2.0);
        float sunY = 10.0 * std::sin(PI / 12.0 * time);
        float sunZ = -10.0 * std::cos(PI / 12.0 * time);

        DeviceContext->Map(LightConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
        FLightConstants* constants = (FLightConstants*)constantbufferMSR.pData;
        {
            constants->LightDirection = FVector3(sunX, sunY, sunZ);
            constants->LightColor = FVector3(0.8f, 0.3f, 0.0f);
        }
        DeviceContext->Unmap(LightConstantBuffer, 0);
    }

    // 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
    void SwapBuffer()
    {
        SwapChain->Present(1, 0); // 1: VSync 활성화
    }


};

