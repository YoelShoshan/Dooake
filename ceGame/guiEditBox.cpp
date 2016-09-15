#include "stdafx.h"
#include "guiEditBox.h"

#include "RendererBackend_IE.h"
extern IRendererBackend_Export* g_pRendererBack;

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "Input.h"

#include "Timer_IE.h"
extern ITimer* g_pTimer;

#include "GraphicsAPI_IE.h"
extern IGraphicsAPI_Export* g_pGraphicsAPI;

#include "MainMenu.h"
extern CMainMenu* g_pMainMenu;

#include "ClipBoard.h"

bool CGuiEditBox::s_bTPressed = false;

CGuiEditBox::CGuiEditBox()
{

	m_iPosInBuffer = 0;

	/*m_iPosInBuffer = 5;
	sprintf(m_Text,"Blah!");*/
	m_Text[0] = NULL;

}

CGuiEditBox::~CGuiEditBox()
{

}

void CGuiEditBox::Render(void)
{
	if (!m_bRender)
		return;

	if (!m_bFocus)
		g_pRendererBack->Render2DQuad_Perc(m_spMatBackground,m_fPosX,m_fPosY,m_fWidth,m_fHeight);
	else
		g_pRendererBack->Render2DQuad_Perc(m_spMatBackgroundFocus,m_fPosX,m_fPosY,m_fWidth,m_fHeight);

	int iResX = g_pGraphicsAPI->GetResX();
	int iResY = g_pGraphicsAPI->GetResY();

	
	float fTextPosX = m_fPosX*0.01f*((float) iResX);
	float fTextPosY = (float) iResY - (m_fPosY+m_fHeight)*0.01f*((float) iResY);


	m_Text[m_iPosInBuffer] = NULL;
	g_pGraphicsAPI->printf(CVec3f(1.f,0.f,0.f),fTextPosX,fTextPosY,m_Text);
}

void CGuiEditBox::Clear(void)
{
	m_iPosInBuffer = 0;
	m_Text[0] = NULL;
	

}

void* CGuiEditBox::GetInfo(void)
{
	return m_Text;
}

bool CGuiEditBox::IsLegalIPCharacters(const char* pTestMe) const
{
	while ((*pTestMe))
	{
		if ( ((*pTestMe)>='0' && (*pTestMe)<='9') || (*pTestMe)=='.')
		{
			pTestMe++;
			continue;
		}
			
		return false;
	}

	return true;
}

