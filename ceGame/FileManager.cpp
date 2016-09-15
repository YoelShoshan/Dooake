#include "stdafx.h"
#include "FileManager.h"

#include <Windows.h>
#include <string>
#include <vector>

#include "Defines.h"

///////////////////////////
//#include "ZedTracker.h"
///////////////////////////

CFileManager::CFileManager()
{
	m_pcFirstCurrentDirectory = NULL;
};

CFileManager::~CFileManager()
{
	//SafeDeleteArray(m_pcFirstCurrentDirectory);
	delete [] m_pcFirstCurrentDirectory;
};


bool CFileManager::FillFileList(char* Path,char* What, FILE_VECTOR& FileVector)
{
	WIN32_FIND_DATA find;

	std::string FullSearch;

	FullSearch = Path;
	FullSearch+= What;


	HANDLE hfind = FindFirstFile(FullSearch.c_str(),&find);

	std::string strPush;

	

	if (hfind!=INVALID_HANDLE_VALUE)
	{
		do {
			strPush = Path;
			strPush+= find.cFileName;
			FileVector.push_back(strPush.c_str());
		} 
		while(FindNextFile(hfind,&find));
	}
	else
	{
		return false;
	}

	return true;
}


/// returns false if:
/// 1. Cancel or exit was chosen
/// 2. a filename that doesn't contain cpExtension was manually written
/*bool CFileManager::UserInputFileDialog(char* cpExtension, char* cpFilter, char*& cpFillMe)
{
	char* TempPath = new char [300];
	ZeroMemory(TempPath,sizeof(char)*300);

	GetCurrentDirectory(299,TempPath);

	char* cpTemp = NULL;
	cpTemp = new char[256];
	ZeroMemory(cpTemp,sizeof(char)*256);
	
	OPENFILENAME OpenFileName={0};
	
	OpenFileName.lStructSize=sizeof(OPENFILENAME);
	OpenFileName.lpstrFile = cpTemp;
	OpenFileName.lpstrFile[0] = '\0';
	OpenFileName.nMaxFile = 255;

	OpenFileName.lpstrFilter = cpFilter;
	OpenFileName.nFilterIndex = 1;
	OpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	ShowCursor(true);  
	bool bTemp = GetOpenFileName(&OpenFileName);
	ShowCursor(false);  

	SetCurrentDirectory(TempPath);
	delete [] TempPath;

	if (strstr(cpTemp,cpExtension))
	{
		cpFillMe = new char[strlen(cpTemp)+1];
		strcpy(cpFillMe,cpTemp);	
		SafeDeleteArray(cpTemp);
	}
	else
	{
		SafeDeleteArray(cpTemp);
		return false;
	}

	return bTemp;
}*/

/*void CFileManager::WriteStringToFile(char* pString, char* FileName)
{
	std::ofstream t_File;
	t_File.open(FileName);
	t_File << pString;

   	//if(t_ScriptFile.fail())
	//{	
		// LOG: log here
		//cout << "ERROR: Could not find Stats.txt!\n";
//		return;										
	//}*

	t_File.close();

}*/

/*void CFileManager::WriteStringToFile(const char* pString, char* FileName)
{
	std::ofstream t_File;
	t_File.open(FileName);
	t_File << pString;

	
	//if(t_ScriptFile.fail())
	//{	
	// LOG: log here
	//cout << "ERROR: Could not find Stats.txt!\n";
	//return;										
	//}

	t_File.close();

}*/