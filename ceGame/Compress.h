#ifndef CRIMSON_COMPRESSION_H
#define CRIMSON_COMPRESSION_H

#pragma once

struct tCompressedData
{
	int iSize;
	char* pData;
};

class ICompress
{
public:
	ICompress() {};
	virtual ~ICompress() {};

	virtual tCompressedData Compress(const char* pCompressMe, int iSize) = 0;
};


#endif