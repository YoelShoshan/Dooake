#ifndef CRIMSON_CLIP_BOARD_H
#define CRIMSON_CLIP_BOARD_H

#pragma once

#include "windows.h"
// used "David Terracino <davet@lycosemail.com>" code.

class CClipBoard
{
public:
	CClipBoard();
	~CClipBoard();

	static void Copy(LPSTR lpszBuffer, HWND hWND = NULL);

	static void Paste(LPSTR lpszBuffer, int nBufSize,	 HWND hWnd = NULL);

};






#endif
