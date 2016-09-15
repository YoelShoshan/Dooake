#include "stdafx.h"
#include "ClipBoard.h"

CClipBoard::CClipBoard()
{

}


CClipBoard::~CClipBoard()
{


}

void CClipBoard::Copy(LPSTR lpszBuffer, HWND hWND)
{
	HGLOBAL hGlobal;		// Global memory handle
	LPSTR lpszData;			// Pointer to clipboard data
	unsigned long nSize;	// Size of clipboard data

	// First, open the clipboard. OpenClipboard() takes one
	// parameter, the handle of the window that will temporarily
	// be it's owner. If NULL is passed, the current process
	// is assumed. After opening, empty the clipboard so we
	// can put our text on it.
	OpenClipboard(hWND);
	EmptyClipboard();

	// Get the size of the string in the buffer that was
	// passed into the function, so we know how much global
	// memory to allocate for the string.
	nSize = lstrlen(lpszBuffer);

	// Allocate the memory for the string.
	hGlobal = GlobalAlloc(GMEM_ZEROINIT, nSize+1);
	
	// If we got any error during the memory allocation,
	// we have been returned a NULL handle.
	if (hGlobal == NULL) return;

	// Now we have a global memory handle to the text
	// stored on the clipboard. We have to lock this global
	// handle so that we have access to it.
	lpszData = (LPSTR)GlobalLock(hGlobal);

	// Now, copy the text from the buffer into the allocated
	// global memory pointer.
	for (UINT i = 0; i < nSize + 1; ++i)
		*(lpszData + i) = *(lpszBuffer + i);

	// Now, simply unlock the global memory pointer,
	// set the clipboard data type and pointer,
	// and close the clipboard.
	GlobalUnlock(hGlobal);
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();

}

void CClipBoard::Paste(LPSTR lpszBuffer, int nBufSize,	 HWND hWnd)
{
	HGLOBAL hGlobal;		// Global memory handle
	LPSTR lpszData;			// Pointer to clipboard data
	unsigned long nSize;	// Size of clipboard data

	// First, open the clipboard. OpenClipboard() takes one
	// parameter, the handle of the window that will temporarily
	// be it's owner. If NULL is passed, the current process
	// is assumed.
	OpenClipboard(hWnd);

	// Request a pointer to the text on the clipboard.
	hGlobal = GetClipboardData(CF_TEXT);

	// If there was no text on the clipboard, we have
	// been returned a NULL handle.	
	if (hGlobal == NULL) 
	{
		lpszBuffer[0] = NULL;
		return;
	}

	// Now we have a global memory handle to the text
	// stored on the clipboard. We have to lock this global
	// handle so that we have access to it.
	lpszData = (LPSTR)GlobalLock(hGlobal);
	// Now get the size of the text on the clipboard.
	nSize = GlobalSize(hGlobal);

	// Make sure the text on the clipboard is not longer
	// that the buffer that was allocated for it. If it was
	// snip the text on the clipboard so that it fits.
	if(nSize >= (UINT)nBufSize) nSize = nBufSize - 1;

	// Now, copy the text into the return buffer. At the
	// end, we need to add a NULL string terminator.

	UINT i;

	for (i = 0; i < nSize; ++i)
		*(lpszBuffer + i) = *(lpszData + i);
	*(lpszBuffer + i) = 0;

	// Now, simply unlock the global memory pointer
	// and close the clipboard.
	GlobalUnlock(hGlobal);
	CloseClipboard();


}