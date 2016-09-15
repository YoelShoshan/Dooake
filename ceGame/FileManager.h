#ifndef YOEL_FILE_H
#define YOEL_FILE_H

#pragma once

//#include <Windows.h>
#include <windows.h>
#include <string>
#include <vector>

typedef std::vector<std::string> FILE_VECTOR;

class CFileManager
{
public:
	CFileManager();
	~CFileManager();
	//TODO: remember that i should make this singleton
	bool FillFileList(char* Path,char* What, FILE_VECTOR& FileVector);
	//bool UserInputFileDialog(char* cpExtension, char* cpFilter, char*& cpFillMe);
	/*void WriteStringToFile(char* pString, char* FileName);
	void WriteStringToFile(const char* pString, char* FileName);*/

	char* m_pcFirstCurrentDirectory;
};





#endif // YOEL_FILE_H