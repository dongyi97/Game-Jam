#include "UUIManager.h"
#include "URenderer.h"  // URenderer 헤더 포함
#include <wincodec.h>
#include <wchar.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

UUIManager* UUIManager::MainUUIManger = nullptr;


HRESULT UUIManager::Initialize(HWND hWnd)
{
    HRESULT hr = S_OK;

    // Direct2D Factory 생성
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, pD2DFactory.GetAddressOf());
    if (FAILED(hr))
        return hr;

    // Direct3D의 스왑 체인 백 버퍼(IdxgiSurface)를 얻어옴
    IDXGISurface* dxgiSurface = nullptr;
    hr = URenderer::MainRenderer->SwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&dxgiSurface);
    if (FAILED(hr))
        return hr;

    // DXGI surface를 이용해 Direct2D RenderTarget 생성
    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0);
    hr = pD2DFactory->CreateDxgiSurfaceRenderTarget(dxgiSurface, &rtProps, &pRenderTarget);
    dxgiSurface->Release();
    if (FAILED(hr))
        return hr;

    // SolidColorBrush 생성
    hr = pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &pTextBrush);
    if (FAILED(hr))
        return hr;

    // DirectWrite Factory 생성
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(pDWriteFactory.GetAddressOf()));
    if (FAILED(hr))
        return hr;

    // 텍스트 포맷 생성
    hr = pDWriteFactory->CreateTextFormat(
        L"Arial",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        32.0f,
        L"en-us",
        &pTextFormat);
    if (FAILED(hr))
        return hr;
    
    // 텍스트를 렌더 사각형 가운데 정렬
    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    // 이미지 로드
    hr = LoadBallImage(L"sampleimage.png");
    if (FAILED(hr))
        return hr;

    return hr;
}

HRESULT UUIManager::LoadBallImage(const wchar_t* imagePath)
{
    HRESULT hr = S_OK;

    // WIC를 이용해 png 파일을 로드한 후 Direct2D 비트맵으로 변환
    Microsoft::WRL::ComPtr<IWICImagingFactory> pWICFactory;
    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pWICFactory));
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
    hr = pWICFactory->CreateDecoderFromFilename(
        imagePath,
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder);
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pFrame;
    hr = pDecoder->GetFrame(0, &pFrame);
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter;
    hr = pWICFactory->CreateFormatConverter(&pConverter);
    if (FAILED(hr))
        return hr;

    hr = pConverter->Initialize(
        pFrame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.f,
        WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr))
        return hr;

    hr = pRenderTarget->CreateBitmapFromWicBitmap(
        pConverter.Get(),
        nullptr,
        &pBallImage);

    return hr;
}

void UUIManager::RenderText()
{
    DrawTextInternal(L""); // 빈 문자열
}

void UUIManager::RenderText(int inScore)
{
    wchar_t scoreText[100];
    swprintf_s(scoreText, L"Score: %d", inScore);

    DrawTextInternal(scoreText);
}

void UUIManager::RenderGameOverText() { // TODO: UI Component 만들어서 자동화 해주기
    wchar_t gameoverText[100] = L"Game Over\nPress R to Restart";
    DrawGameOverText(gameoverText);
}


void UUIManager::RenderImage()
{
    if (!pRenderTarget)
        return;

    pRenderTarget->BeginDraw();

    // Ball-Hierarchy
    if (pBallImage) {
        D2D1_SIZE_F bitmapSize = pBallImage->GetSize();
        D2D1_RECT_F destRect = D2D1::RectF(10, 60, 10 + bitmapSize.width, 60 + bitmapSize.height);
        pRenderTarget->DrawBitmap(pBallImage.Get(), destRect);
    }

    HRESULT hr = pRenderTarget->EndDraw();

    if (FAILED(hr)) {}
}

void UUIManager::DrawTextInternal(const wchar_t* text)
{
    if (!pRenderTarget)
        return;

    pRenderTarget->BeginDraw();

    D2D1_SIZE_F rtSize = pRenderTarget->GetSize();
    float desiredWidth = 290.0f;
    float desiredHeight = 40.0f;
    float topMargin = 10.0f;

    D2D1_RECT_F layoutRect = D2D1::RectF(
        (rtSize.width - desiredWidth) / 2,
        topMargin,
        (rtSize.width - desiredWidth) / 2 + desiredWidth,
        topMargin + desiredHeight
    );

    pRenderTarget->DrawText(
        text,
        static_cast<UINT32>(wcslen(text)),
        pTextFormat.Get(),
        layoutRect,
        pTextBrush.Get());

    HRESULT hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {}
}

void UUIManager::DrawGameOverText(const wchar_t* text) {
    if (!pRenderTarget)
        return;

    pRenderTarget->BeginDraw();

    D2D1_SIZE_F rtSize = pRenderTarget->GetSize();
    float desiredWidth = 290.0f;
    float desiredHeight = 40.0f;
    float topMargin = 100.0f;

    D2D1_RECT_F layoutRect = D2D1::RectF(
        (rtSize.width - desiredWidth) / 2,
        topMargin,
        (rtSize.width - desiredWidth) / 2 + desiredWidth,
        topMargin + desiredHeight
    );

    pRenderTarget->DrawText(
        text,
        static_cast<UINT32>(wcslen(text)),
        pTextFormat.Get(),
        layoutRect,
        pTextBrush.Get());

    HRESULT hr = pRenderTarget->EndDraw();
    if (FAILED(hr)) {}
}


