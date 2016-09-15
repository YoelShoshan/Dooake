#include "stdafx.h"
#include "Input.h"


#include <assert.h>
#define DI_COOP_LEVEL DISCL_EXCLUSIVE
//#define DI_COOP_LEVEL DISCL_NONEXCLUSIVE

CInput::CInput(void)
{
	m_bDisableMouseButtons=false;	
}


void CInput::SetCooperativeLevel(DWORD dwCooperativeLevel)
{
	m_pDirectInputKeyboardDevice->SetCooperativeLevel(m_hWnd,DISCL_FOREGROUND | dwCooperativeLevel);
	m_pDirectInputMouseDevice->SetCooperativeLevel(m_hWnd,DISCL_FOREGROUND|dwCooperativeLevel);
}

void CInput::Init(DWORD dwWidth,DWORD dwHeight, HWND hWnd)
{
	m_hWnd = hWnd;

	m_pDirectInputObject = NULL;
	m_pDirectInputKeyboardDevice = NULL;
	m_pDirectInputMouseDevice = NULL;

	m_fCursorX = (float)dwWidth / 2;
	m_fCursorY = (float)dwHeight / 2;
	m_dwScreenWidth = dwWidth;
	m_dwScreenHeight = dwHeight;

	//if(!InitDirectInput()) g_App.SetD3DStatus(false);
	//else if(!InitKeyboard() || !InitMouse()) g_App.SetD3DStatus(false);

	//g_LogFile.OutPutPlainText("CInput - Direct Input:","3","000099",true);

	InitDirectInput();

	//g_LogFile.OutPutPlainText("CInput::InitKeyboard:","3","000000",false);
	//if (InitKeyboard())
	//	g_LogFile.OutPutPlainText("Successful.","3","009900",true);


	InitKeyboard();

	//g_LogFile.OutPutPlainText("CInput::InitMouse:","3","000000",false);
	//if (InitMouse())
	//g_LogFile.OutPutPlainText("Successful.","3","009900",true);

	InitMouse();







	// smoothing arrays

	for (int i=0;i<MOUSE_SMOOTH_ARRAYS_SIZE;i++)
	{
		m_fSmoothingXarray[i] = 0.f;
		m_fSmoothingYarray[i] = 0.f;
	}

	

}

/************************************************************************/
/* Name:		~CInput													*/
/* Description:	release DirectInput object and devices					*/
/************************************************************************/

CInput::~CInput(void)
{
	
	if(m_pDirectInputKeyboardDevice != NULL)
	{
		m_pDirectInputKeyboardDevice->Unacquire();
		m_pDirectInputKeyboardDevice->Release();
		m_pDirectInputKeyboardDevice = NULL;
	}

	if(m_pDirectInputMouseDevice != NULL)
	{
		m_pDirectInputMouseDevice->Unacquire();
		m_pDirectInputMouseDevice->Release();
		m_pDirectInputMouseDevice = NULL;
	}

	if(m_pDirectInputObject != NULL)
	{
		m_pDirectInputObject->Release();
		m_pDirectInputObject = NULL;
	}
}//~CInput

/************************************************************************/
/* Name:		InitDirectInput											*/
/* Description:	create DirectInput object								*/
/************************************************************************/

bool CInput::InitDirectInput(void)
{
//	g_LogFile.OutPutPlainText("CInput::InitDirectInput:","3","000000",false);
if(FAILED(DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&m_pDirectInputObject,NULL)))
 {
	//MessageBox(m_hWnd,"DirectInput8Create() failed!","InitDirectInput()",MB_OK);
//	 g_LogFile.OutPutPlainText("FAILED!!!","3","990000",true);
 return false;
 }
else
//	g_LogFile.OutPutPlainText("Successful.","3","009900",true);

return true;
}//InitDirectInput

/************************************************************************/
/* Name:		InitKeyboard											*/
/* Description:	init keyboard device									*/
/************************************************************************/

bool CInput::InitKeyboard(void)
{
if(FAILED(m_pDirectInputObject->CreateDevice(GUID_SysKeyboard,&m_pDirectInputKeyboardDevice,NULL)))
 {
	 //MessageBox(m_hWnd,"CreateDevice() failed!","InitKeyboard()",MB_OK);
//	 g_LogFile.OutPutPlainText("CreateDevice FAILED!!!","3","990000",true);
 return false;
 }


if(FAILED(m_pDirectInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard)))
 {
	 //MessageBox(m_hWnd,"SetDataFormat() failed!","InitKeyboard()",MB_OK);
//	 g_LogFile.OutPutPlainText("SetDataFormat FAILED!!!","3","990000",true);
 return false;
 }


 //if(FAILED(m_pDirectInputKeyboardDevice->SetCooperativeLevel(m_hWnd,DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
 if(FAILED(m_pDirectInputKeyboardDevice->SetCooperativeLevel(m_hWnd,DISCL_FOREGROUND | DI_COOP_LEVEL)))
 {
	 //MessageBox(m_hWnd,"SetCooperativeLevel() failed!","InitKeyboard()",MB_OK);
//	 g_LogFile.OutPutPlainText("SetCoorperativeLevel FAILED!!!","3","990000",true);
 return false;
 }


