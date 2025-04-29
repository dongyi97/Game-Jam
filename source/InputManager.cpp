#include "InputManager.h"

std::unordered_map<std::string, EventHandler> InputManager::eventHandlers;

// 각종 메시지를 처리할 함수
LRESULT CALLBACK InputManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }

    switch (message)
    {
    case WM_DESTROY:
        // Signal that the app should quit
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN: // TODO: 매직 넘버 제거
        // 키보드 입력 이벤트 처리
        if (wParam == 'A' && InputManager::eventHandlers.find("OnLeftArrow") != InputManager::eventHandlers.end()) {
            InputManager::eventHandlers["OnLeftArrow"]();
        }
        if (wParam == 'D' && InputManager::eventHandlers.find("OnRightArrow") != InputManager::eventHandlers.end()) {
            InputManager::eventHandlers["OnRightArrow"]();
        }
        if (wParam == 'W' && InputManager::eventHandlers.find("OnUpArrow") != InputManager::eventHandlers.end()) {
            InputManager::eventHandlers["OnUpArrow"]();
        }
        if (wParam == 'S' && InputManager::eventHandlers.find("OnDownArrow") != InputManager::eventHandlers.end()) {
            InputManager::eventHandlers["OnDownArrow"]();
        }
        if (wParam == VK_SPACE && InputManager::eventHandlers.find("OnPressedSpace") != InputManager::eventHandlers.end()) {
            InputManager::eventHandlers["OnPressedSpace"]();
        }
        if (wParam == 'R' && InputManager::eventHandlers.find("OnPressedR") != InputManager::eventHandlers.end()) {
            InputManager::eventHandlers["OnPressedR"]();
        }
        if (wParam == 'G' && InputManager::eventHandlers.find("OnGKey") != InputManager::eventHandlers.end()) {
            InputManager::eventHandlers["OnGKey"]();
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void InputManager::RegisterEvent(const std::string& eventName, EventHandler handler) {
    eventHandlers[eventName] = handler;
}

MouseInput::MouseInput() : m_directInput(nullptr), m_mouseDev(nullptr),
m_mouseX(0), m_mouseY(0), m_screenWidth(0), m_screenHeight(0)
{
    ZeroMemory(&m_mouseState, sizeof(m_mouseState));
    ZeroMemory(&m_prevMouseState, sizeof(m_prevMouseState));
}

bool MouseInput::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    HRESULT result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION,
        IID_IDirectInput8, (void**)&m_directInput, NULL);
    if (FAILED(result))
        return false;

    result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouseDev, NULL);
    if (FAILED(result))
        return false;

    result = m_mouseDev->SetDataFormat(&c_dfDIMouse);
    if (FAILED(result))
        return false;

    result = m_mouseDev->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(result))
        return false;

    m_mouseDev->Acquire();

    return true;
}

void MouseInput::Shutdown()
{
    if (m_mouseDev)
    {
        m_mouseDev->Unacquire();
        m_mouseDev->Release();
        m_mouseDev = nullptr;
    }

    if (m_directInput)
    {
        m_directInput->Release();
        m_directInput = nullptr;
    }
}

bool MouseInput::Frame()
{
    memcpy(&m_prevMouseState, &m_mouseState, sizeof(m_mouseState));

    if (FAILED(m_mouseDev->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState)))
    {
        if (FAILED(m_mouseDev->Acquire()))
            return false;

        if (FAILED(m_mouseDev->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState)))
            return false;
    }

    m_mouseX += m_mouseState.lX;
    m_mouseY += m_mouseState.lY;

    if (m_mouseX < 0) m_mouseX = 0;
    if (m_mouseY < 0) m_mouseY = 0;
    if (m_mouseX > m_screenWidth) m_mouseX = m_screenWidth;
    if (m_mouseY > m_screenHeight) m_mouseY = m_screenHeight;

    return true;
}

bool MouseInput::IsLeftPressed() const
{
    return (m_mouseState.rgbButtons[0] & 0x80) != 0;
}

bool MouseInput::IsRightPressed() const
{
    return (m_mouseState.rgbButtons[1] & 0x80) != 0;
}

bool MouseInput::IsMiddlePressed() const
{
    return (m_mouseState.rgbButtons[2] & 0x80) != 0;
}

bool MouseInput::IsLeftClicked() const
{
    return !(m_prevMouseState.rgbButtons[0] & 0x80) && (m_mouseState.rgbButtons[0] & 0x80);
}

bool MouseInput::IsRightClicked() const
{
    return !(m_prevMouseState.rgbButtons[1] & 0x80) && (m_mouseState.rgbButtons[1] & 0x80);
}

void MouseInput::GetMouseLocation(int& mouseX, int& mouseY) const
{
    mouseX = m_mouseX;
    mouseY = m_mouseY;
}

void MouseInput::GetMouseDelta(int& deltaX, int& deltaY) const
{
    deltaX = m_mouseState.lX;
    deltaY = m_mouseState.lY;
}