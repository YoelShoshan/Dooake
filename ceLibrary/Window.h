#ifndef CRIMSON_WINDOW_H
#define CRIMSON_WINDOW_H


#pragma once

#include <windows.h>

struct tWinProperties
{
	UINT iXPos;
	UINT iYPos;
	UINT iClientWidth;
	UINT iClientHeight;
	char* pClassName;
	char* pCaption;
};

class CWindow
{
public:
	CWindow();
	virtual ~CWindow();
	bool Create(HINSTANCE hInst, tWinProperties& windowProps);
	void Destroy();
	void SetWindowText(const char* pText);
	HWND GetHWND() const;
	
protected:
	HWND m_hWnd;
	// Needed to free the Atom returned by RegisterClassEx
	HINSTANCE m_hInst;
	ATOM m_aClass;

	virtual LRESULT WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	// Static function which is passed as callback to the win32 functions.
	// The classpointer is transmitted via a user variable in the WM_CREATE Message
	// Once WM_CREATE occurs, its stored via SetWindowLongPtr inside the window itself.
	// When other messages occur, this pointer is read, and the Message is redirected
	// to the WindowProc function in the according class instance.
	static LRESULT GeneralWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

};

//----------------------------------------------------------------------------------------------------------
__forceinline HWND CWindow::GetHWND() const
{
	return m_hWnd;
}
//----------------------------------------------------------------------------------------------------------



#endif