if(FAILED(m_pDirectInputKeyboardDevice->Acquire()))
 {
	 //MessageBox(m_hWnd,"Acquire() failed!","InitKeyboard()",MB_OK);
//	 g_LogFile.OutPutPlainText("Acquire FAILED!!!","3","990000",true);
 return false;
 }


return true;
}//InitKeyboard

void CInput::AquireKBandMouse(void)
{
	m_pDirectInputKeyboardDevice->Acquire();
	m_pDirectInputMouseDevice->Acquire();

	/*InitMouse();
	InitKeyboard();*/
}

void CInput::UnAquireKBandMouse(void)
{
	m_pDirectInputKeyboardDevice->Unacquire();
	m_pDirectInputMouseDevice->Unacquire();
}

/************************************************************************/
/* Name:		InitMouse												*/
/* Description:	init mouse device										*/
/************************************************************************/

bool CInput::InitMouse(void)
{
DIDEVCAPS MouseCapabilities;

if(FAILED(m_pDirectInputObject->CreateDevice(GUID_SysMouse,&m_pDirectInputMouseDevice,NULL)))
 {
	 //MessageBox(m_hWnd,"CreateDevice() failed!","InitMouse()",MB_OK);
//	 g_LogFile.OutPutPlainText("CreateDevice FAILED!!!","3","990000",true);
 return false;
 }

if(FAILED(m_pDirectInputMouseDevice->SetDataFormat(&c_dfDIMouse2)))
 {
//	 g_LogFile.OutPutPlainText("SetDataFormat FAILED!!!","3","990000",true);
 //MessageBox(m_hWnd,"SetDataFormat() failed!","InitMouse()",MB_OK);
 return false;
 }

 // HERE I SET THE EXCLUSIVE / NONEXCLUSIVE PART

 //if(FAILED(m_pDirectInputMouseDevice->SetCooperativeLevel(m_hWnd,DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
 //if(FAILED(m_pDirectInputMouseDevice->SetCooperativeLevel(m_hWnd,DISCL_EXCLUSIVE|DISCL_FOREGROUND)))
 if(FAILED(m_pDirectInputMouseDevice->SetCooperativeLevel(m_hWnd,DI_COOP_LEVEL|DISCL_FOREGROUND)))
  
 {
//	 g_LogFile.OutPutPlainText("SetCooperativeLevel FAILED!!!","3","990000",true);
	 //MessageBox(m_hWnd,"SetCooperativeLevel() failed!","InitMouse()",MB_OK);
 return false;
 }

if(FAILED(m_pDirectInputMouseDevice->Acquire()))
 {
//	 g_LogFile.OutPutPlainText("Acquire FAILED!!!","3","990000",true);
	 //MessageBox(m_hWnd,"Acquire() failed!","InitMouse()",MB_OK);
 return false;
 }

MouseCapabilities.dwSize = sizeof(MouseCapabilities);
m_pDirectInputMouseDevice->GetCapabilities(&MouseCapabilities);

if(!(MouseCapabilities.dwFlags & DIDC_ATTACHED))
 {
//	 g_LogFile.OutPutPlainText("Mouse is not attached!!!","3","990000",true);
 //MessageBox(m_hWnd,"Mouse is currently not attached!","InitMouse()",MB_OK);
 return false;
 }

m_dwAxes = MouseCapabilities.dwAxes;
m_dwButtons = MouseCapabilities.dwButtons;

SetCursor("arrow.bmp",0,0,0xffffffff);
SetAttributes(false,1.0f);

return true;
}//InitMouse

/************************************************************************/
/* Name:		Update													*/
/* Description:	update state of keyboard device							*/
/************************************************************************/

bool CInput::Update(void)
{

if(FAILED(m_pDirectInputKeyboardDevice->Acquire()))
{
	return false;
}


// removed for polling
if(FAILED(m_pDirectInputKeyboardDevice->GetDeviceState(sizeof(m_KeyBuffer),(LPVOID)&m_KeyBuffer)))
 {
	 MessageBox(m_hWnd,"GetDeviceState() failed!","Update()",MB_OK);
 return false;
 }

//PollKeyboard();


if(m_pDirectInputMouseDevice->GetDeviceState(sizeof(m_MouseState),(LPVOID)&m_MouseState) == DIERR_INPUTLOST)
 {
 m_pDirectInputMouseDevice->Acquire();
 }
 

m_fCursorX += m_MouseState.lX * m_fSensitivity;
if(m_fCursorX < 0) m_fCursorX = 0;
else if(m_fCursorX > m_dwScreenWidth) m_fCursorX = (float)m_dwScreenWidth;

if(m_bInverted) m_fCursorY -= m_MouseState.lY * m_fSensitivity;
else m_fCursorY += m_MouseState.lY * m_fSensitivity;
if(m_fCursorY < 0) m_fCursorY = 0;
else if(m_fCursorY > m_dwScreenHeight) m_fCursorY = (float)m_dwScreenHeight;

return true;
}//Update






