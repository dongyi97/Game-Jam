#pragma once

// D3D ��뿡 �ʿ��� ������ϵ��� �����մϴ�.
#include <d3d11.h>
#include <d3dcompiler.h>

// D3D ��뿡 �ʿ��� ���̺귯������ ��ũ�մϴ�.
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
        FVector3 LightDirection; // ���� ����
        float Padding; // ������ ���߱� ���� �е�
        FVector3 LightColor; // ���� ����
    };

    // Direct3D 11 ��ġ(Device)�� ��ġ ���ؽ�Ʈ(Device Context) �� ���� ü��(Swap Chain)�� �����ϱ� ���� �����͵�
    ID3D11Device* Device = nullptr; // GPU�� ����ϱ� ���� Direct3D ��ġ
    ID3D11DeviceContext* DeviceContext = nullptr; // GPU ��� ������ ����ϴ� ���ؽ�Ʈ
    IDXGISwapChain* SwapChain = nullptr; // ������ ���۸� ��ü�ϴ� �� ���Ǵ� ���� ü��
    // �������� �ʿ��� ���ҽ� �� ���¸� �����ϱ� ���� ������
    ID3D11Texture2D* FrameBuffer = nullptr; // ȭ�� ��¿� �ؽ�ó
    ID3D11RenderTargetView* FrameBufferRTV = nullptr; // �ؽ�ó�� ���� Ÿ������ ����ϴ� ��
    ID3D11RasterizerState* RasterizerState = nullptr; // �����Ͷ����� ����(�ø�, ä��� ��� �� ����)
    ID3D11Buffer* VertexConstantBuffer = nullptr; // ���̴��� vertex �����͸� �����ϱ� ���� ��� ����
    ID3D11Buffer* TransformConstantBuffer = nullptr; // ���̴��� transform �����͸� �����ϱ� ���� ��� ����
    ID3D11Buffer* LightConstantBuffer = nullptr; // ���̴��� ���� ������ �����ϱ� ���� ��� ����

	ID3D11BlendState* AlphaBlendState = nullptr; // ���� ����  (���� ���� �� ����)

    FLOAT ClearColor[4] = { 0.f, 0.f, 0.f, 1.0f }; // ȭ���� �ʱ�ȭ(clear)�� �� ����� ���� (RGBA)
    D3D11_VIEWPORT ViewportInfo; // ������ ������ �����ϴ� ����Ʈ ����

    ID3D11DepthStencilView* DepthStencilView;
    ID3D11Texture2D* DepthStencilBuffer;
    ID3D11DepthStencilState* DepthStencilState;

    ID3D11VertexShader* SimpleVertexShader;
    ID3D11PixelShader* SimplePixelShader;
    ID3D11InputLayout* SimpleInputLayout;
    unsigned int Stride;

