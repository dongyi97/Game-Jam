#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

#include "UObject.h"
#include <vector>
#include <memory>

class UUIManager {

public:
    static UUIManager* MainUUIManger;
    HRESULT Initialize(HWND hWnd);

    HRESULT LoadBallImage(const wchar_t* imagePath);

    void RenderText();
    void RenderText(int inScore);
    void RenderGameOverText();
    void RenderImage();

	//void Release();

private:
    Microsoft::WRL::ComPtr<ID2D1Factory> pD2DFactory;
    // RenderTarget을 ID2D1RenderTarget으로 변경
    Microsoft::WRL::ComPtr<ID2D1RenderTarget> pRenderTarget;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pTextBrush;
    Microsoft::WRL::ComPtr<IDWriteFactory> pDWriteFactory;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormat;

    // Ball-Hierarchy 이미지
    Microsoft::WRL::ComPtr<ID2D1Bitmap> pBallImage;

    void DrawTextInternal(const wchar_t* text);
    void DrawGameOverText(const wchar_t* text);
    
};
