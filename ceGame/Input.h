#ifndef CRIMSON_INPUT_H
#define CRIMSON_INPUT_H

#pragma once

#define st_Input CInput::Inst()

#include "Singleton.h"

#include <dinput.h>


#pragma comment( lib, "dinput8.lib")
#pragma comment( lib, "dxguid.lib")

/**/
//constants
#define D3DFVF_CUSTOMVERTEX	(D3DFVF_XYZ | D3DFVF_DIFFUSE)

//structures
struct D3DVERTEX
{
	float	fX,
		fY,
		fZ;
	DWORD	dwColor;
};
/**/

#define DIMOUSE_LEFTBUTTON		0
#define DIMOUSE_RIGHTBUTTON		1
#define DIMOUSE_MIDDLEBUTTON	2
#define DIMOUSE_4BUTTON		3
#define DIMOUSE_5BUTTON		4
#define DIMOUSE_6BUTTON		5
#define DIMOUSE_7BUTTON		6
#define DIMOUSE_8BUTTON		7

#define D3DFVF_CURSORVERTEX		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef struct SCursorVertex
{
float		fX,
		fY,
		fZ,
		fRHW;
DWORD		dwColor;
float		fU,
		fV;
} D3DCURSORVERTEX;

//#define MOUSE_SMOOTH_ARRAYS_SIZE 30
#define MOUSE_SMOOTH_ARRAYS_SIZE 320

class CInput: public CSingleton<CInput>
{
public:
	CInput(void);
	~CInput(void);

	void Init(DWORD dwWidth,DWORD dwHeight, HWND hWnd);



	bool					InitDirectInput(void);
	bool					InitKeyboard(void);
	bool					InitMouse(void);
	void                    AquireKBandMouse(void);
	void                    UnAquireKBandMouse(void);
	bool					Update(void);
	
	void					SetCooperativeLevel(DWORD dwCooperativeLevel);

	
	bool					KeyPressed(int);

	/*void					DrawCursor(void);*/
	void					SetCursor(const char*,int,int,DWORD);
	void					SetAttributes(bool,float);
	void					SetCursorPosition(float,float);

	void                    CenterMouse(void);

	float					GetRelativeX(bool bSmooth);
	float					GetRelativeY(bool bSmooth);
	float					GetRelativeZ(void);
	float					GetAbsoluteX(void);
	float					GetAbsoluteY(void);
	bool					MouseButtonDown(int);
	bool					MouseButtonDownFirstFrame(int);
	
	// disables the keys until next time they are up and down again
	void                    DisableMouseButtons(void) { m_bDisableMouseButtons=true;};
	

	DWORD                   GetScreenWidth(void) { return m_dwScreenWidth;};
	DWORD                   GetScreenHeight(void) { return m_dwScreenHeight;};

	void                    SetScreenWidth(DWORD dwScreenWidth) { m_dwScreenWidth = dwScreenWidth;};
	void                    SetScreenHeight(DWORD dwScreenHeight) { m_dwScreenHeight = dwScreenHeight;};


	// "was pressed" key buffer - used if I want each press to change a state of sth
	// (instead of a continues change)
	bool                    m_WPKeyBuffer[256]; 

	bool m_bDisableMouseButtons;
	

	int m_iFrameButtonWasPressed1;
	int m_iFrameButtonWasPressed2;

private:

	
	
		

	LPDIRECTINPUT8			m_pDirectInputObject;
	LPDIRECTINPUTDEVICE8	m_pDirectInputKeyboardDevice;
	LPDIRECTINPUTDEVICE8	m_pDirectInputMouseDevice;

	char					m_KeyBuffer[256];


	float                   m_fSmoothingXarray[MOUSE_SMOOTH_ARRAYS_SIZE];
	float                   m_fSmoothingYarray[MOUSE_SMOOTH_ARRAYS_SIZE];
	
	//LPDIRECT3DTEXTURE9		m_pCursorTexture;

	DIMOUSESTATE2			m_MouseState;
	bool					m_bInverted;
	float					m_fSensitivity,
							m_fCursorX,
							m_fCursorY;
	int						m_iHotSpotX,
							m_iHotSpotY;
	DWORD					m_dwAxes,
							m_dwButtons,
							m_dwCursorColor,
							m_dwScreenWidth,
							m_dwScreenHeight;

	HWND                    m_hWnd;  // the window we're getting key events from
};

#endif