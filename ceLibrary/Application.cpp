#include "stdafx.h"
#include "Application.h"


CApplication::CApplication()
{

}
CApplication::~CApplication()
{

}

bool CApplication::Init(HWND hWnd)
{
	m_hWnd = hWnd;

	return true;
}

int CApplication::MessageLoop(void)
{
	MSG					msg;											// Window Message Structure
		
	bool bIsMessagePumpActive = TRUE;								// Set isMessagePumpActive To TRUE
		while (bIsMessagePumpActive == TRUE)						// While The Message Pump Is Active
		{
			// Success Creating Window.  Check For Window Messages
			if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE) != 0)
			{
				// Check For WM_QUIT Message
				if (msg.message != WM_QUIT)						// Is The Message A WM_QUIT Message?
				{
					DispatchMessage (&msg);						// If Not, Dispatch The Message
				}
				else											// Otherwise (If Message Is WM_QUIT)
				{
					bIsMessagePumpActive = FALSE;				// Terminate The Message Pump
				}
			}
			else												// If There Are No Messages
			{
				Frame();

			}
		}	


		return 1;
}