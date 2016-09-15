#include "stdafx.h"
#include "Window.h"

//----------------------------------------------------------------------------------------------------------
CWindow::CWindow()
{
	m_hWnd=NULL;
	m_aClass=0;
	m_hInst=NULL;
}
//----------------------------------------------------------------------------------------------------------
CWindow::~CWindow()
{
	Destroy();
}
//----------------------------------------------------------------------------------------------------------
LRESULT CWindow::GeneralWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// This function could _may_ contain some 64 bit portability issues. The compiler reports
	// the SetWindowLongPtr function to take a LONG parameter, while the MSDN says its a LONG_PTR
	switch (Msg)
	{
		case WM_CREATE:
			{
				SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);

				CWindow* pClass=(CWindow*)(GetWindowLongPtr(hWnd, GWL_USERDATA));
				if (pClass)
					return pClass->WindowProc(hWnd,Msg,wParam,lParam);
				return 0;
			}
			break;

		case WM_DESTROY:
			{
				CWindow* pClass=(CWindow*)(GetWindowLongPtr(hWnd, GWL_USERDATA));
				SetWindowLongPtr(hWnd, GWL_USERDATA, NULL);
				if (pClass)
					return pClass->WindowProc(hWnd,Msg,wParam,lParam);
				else
					return DefWindowProc(hWnd,Msg,wParam,lParam);
			}

			break;
	}

	CWindow* pClass=(CWindow*)(GetWindowLongPtr(hWnd, GWL_USERDATA));
	if (pClass)
		return pClass->WindowProc(hWnd,Msg,wParam,lParam);
	else
		return DefWindowProc(hWnd,Msg,wParam,lParam);
}
//----------------------------------------------------------------------------------------------------------
bool CWindow::Create(HINSTANCE hInst, tWinProperties& windowProps)
{
	m_hInst=hInst;
	WNDCLASSEX wc = {sizeof(WNDCLASSEX), 0/*CS_CLASSDC/**/, (WNDPROC)GeneralWndProc, 0L, 0L, 
                     hInst, NULL, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,        
					 windowProps.pClassName, NULL};
	
	wc.style=CS_HREDRAW | CS_VREDRAW;
    //wc.hCursor = LoadCursor(NULL, IDC_ARROW);	
	
	m_aClass=RegisterClassEx(&wc);
	if (m_aClass==0)
	{
		//g_pLog->Error("CWindow::Create RegisterClassEx failed | ErrorCode: %d",GetLastError());
		return false;
	}

	// Adjust the rect of the window so that the clientsize matches the values in the properties
	const DWORD dwWindowStyle=WS_OVERLAPPEDWINDOW;
	RECT rc;
    SetRect(&rc, 0, 0, windowProps.iClientWidth, windowProps.iClientHeight);
    AdjustWindowRect(&rc, dwWindowStyle, false);

	m_hWnd = CreateWindowEx(
			0,
			windowProps.pClassName,
			windowProps.pCaption,
			WS_OVERLAPPEDWINDOW,// | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			windowProps.iXPos,
			windowProps.iYPos,
			(rc.right-rc.left),
			(rc.bottom-rc.top),
			NULL,
			NULL,
			hInst,
			(CWindow*)this );

	if (m_hWnd==0)
	{
		//g_pLog->Error("CWindow::Create CreateWindowEx failed | ErrorCode: %d",GetLastError());
		return false;
	}

	ShowWindow(m_hWnd,SW_SHOW);
	UpdateWindow(m_hWnd);

	return true; 
}
//----------------------------------------------------------------------------------------------------------
void CWindow::Destroy()
{
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd=NULL;
	}
	if ((m_aClass)&&(m_hInst))
	{
		int ret=UnregisterClass((LPCSTR)m_aClass,m_hInst);
		m_aClass=0;
	}
	m_hInst=NULL;
	
}
//----------------------------------------------------------------------------------------------------------
LRESULT CWindow::WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg==WM_CLOSE)
	{
		Destroy();
		return 0;
	}
	return DefWindowProc(hWnd,Msg,wParam,lParam);
}
//----------------------------------------------------------------------------------------------------------
void CWindow::SetWindowText(const char* pText)
{
	::SetWindowText(m_hWnd,pText);
}
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
