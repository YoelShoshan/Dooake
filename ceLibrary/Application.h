#ifndef CRIMSON_APPLICATION_H
#define CRIMSON_APPLICATION_H

#pragma once

#include "ceLibrary.h"

class CApplication
{
public:
	CApplication();
	~CApplication();

	bool Init(HWND hWnd);

	int MessageLoop(void);

	virtual void Frame(void) = 0;

	// Render();
private:

	HWND m_hWnd;
	//CWindow3D m_Window3D;
};


#endif