#ifndef CRIMSON_COMPRESS_DUMMY_H
#define CRIMSON_COMPRESS_DUMMY_H

#pragma once

#include "Compress.h"

class CCompressDummy : public ICompress
{
public:
	CCompressDummy();
	~CCompressDummy();


	// use immidiately, previous compress result will be overriden!
	tCompressedData Compress(const char* pCompressMe, int iSize);

private:
	int   m_iBufferSize;
	char *m_pBuffer;
};



#endif