void CGuiEditBox::RunLogics(float fMouseX_Perc,float fMouseY_Perc,bool bMousePressed_Left)
{

//todo: lol - change this insanely brute force coding *ggg

	if (!m_bActive)
		return ;

	if (CursorOnMe(fMouseX_Perc,fMouseY_Perc))
	{
		if (bMousePressed_Left)
			SetFocus();
	}
	


	if (!m_bFocus)
		return ;


	if (m_iLimit>0)
		if (m_iPosInBuffer>=m_iLimit)
			return ;

	if (m_iPosInBuffer>=EDIT_BOX_TEXT_BUFFER_SIZE)
		return ;



	static bool bPasteIsUp = true;

	// handle paste
	if (bPasteIsUp)
	if (st_Input->KeyPressed(DIK_LCONTROL) && st_Input->KeyPressed(DIK_V))
	{
		bPasteIsUp = false;
		char pasteText[100];
		CClipBoard::Paste(pasteText, 95);

		int iStrLen = strlen(pasteText);

		if (iStrLen==0)
			return; // nothing to paste

		if (m_iPosInBuffer+iStrLen-1>=EDIT_BOX_TEXT_BUFFER_SIZE)
			return; // not enough space in text buffer

		
		
		if (m_dwType==EDIT_BOX_TYPE_IP_ADDRESS)
		{
			if (IsLegalIPCharacters(pasteText))
			{
				memcpy(&m_Text[m_iPosInBuffer],pasteText,iStrLen);
				m_iPosInBuffer+=iStrLen;	

			}
		}
		else //
		{
			memcpy(&m_Text[m_iPosInBuffer],pasteText,iStrLen);
			m_iPosInBuffer+=iStrLen;	
		}

	} 
	else
		bPasteIsUp = true;
	

	////////////////////////////////////////////////////////////
	/*static bool bBackSpacePressed=false;
	if(st_Input->KeyPressed(DIK_BACKSPACE))
	{
		if (!bBackSpacePressed)
		{
			bBackSpacePressed=true;
			
			if (m_iPosInBuffer>0)
				m_iPosInBuffer--;
		}								
	}
	else
		bBackSpacePressed=false;*/

	static double timeSinceLastPress = 1000.f;
	static bool bBackspaceTurboMode = false;
	static bool bFirst = true;

	if(st_Input->KeyPressed(DIK_BACKSPACE))
	{
		if (bFirst || timeSinceLastPress>400.0 || (bBackspaceTurboMode && (timeSinceLastPress>35.0)))
		{			
			if (!bFirst)
			{
				bBackspaceTurboMode = true;
			}										
			timeSinceLastPress = 0.0;
			if (m_iPosInBuffer>0)
				m_iPosInBuffer--;
			bFirst = false;
		} 
		
	}
	else
	{		
		bBackspaceTurboMode = false;
		bFirst = true;
		timeSinceLastPress = 0.0;
	}

	timeSinceLastPress+= g_pTimer->GetDeltaTime();		
	

	////////////////////////////////////////////////////////////


	
	
	if (m_dwType==EDIT_BOX_TYPE_IP_ADDRESS)
		if (m_iPosInBuffer==15)
			return ;

	////////////////////////////////////////////////////////////
	static bool bPeriodPressed=false;
	if(st_Input->KeyPressed(DIK_PERIOD))
	{
		if (!bPeriodPressed)
		{
			bPeriodPressed=true;
			
			m_Text[m_iPosInBuffer] = '.';
			m_iPosInBuffer++;		
		}								
	}
	else
		bPeriodPressed=false;
	////////////////////////////////////////////////////////////



	

	////////////////////////////////////////////////////////////
	static bool b0Pressed=false;
	if(st_Input->KeyPressed(DIK_0))
	{
		if (!b0Pressed)
		{
			b0Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '0';
			else
				m_Text[m_iPosInBuffer] = ')';

			m_iPosInBuffer++;		
		}								
	}
	else
		b0Pressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool b1Pressed=false;
	if(st_Input->KeyPressed(DIK_1))
	{
		if (!b1Pressed)
		{
			b1Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '1';
			else
				m_Text[m_iPosInBuffer] = '!';

			m_iPosInBuffer++;		
		}								
	}
	else
		b1Pressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool b2Pressed=false;
	if(st_Input->KeyPressed(DIK_2))
	{
		if (!b2Pressed)
		{
			b2Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '2';
			else
				m_Text[m_iPosInBuffer] = '@';

			m_iPosInBuffer++;		
		}								
	}
	else
		b2Pressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool b3Pressed=false;
	if(st_Input->KeyPressed(DIK_3))
	{
		if (!b3Pressed)
		{
			b3Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '3';
			else
				m_Text[m_iPosInBuffer] = '#';

			m_iPosInBuffer++;		
		}								
	}
	else
		b3Pressed=false;
	////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////
	static bool b4Pressed=false;
	if(st_Input->KeyPressed(DIK_4))
	{
		if (!b4Pressed)
		{
			b4Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '4';
			else
				m_Text[m_iPosInBuffer] = '$';

			m_iPosInBuffer++;		
		}								
	}
	else
		b4Pressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool b5Pressed=false;
	if(st_Input->KeyPressed(DIK_5))
	{
		if (!b5Pressed)
		{
			b5Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '5';
			else
				m_Text[m_iPosInBuffer] = '%';


			m_iPosInBuffer++;		
		}								
	}
	else
		b5Pressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool b6Pressed=false;
	if(st_Input->KeyPressed(DIK_6))
	{
		if (!b6Pressed)
		{
			b6Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '6';
			else
				m_Text[m_iPosInBuffer] = '^';

			m_iPosInBuffer++;		
		}								
	}
	else
		b6Pressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool b7Pressed=false;
	if(st_Input->KeyPressed(DIK_7))
	{
		if (!b7Pressed)
		{
			b7Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '7';
			else
				m_Text[m_iPosInBuffer] = '&';


			m_iPosInBuffer++;		
		}								
	}
	else
		b7Pressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool b8Pressed=false;
	if(st_Input->KeyPressed(DIK_8))
	{
		if (!b8Pressed)
		{
			b8Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '8';
			else
				m_Text[m_iPosInBuffer] = '*';

			m_iPosInBuffer++;		
		}								
	}
	else
		b8Pressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool b9Pressed=false;
	if(st_Input->KeyPressed(DIK_9))
	{
		if (!b9Pressed)
		{
			b9Pressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '9';
			else
				m_Text[m_iPosInBuffer] = '(';


			m_iPosInBuffer++;		
		}								
	}
	else
		b9Pressed=false;
	////////////////////////////////////////////////////////////

	if (m_dwType==EDIT_BOX_TYPE_IP_ADDRESS)
		return ;

	////////////////////////////////////////////////////////////
	static bool bReturnPressed=false;
	if(st_Input->KeyPressed(DIK_RETURN) || st_Input->KeyPressed(DIK_NUMPADENTER))
	{
		if (!bReturnPressed)
		{
			bReturnPressed=true;		

			if (m_dwType==EDIT_BOX_TYPE_FREE_TEXT_WITH_NEWLINES)
			{
				m_Text[m_iPosInBuffer] = '\n';
				m_iPosInBuffer++;		
			}
			else if (m_dwType==EDIT_BOX_TYPE_FREE_TEXT_NO_NEWLINES)
			{
				// perhaps trigger action here

			}
		}								
	}
	else
		bReturnPressed=false;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	static bool bSpaceBarPressed=false;
	if(st_Input->KeyPressed(DIK_SPACE))
	{
		if (!bSpaceBarPressed)
		{
			bSpaceBarPressed=true;		

			m_Text[m_iPosInBuffer] = ' ';
			m_iPosInBuffer++;		
		}								
	}
	else
		bSpaceBarPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bAPressed=false;
	if(st_Input->KeyPressed(DIK_A))
	{
		if (!bAPressed)
		{
			bAPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'a';
			else
				m_Text[m_iPosInBuffer] = 'A';

			m_iPosInBuffer++;		
		}								
	}
	else
		bAPressed=false;
	////////////////////////////////////////////////////////////

	
	////////////////////////////////////////////////////////////
	static bool bBPressed=false;
	if(st_Input->KeyPressed(DIK_B))
	{
		if (!bBPressed)
		{
			bBPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'b';
			else
				m_Text[m_iPosInBuffer] = 'B';

			m_iPosInBuffer++;		
		}								
	}
	else
		bBPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bCPressed=false;
	if(st_Input->KeyPressed(DIK_C))
	{
		if (!bCPressed)
		{
			bCPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'c';
			else
				m_Text[m_iPosInBuffer] = 'C';

			m_iPosInBuffer++;		
		}								
	}
	else
		bCPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bDPressed=false;
	if(st_Input->KeyPressed(DIK_D))
	{
		if (!bDPressed)
		{
			bDPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'd';
			else
				m_Text[m_iPosInBuffer] = 'D';

			m_iPosInBuffer++;		
		}								
	}
	else
		bDPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bEPressed=false;
	if(st_Input->KeyPressed(DIK_E))
	{
		if (!bEPressed)
		{
			bEPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'e';
			else
				m_Text[m_iPosInBuffer] = 'E';

			m_iPosInBuffer++;		
		}								
	}
	else
		bEPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bFPressed=false;
	if(st_Input->KeyPressed(DIK_F))
	{
		if (!bFPressed)
		{
			bFPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'f';
			else
				m_Text[m_iPosInBuffer] = 'F';

			m_iPosInBuffer++;		
		}								
	}
	else
		bFPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bGPressed=false;
	if(st_Input->KeyPressed(DIK_G))
	{
		if (!bGPressed)
		{
			bGPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'g';
			else
				m_Text[m_iPosInBuffer] = 'G';

			m_iPosInBuffer++;		
		}								
	}
	else
		bGPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bHPressed=false;
	if(st_Input->KeyPressed(DIK_H))
	{
		if (!bHPressed)
		{
			bHPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'h';
			else
				m_Text[m_iPosInBuffer] = 'H';

			m_iPosInBuffer++;		
		}								
	}
	else
		bHPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bIPressed=false;
	if(st_Input->KeyPressed(DIK_I))
	{
		if (!bIPressed)
		{
			bIPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'i';
			else
				m_Text[m_iPosInBuffer] = 'I';

			m_iPosInBuffer++;		
		}								
	}
	else
		bIPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bJPressed=false;
	if(st_Input->KeyPressed(DIK_J))
	{
		if (!bJPressed)
		{
			bJPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'j';
			else
				m_Text[m_iPosInBuffer] = 'J';

			m_iPosInBuffer++;		
		}								
	}
	else
		bJPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bKPressed=false;
	if(st_Input->KeyPressed(DIK_K))
	{
		if (!bKPressed)
		{
			bKPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'k';
			else
				m_Text[m_iPosInBuffer] = 'K';

			m_iPosInBuffer++;		
		}								
	}
	else
		bKPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bLPressed=false;
	if(st_Input->KeyPressed(DIK_L))
	{
		if (!bLPressed)
		{
			bLPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'l';
			else
				m_Text[m_iPosInBuffer] = 'L';

			m_iPosInBuffer++;		
		}								
	}
	else
		bLPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bMPressed=false;
	if(st_Input->KeyPressed(DIK_M))
	{
		if (!bMPressed)
		{
			bMPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'm';
			else
				m_Text[m_iPosInBuffer] = 'M';

			m_iPosInBuffer++;		
		}								
	}
	else
		bMPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bNPressed=false;
	if(st_Input->KeyPressed(DIK_N))
	{
		if (!bNPressed)
		{
			bNPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'n';
			else
				m_Text[m_iPosInBuffer] = 'N';

			m_iPosInBuffer++;		
		}								
	}
	else
		bNPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bOPressed=false;
	if(st_Input->KeyPressed(DIK_O))
	{
		if (!bOPressed)
		{
			bOPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'o';
			else
				m_Text[m_iPosInBuffer] = 'O';

			m_iPosInBuffer++;		
		}								
	}
	else
		bOPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bPPressed=false;
	if(st_Input->KeyPressed(DIK_P))
	{
		if (!bPPressed)
		{
			bPPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'p';
			else
				m_Text[m_iPosInBuffer] = 'P';

			m_iPosInBuffer++;		
		}								
	}
	else
		bPPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bQPressed=false;
	if(st_Input->KeyPressed(DIK_Q))
	{
		if (!bQPressed)
		{
			bQPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'q';
			else
				m_Text[m_iPosInBuffer] = 'Q';

			m_iPosInBuffer++;		
		}								
	}
	else
		bQPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bRPressed=false;
	if(st_Input->KeyPressed(DIK_R))
	{
		if (!bRPressed)
		{
			bRPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'r';
			else
				m_Text[m_iPosInBuffer] = 'R';

			m_iPosInBuffer++;		
		}								
	}
	else
		bRPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bSPressed=false;
	if(st_Input->KeyPressed(DIK_S))
	{
		if (!bSPressed)
		{
			bSPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 's';
			else
				m_Text[m_iPosInBuffer] = 'S';

			m_iPosInBuffer++;		
		}								
	}
	else
		bSPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	
	if(st_Input->KeyPressed(DIK_T))
	{
		if (!s_bTPressed)
		{
			s_bTPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 't';
			else
				m_Text[m_iPosInBuffer] = 'T';

			m_iPosInBuffer++;		
		}								
	}
	else
		s_bTPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bUPressed=false;
	if(st_Input->KeyPressed(DIK_U))
	{
		if (!bUPressed)
		{
			bUPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'u';
			else
				m_Text[m_iPosInBuffer] = 'U';

			m_iPosInBuffer++;		
		}								
	}
	else
		bUPressed=false;
	////////////////////////////////////////////////////////////

	
	////////////////////////////////////////////////////////////
	static bool bVPressed=false;
	if(st_Input->KeyPressed(DIK_V))
	{
		if (!bVPressed)
		{
			bVPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'v';
			else
				m_Text[m_iPosInBuffer] = 'V';

			m_iPosInBuffer++;		
		}								
	}
	else
		bVPressed=false;
	////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////
	static bool bWPressed=false;
	if(st_Input->KeyPressed(DIK_W))
	{
		if (!bWPressed)
		{
			bWPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'w';
			else
				m_Text[m_iPosInBuffer] = 'W';

			m_iPosInBuffer++;		
		}								
	}
	else
		bWPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bXPressed=false;
	if(st_Input->KeyPressed(DIK_X))
	{
		if (!bXPressed)
		{
			bXPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'x';
			else
				m_Text[m_iPosInBuffer] = 'X';

			m_iPosInBuffer++;		
		}								
	}
	else
		bXPressed=false;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	static bool bYPressed=false;
	if(st_Input->KeyPressed(DIK_Y))
	{
		if (!bYPressed)
		{
			bYPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'y';
			else
				m_Text[m_iPosInBuffer] = 'Y';

			m_iPosInBuffer++;		
		}								
	}
	else
		bYPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bZPressed=false;
	if(st_Input->KeyPressed(DIK_Z))
	{
		if (!bZPressed)
		{
			bZPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = 'z';
			else
				m_Text[m_iPosInBuffer] = 'Z';

			m_iPosInBuffer++;		
		}								
	}
	else
		bZPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bApostrophePressed=false;
	if(st_Input->KeyPressed(DIK_APOSTROPHE))
	{
		if (!bApostrophePressed)
		{
			bApostrophePressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '\'';	
			else
				m_Text[m_iPosInBuffer] = '""';	

			m_iPosInBuffer++;		
		}								
	}
	else
		bApostrophePressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bNumPadEqualsPressed=false;
	if(st_Input->KeyPressed(DIK_NUMPADEQUALS))
	{
		if (!bNumPadEqualsPressed)
		{
			bNumPadEqualsPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '=';	
			else
				m_Text[m_iPosInBuffer] = '_';

			m_iPosInBuffer++;		
		}								
	}
	else
		bNumPadEqualsPressed=false;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	static bool bMinusEqualsPressed=false;
	if(st_Input->KeyPressed(DIK_MINUS))
	{
		if (!bMinusEqualsPressed)
		{
			bMinusEqualsPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '-';	
			else
				m_Text[m_iPosInBuffer] = '_';	

			m_iPosInBuffer++;		
		}								
	}
	else
		bMinusEqualsPressed=false;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	static bool bSemiColonPressed=false;
	if(st_Input->KeyPressed(DIK_SEMICOLON))
	{
		if (!bSemiColonPressed)
		{
			bSemiColonPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = ';';		
			else
				m_Text[m_iPosInBuffer] = ':';	

			m_iPosInBuffer++;		
		}								
	}
	else
		bSemiColonPressed=false;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	static bool bAddPressed=false;
	if(st_Input->KeyPressed(DIK_ADD))
	{
		if (!bAddPressed)
		{
			bAddPressed=true;		

			
			m_Text[m_iPosInBuffer] = '+';		
			

			m_iPosInBuffer++;		
		}								
	}
	else
		bAddPressed=false;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	static bool bSubtractPressed=false;
	if(st_Input->KeyPressed(DIK_SUBTRACT))
	{
		if (!bSubtractPressed)
		{
			bSubtractPressed=true;		

			
			m_Text[m_iPosInBuffer] = '-';		
			

			m_iPosInBuffer++;		
		}								
	}
	else
		bSubtractPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bUnderLinePressed=false;
	if(st_Input->KeyPressed(DIK_UNDERLINE))
	{
		if (!bUnderLinePressed)
		{
			bUnderLinePressed=true;		

			
			m_Text[m_iPosInBuffer] = '_';		
			

			m_iPosInBuffer++;		
		}								
	}
	else
		bUnderLinePressed=false;
	////////////////////////////////////////////////////////////


	
	////////////////////////////////////////////////////////////
	static bool bLBracketPressed=false;
	if(st_Input->KeyPressed(DIK_LBRACKET))
	{
		if (!bLBracketPressed)
		{
			bLBracketPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '[';		
			else
				m_Text[m_iPosInBuffer] = '{';	

			m_iPosInBuffer++;		
		}								
	}
	else
		bLBracketPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bRBracketPressed=false;
	if(st_Input->KeyPressed(DIK_RBRACKET))
	{
		if (!bRBracketPressed)
		{
			bRBracketPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = ']';		
			else
				m_Text[m_iPosInBuffer] = '}';	

			m_iPosInBuffer++;		
		}								
	}
	else
		bRBracketPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bSlashPressed=false;
	if(st_Input->KeyPressed(DIK_SLASH))
	{
		if (!bSlashPressed)
		{
			bSlashPressed=true;		

			if(!st_Input->KeyPressed(DIK_LSHIFT) && !st_Input->KeyPressed(DIK_RSHIFT))
				m_Text[m_iPosInBuffer] = '/';		
			else
				m_Text[m_iPosInBuffer] = '?';	

			m_iPosInBuffer++;		
		}								
	}
	else
		bSlashPressed=false;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	static bool bNumPadStarPressed=false;
	if(st_Input->KeyPressed(DIK_NUMPADSTAR))
	{
		if (!bNumPadStarPressed)
		{
			bNumPadStarPressed=true;		

			m_Text[m_iPosInBuffer] = '/';		

			m_iPosInBuffer++;		
		}								
	}
	else
		bNumPadStarPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bCommaPressed=false;
	if(st_Input->KeyPressed(DIK_COMMA))
	{
		if (!bCommaPressed)
		{
			bCommaPressed=true;		

			m_Text[m_iPosInBuffer] = ',';		

			m_iPosInBuffer++;		
		}								
	}
	else
		bCommaPressed=false;
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	static bool bBackSlashPressed=false;
	if(st_Input->KeyPressed(DIK_BACKSLASH))
	{
		if (!bBackSlashPressed)
		{
			bBackSlashPressed=true;		

			m_Text[m_iPosInBuffer] = '\\';		

			m_iPosInBuffer++;		
		}								
	}
	else
		bBackSlashPressed=false;
	////////////////////////////////////////////////////////////
	







	

}
void CGuiEditBox::SetText(const char* pText)
{
	memcpy(m_Text,pText,strlen(pText)+1);
	m_iPosInBuffer = strlen(pText);

}


// if null then no background is used
void CGuiEditBox::Load(DWORD dwType,const char *pchMatBackground,const char *pchMatBackgroundFocus,int iLimit)
{
	m_spMatBackground = g_pRendererBack->LoadMaterial(pchMatBackground);
	m_spMatBackgroundFocus = g_pRendererBack->LoadMaterial(pchMatBackgroundFocus);

	m_dwType = dwType;

	m_iLimit = iLimit;
}