#ifndef CRIMSON_WINDOW_3D_H
#define CRIMSON_WINDOW_3D_H

///////////////////////////////////
// CWindow class created by cdTag
///////////////////////////////////

#pragma once

#include "Window.h"

class CWindow3D : public CWindow
{
public:
	CWindow3D() {};
	~CWindow3D() {};

	LRESULT WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};




#endif