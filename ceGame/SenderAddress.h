#ifndef CRIMSON_SENDER_ADRESS_H
#define CRIMSON_SENDER_ADRESS_H

#pragma once

class CSenderAddress
{
public:
	unsigned char  m_IP[4];
	unsigned short m_Port;

bool operator < (const CSenderAddress& against) const
{
	DWORD dwSelfIP = (*(DWORD*)&m_IP);
	DWORD dwAgainstIP = (*(DWORD*)&against.m_IP);

	if (dwSelfIP<dwAgainstIP)
		return true;

	if (dwAgainstIP<dwSelfIP)
		return false;

	// if we're here then dwAgainstIP==dwSelfIP so now port will determine
	if (m_Port<against.m_Port)
		return true;

	return false;
}

bool operator == (const CSenderAddress& against)
{
	if (m_IP[0]==against.m_IP[0] &&
		m_IP[1]==against.m_IP[1] &&
		m_IP[2]==against.m_IP[2] &&
		m_IP[3]==against.m_IP[3] &&
		m_Port==against.m_Port)
			return true;

	return false;
}

};


#endif