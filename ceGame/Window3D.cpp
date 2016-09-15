#include "stdafx.h"
#include "Window3D.h"

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "Timer_IE.h"
#include "Input.h"

LRESULT CWindow3D::WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{

	static char Title[400];

	
	switch (Msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	
	case WM_LBUTTONDOWN:
		st_Input->AquireKBandMouse();
		
		break;
/*	case WM_LBUTTONDOWN:
		//m_bHasFocus=true;
		//CEngine::Inst()->m_DInput.SetActive(true);
		//CEngine::Inst()->m_DInput.SetAcquire();
		break;*/
	case WM_SIZE:
		{
			int iWidth=LOWORD(lParam);
			int iHeight=HIWORD(lParam);
			if (g_pGraphicsAPI)
				g_pGraphicsAPI->OnResize(iWidth,iHeight);
				
		}
		break;
	}
	return __super::WindowProc(hWnd,Msg,wParam,lParam);
}