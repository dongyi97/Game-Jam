#ifndef INPUTMANGER_H
#define INPUTMANGER_H

#include <string>
#include <unordered_map>
#include <functional>
#include <Windows.h>
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using EventHandler = std::function<void()>;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class InputManager
{
public:
	// 키 입력에 따른 함수 등록
	static void RegisterEvent(const std::string& eventName, EventHandler handler);
	// 각종 메시지를 처리할 함수
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	// 키보드 입력에 따른 이벤트를 저장해두는 map
	static std::unordered_map<std::string, EventHandler> eventHandlers;
	// 마우스 입력 받는 변수
	LPDIRECTINPUT8 directInput;
};

class MouseInput
{
private:
	LPDIRECTINPUT8 m_directInput;
	LPDIRECTINPUTDEVICE8 m_mouseDev;
	DIMOUSESTATE m_mouseState;
	DIMOUSESTATE m_prevMouseState;
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

public:
	MouseInput();
	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void Shutdown();
	bool Frame();

	bool IsLeftPressed() const;
	bool IsRightPressed() const;
	bool IsMiddlePressed() const;
	bool IsLeftClicked() const;
	bool IsRightClicked() const;

	void GetMouseLocation(int& mouseX, int& mouseY) const;
	void GetMouseDelta(int& deltaX, int& deltaY) const;
};

#endif