/************************************************************************/
/* Name:		KeyPressed												*/
/* Description:	check if key has been pressed							*/
/************************************************************************/

bool CInput::KeyPressed(int Key)
{
if(m_KeyBuffer[Key] & 0x80)
 {
 return true;
 }

return false;
}//KeyPressed

/************************************************************************/
/* Name:		DrawCursor												*/
/* Description:	draw mouse cursor with Direct3D							*/
/************************************************************************/

/*void CInput::DrawCursor(void)
{
D3DCURSORVERTEX aCursorVertices[4];

aCursorVertices[0].fX = m_fCursorX - m_iHotSpotX;
aCursorVertices[0].fY = m_fCursorY - m_iHotSpotY;
aCursorVertices[0].fZ = 0.0f;
aCursorVertices[0].fRHW = 1.0f;
aCursorVertices[0].dwColor = m_dwCursorColor;
aCursorVertices[0].fU = 0.0f;
aCursorVertices[0].fV = 0.0f;

aCursorVertices[1].fX = m_fCursorX - m_iHotSpotX;
aCursorVertices[1].fY = m_fCursorY + 32.0f - m_iHotSpotY;
aCursorVertices[1].fZ = 0.0f;
aCursorVertices[1].fRHW = 1.0f;
aCursorVertices[1].dwColor = m_dwCursorColor;
aCursorVertices[1].fU = 0.0f;
aCursorVertices[1].fV = 1.0f;

aCursorVertices[2].fX = m_fCursorX + 32.0f - m_iHotSpotX;
aCursorVertices[2].fY = m_fCursorY - m_iHotSpotY;
aCursorVertices[2].fZ = 0.0f;
aCursorVertices[2].fRHW = 1.0f;
aCursorVertices[2].dwColor = m_dwCursorColor;
aCursorVertices[2].fU = 1.0f;
aCursorVertices[2].fV = 0.0f;

aCursorVertices[3].fX = m_fCursorX + 32.0f - m_iHotSpotX;
aCursorVertices[3].fY = m_fCursorY + 32.0f - m_iHotSpotY;
aCursorVertices[3].fZ = 0.0f;
aCursorVertices[3].fRHW = 1.0f;
aCursorVertices[3].dwColor = m_dwCursorColor;
aCursorVertices[3].fU = 1.0f;
aCursorVertices[3].fV = 1.0f;

g_App.GetDevice()->SetFVF(D3DFVF_CURSORVERTEX);

g_App.GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
g_App.GetDevice()->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
g_App.GetDevice()->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

g_App.GetDevice()->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);

g_App.GetDevice()->SetTexture(0,m_pCursorTexture);
g_App.GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,&aCursorVertices[0],sizeof(D3DCURSORVERTEX));

g_App.GetDevice()->SetFVF(D3DFVF_CUSTOMVERTEX);
}//DrawCursor*/

/************************************************************************/
/* Name:		SetCursor												*/
/* Description:	load new mouse cursor from file							*/
/************************************************************************/

void CInput::SetCursor(const char* strFilename,int iCursorHotSpotX,int iCursorHotSpotY,DWORD dwColor)
{
//D3DXCreateTextureFromFileExA(g_App.GetDevice(),strFilename,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,0xff000000,NULL,NULL,&m_pCursorTexture);

m_iHotSpotX = iCursorHotSpotX;
m_iHotSpotY = iCursorHotSpotY;
m_dwCursorColor = dwColor;
}//SetCursor

/************************************************************************/
/* Name:		SetAttributes											*/
/* Description:	change cursor behaviour									*/
/************************************************************************/

void CInput::SetAttributes(bool bInvert,float fNewSensitivity)
{
m_bInverted = bInvert;
m_fSensitivity = fNewSensitivity;
}//SetAttributes

/************************************************************************/
/* Name:		SetCursorPosition										*/
/* Description:	change cursor position									*/
/************************************************************************/


void CInput::CenterMouse(void)
{
	m_fCursorX = (float)m_dwScreenWidth / 2;
	m_fCursorY = (float)m_dwScreenHeight / 2;
}

