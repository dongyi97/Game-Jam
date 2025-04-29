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
	// Ű �Է¿� ���� �Լ� ���
	static void RegisterEvent(const std::string& eventName, EventHandler handler);
	// ���� �޽����� ó���� �Լ�
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	// Ű���� �Է¿� ���� �̺�Ʈ�� �����صδ� map
	static std::unordered_map<std::string, EventHandler> eventHandlers;
	// ���콺 �Է� �޴� ����
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