public:
    // ������ �ʱ�ȭ �Լ�
    void Create(HWND hWindow)
    {
        // Direct3D ��ġ �� ���� ü�� ����
        CreateDeviceAndSwapChain(hWindow);

        // ������ ���� ����
        CreateFrameBuffer();

        // ���� ���ٽ� ���� ����
        CreateStencilBuffer();

        // �����Ͷ����� ���� ����
        CreateRasterizerState();

        // ���� ���� ���� (�߰�)
        CreateBlendState();

        // ���� ���ٽ� ���� �� ���� ���´� �� �ڵ忡���� �ٷ��� ����
    }

    // ���� ���� ���� �Լ� �߰�
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

    // ���� ���� ���� �Լ� �߰�
    void ReleaseBlendState()
    {
        if (AlphaBlendState)
        {
            AlphaBlendState->Release();
            AlphaBlendState = nullptr;
        }
    }

    // Direct3D ��ġ �� ���� ü���� �����ϴ� �Լ�
    void CreateDeviceAndSwapChain(HWND hWindow)
    {
        // �����ϴ� Direct3D ��� ������ ����
        D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

        // ���� ü�� ���� ����ü �ʱ�ȭ
        DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
        swapchaindesc.BufferDesc.Width = 0; // â ũ�⿡ �°� �ڵ����� ����
        swapchaindesc.BufferDesc.Height = 0; // â ũ�⿡ �°� �ڵ����� ����
        swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // ���� ����
        swapchaindesc.SampleDesc.Count = 1; // ��Ƽ ���ø� ��Ȱ��ȭ
        swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // ���� Ÿ������ ���
        swapchaindesc.BufferCount = 2; // ���� ���۸�
        swapchaindesc.OutputWindow = hWindow; // �������� â �ڵ�
        swapchaindesc.Windowed = TRUE; // â ���
        swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // ���� ���
        


        // Direct3D ��ġ�� ���� ü���� ����
        HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
            &swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

        // ������ ���� ü���� ���� ��������
        SwapChain->GetDesc(&swapchaindesc);

        // ����Ʈ ���� ����
        ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
    }

    // Direct3D ��ġ �� ���� ü���� �����ϴ� �Լ�
    void ReleaseDeviceAndSwapChain()
    {
        if (DeviceContext)
        {
            DeviceContext->Flush(); // �����ִ� GPU ��� ����
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

    // ������ ���۸� �����ϴ� �Լ�
    void CreateFrameBuffer()
    {
        // ���� ü�����κ��� �� ���� �ؽ�ó ��������
        SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

        // ���� Ÿ�� �� ����
        D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
        framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // ���� ����
        framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D �ؽ�ó

        Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
    }

    // ������ ���۸� �����ϴ� �Լ�
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

    // �����Ͷ����� ���¸� �����ϴ� �Լ�
    void CreateRasterizerState()
    {
        D3D11_RASTERIZER_DESC rasterizerdesc = {};
        rasterizerdesc.FillMode = D3D11_FILL_SOLID; // ä��� ���
        rasterizerdesc.CullMode = D3D11_CULL_BACK; // �� ���̽� �ø�

        Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
    }

    // �����Ͷ����� ���¸� �����ϴ� �Լ�
    void ReleaseRasterizerState()
    {
        if (RasterizerState)
        {
            RasterizerState->Release();
            RasterizerState = nullptr;
        }
    }

    // �������� ���� ��� ���ҽ��� �����ϴ� �Լ�
    void Release()
    {
        RasterizerState->Release();

        // ���� Ÿ���� �ʱ�ȭ
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
            MsgBoxAssert("���ؽ� ���� ������ �����߽��ϴ�.");
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
        // ���� ���ٽ� ���� ���� ����
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

        // ���� ���ٽ� �� ����
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
        depthStencilViewDesc.Format = depthStencilDesc.Format;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        Device->CreateDepthStencilView(DepthStencilBuffer, &depthStencilViewDesc, &DepthStencilView);

        // ���� ���ٽ� ���� ����
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
        // ����Ʈ ����ü ����

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        SwapChain->GetDesc(&swapChainDesc);

        ViewportInfo.TopLeftX = swapChainDesc.BufferDesc.Width * topLeftX;    // ȭ���� ���� ��� X ��ǥ
        ViewportInfo.TopLeftY = swapChainDesc.BufferDesc.Height * topLeftY;    // ȭ���� ���� ��� Y ��ǥ
        ViewportInfo.Width = swapChainDesc.BufferDesc.Width * width;          // ����Ʈ �ʺ�
        ViewportInfo.Height = swapChainDesc.BufferDesc.Height * height;        // ����Ʈ ����
        ViewportInfo.MinDepth = 0.0f;        // �ּ� ���� ��
        ViewportInfo.MaxDepth = 1.0f;        // �ִ� ���� ��

        // ����Ʈ ����
        DeviceContext->RSSetViewports(1, &ViewportInfo);
    }

    void PrepareShader()
    {
        DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
        DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
        DeviceContext->IASetInputLayout(SimpleInputLayout);

        // ���̴��� ��� ���� ���ε�
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

        // nullptr ��� ������ ���� ���� ���
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
        // ��� ���� ������Ʈ
        D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

        //���� ����
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

    // ���� ü���� �� ���ۿ� ����Ʈ ���۸� ��ü�Ͽ� ȭ�鿡 ���
    void SwapBuffer()
    {
        SwapChain->Present(1, 0); // 1: VSync Ȱ��ȭ
    }


};