void CInput::SetCursorPosition(float fNewX,float fNewY)
{
m_fCursorX = fNewX;
m_fCursorY = fNewY;
}//SetCursorPosition

/************************************************************************/
/* Name:		GetRelativeX											*/
/* Description:	return relative x change of cursor						*/
/************************************************************************/

float CInput::GetRelativeX(bool bSmooth)
{

	if (bSmooth)
	{

		memcpy(&m_fSmoothingXarray[1],&m_fSmoothingXarray[0],sizeof(float)*(MOUSE_SMOOTH_ARRAYS_SIZE-1));
		m_fSmoothingXarray[0] = (float)m_MouseState.lX;

		float fTempX = 0.f;

		for (int i=0;i<MOUSE_SMOOTH_ARRAYS_SIZE;i++)
			fTempX+=m_fSmoothingXarray[i];

		fTempX/=MOUSE_SMOOTH_ARRAYS_SIZE;

		return fTempX;
	}
	else
		return (float)m_MouseState.lX;
		
	// original
	/*//return (float)m_MouseState.lX * m_fSensitivity;*/

}//GetRelativeX

/************************************************************************/
/* Name:		GetRelativeY											*/
/* Description:	return relative y change of cursor						*/
/************************************************************************/

float CInput::GetRelativeY(bool bSmooth)
{
	if (bSmooth)
	{

		memcpy(&m_fSmoothingYarray[1],&m_fSmoothingYarray[0],sizeof(float)*(MOUSE_SMOOTH_ARRAYS_SIZE-1));
		m_fSmoothingYarray[0] = (float)m_MouseState.lY;


		float fTempY = 0.f;

		for (int i=0;i<MOUSE_SMOOTH_ARRAYS_SIZE;i++)
			fTempY+=m_fSmoothingYarray[i];

		fTempY/=MOUSE_SMOOTH_ARRAYS_SIZE;

		return fTempY;
	}
	else
		return (float)m_MouseState.lY;

	//original
	/*//return (float)m_MouseState.lY * m_fSensitivity;*/
	
}//GetRelativeY

/************************************************************************/
/* Name:		GetRelativeZ											*/
/* Description:	return relative z change of cursor						*/
/************************************************************************/

float CInput::GetRelativeZ(void)
{
return (float)m_MouseState.lZ * m_fSensitivity;
}//GetRelativeZ

/************************************************************************/
/* Name:		GetAbsoluteX											*/
/* Description:	return absolute x of cursor								*/
/************************************************************************/

float CInput::GetAbsoluteX(void)
{
return m_fCursorX;
}//GetAbsoluteX

/************************************************************************/
/* Name:		GetAbsoluteY											*/
/* Description:	return absolute y of cursor								*/
/************************************************************************/

float CInput::GetAbsoluteY(void)
{

return m_fCursorY;


}//GetAbsoluteY

/************************************************************************/
/* Name:		MouseButtonDown											*/
/* Description:	check for pressed mouse button							*/
/************************************************************************/

bool CInput::MouseButtonDown(int Button)
{
	if (m_bDisableMouseButtons)
	{
		
		if(!(m_MouseState.rgbButtons[Button] & 0x80))
			m_bDisableMouseButtons=false;
	}

//if (!m_bDisableMouseButtons)
if(m_MouseState.rgbButtons[Button] & 0x80)
 {

/*	 if (m_iFrameButtonWasPressed1!=st_Engine::Inst()->m_iFrameCounter)
	 {
		m_iFrameButtonWasPressed2 = m_iFrameButtonWasPressed1;
		m_iFrameButtonWasPressed1 = st_Engine::Inst()->m_iFrameCounter;
	 
		if (m_iFrameButtonWasPressed1-m_iFrameButtonWasPressed2>1)		*/
			return true;
	 //}
 }


return false;
}//MouseButtonDown

/************************************************************************/
/* Name:		MouseButtonDownFirstFrame								*/
/* Description:	check for pressed mouse button							*/
/************************************************************************/

bool CInput::MouseButtonDownFirstFrame(int Button)
{
	if (m_bDisableMouseButtons)
	{
		
		if(!(m_MouseState.rgbButtons[Button] & 0x80))
			m_bDisableMouseButtons=false;
	}

/*if (!m_bDisableMouseButtons)
if(m_MouseState.rgbButtons[Button] & 0x80)
 {

	 if (m_iFrameButtonWasPressed1!=st_Engine::Inst()->m_iFrameCounter)
	 {
		m_iFrameButtonWasPressed2 = m_iFrameButtonWasPressed1;
		m_iFrameButtonWasPressed1 = st_Engine::Inst()->m_iFrameCounter;
	 
		if (m_iFrameButtonWasPressed1-m_iFrameButtonWasPressed2>1)		
			return true;
	 }
 }*/


return false;

}//MouseButtonDown

 


