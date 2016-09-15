#ifndef CRIMSON_DEFENITION_FILE_H
#define CRIMSON_DEFENITION_FILE_H

#pragma once

#include <map>
#include <string>

class CDefFile
{
public:
	CDefFile();
	~CDefFile();

	bool ProcessFile(const char* pcFileName);
	const char* GetParameter(const char* pcKey);
private:

	std::map<std::string, std::string> m_Parameters;

};







#endif