#ifndef CRIMSON_ENGINE_OPTIONS_H
#define CRIMSON_ENGINE_OPTIONS_H

#pragma once


class IEngineOptions 
{
public:
	IEngineOptions() {};
	virtual ~IEngineOptions() {};

	virtual bool GetDepthOfField() = 0;
	

	virtual bool GetRendRTTBox() = 0;

	virtual bool GetFullFallback() = 0;

	virtual bool GetWindowed() = 0;

	virtual unsigned int GetScreenWidth() = 0;
	virtual unsigned int GetScreenHeight() = 0;
	virtual unsigned int GetScreenFrequency() = 0;

	// HACK - doesn't belong in engine - i need to add level choosing in the game
	virtual	const char* GetDesiredLevel() = 0;

};

class CEngineOptions : public IEngineOptions
{
public:
	CEngineOptions() {m_pcDesiredLevel=NULL;};
	~CEngineOptions() {delete [] m_pcDesiredLevel;};

	bool GetDepthOfField() { return false;};

	bool GetRendRTTBox() { return bRenderRTTBox;};

	bool GetFullFallback() { return bFullFallback;};

	bool GetWindowed() { return bWindowed;};

	unsigned int GetScreenWidth()  {return uiScreenWidth;};
	unsigned int GetScreenHeight() {return uiScreenHeight;};
	unsigned int GetScreenFrequency()  {return uiScreenFrequency;};

	const char* GetDesiredLevel() {return m_pcDesiredLevel;};


	bool bWindowed;
	bool bDepthOfField;		
	bool bRenderRTTBox;
	bool bTotalFallback;
	bool bFullFallback;
	
	unsigned int uiScreenWidth;
	unsigned int uiScreenHeight;
	unsigned int uiScreenFrequency;

	char* m_pcDesiredLevel;
};






#endif