#pragma once
#include <wtypes.h>
class FWindowManager
{
public:
	static HWND GetHWND() 
	{
		return MainhWnd;
	}


	static void SetHWND(HWND _MainhWnd)
	{ 
		MainhWnd = _MainhWnd;

		return ;
	}

public:
	static HWND MainhWnd;
};

