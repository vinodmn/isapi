// MultipartParser.cpp: implementation of the MultipartParser class.
#include "MultipartParser.h"
#include "MultipartEntry.h"

// Safe strlen routine...

int StrLen(const char *p)
{
	int	iResult = 0;

	__try
	{
		if(p != NULL)
			iResult = ::strlen(p);
	}
	__except(TRUE)
	{
		iResult = 0;
	}

	return iResult;
}

// Construction/Destruction

MultipartParser::MultipartParser()
:  m_pBound(NULL),
   m_nSizeOfBound(0),
	m_pBuffer(NULL),
	m_ulBuffer(0),
	m_ulCurrent(0)
{

}

MultipartParser::~MultipartParser()
{
	Clear();
}

void MultipartParser::Clear()
{
	ClearVector();
	if(m_pBound != NULL)
	{
		delete [] m_pBound;
		m_pBound = NULL;
	}

	if(m_pBuffer != NULL)
	{
		if(m_ulBuffer >= 65534)
			m_FileMap.Close();
		else
			delete [] m_pBuffer;
		m_pBuffer = NULL;
		m_ulBuffer = 0;
		m_ulCurrent = 0;
	}
}

const char *MultipartParser::GetBoundry() const
{
	return m_pBound;
}

void MultipartParser::SetBoundry(const char *szBound)
{
	if(m_pBound != NULL)
	{
		delete [] m_pBound;
		m_pBound = NULL;
	}

	if(szBound != NULL)
	{
		int nSizeOfBound = StrLen(szBound);

		if(nSizeOfBound > 0)
		{
			m_nSizeOfBound = nSizeOfBound;
			m_pBound = new char[m_nSizeOfBound + 2];
			::ZeroMemory(m_pBound, m_nSizeOfBound + 2);
			::CopyMemory(m_pBound, szBound, m_nSizeOfBound);
		}
	}
}

void MultipartParser::Reserve(unsigned long ulSize)
{
	Clear();

	m_ulBuffer = ulSize;
	if(m_ulBuffer >= 65534)
		m_pBuffer = (PBYTE)m_FileMap.Map((DWORD) m_ulBuffer + 2);
	else
		m_pBuffer = new BYTE[m_ulBuffer + 2];

	::ZeroMemory(m_pBuffer, m_ulBuffer + 2);
	m_ulCurrent = 0;
}

BOOL MultipartParser::Add(const PBYTE pBuffer, unsigned long ulSize)
{
	if((m_ulCurrent + ulSize) <= m_ulBuffer)
	{
		::CopyMemory(m_pBuffer + m_ulCurrent, pBuffer, ulSize);
		m_ulCurrent += ulSize;
		return TRUE;
	}
	else
		return FALSE;
}

int MultipartParser::Parse()
{
	if(m_ulCurrent > 0)
		return Set(m_pBuffer, m_ulCurrent);
	else
		return 0;
}

int MultipartParser::Set(const PBYTE pBuf, int nSizeOfBuf)
{
	int					nLastOffset		= 0;
	int					nOffset			= 1;
	int					nTotalOffset	= 0;
	int					nSize;
	int					nIndex			= 0;
	MultipartEntry		*pEntry;

	ClearVector();

	// Chop off the first part.  The first part is slightly unique
	// since the starting boundry is part of the data.  That's why
	// the first chunk has a slack of 1 byte.  This ensures that
	// we don't pick up the starting boundry.
	nOffset = _Chunk(pBuf, nSizeOfBuf, 1);
	if(nOffset != -1)
	{
		nTotalOffset += nOffset;
		nSize = nTotalOffset - nLastOffset;
		pEntry = new MultipartEntry(nIndex++, &m_Map);
		pEntry->Set(pBuf + nLastOffset, nSize);
		m_Vector.push_back(pEntry);
		nLastOffset = nTotalOffset;
	}

	// Now iterate through the  rest of the buffer.
	while(nOffset != -1)
	{
		nTotalOffset += m_nSizeOfBound;
		nTotalOffset += 2;
		nOffset = _Chunk(pBuf + nTotalOffset, nSizeOfBuf - nTotalOffset);
		if(nOffset != -1)
		{
			nTotalOffset += nOffset;
			nSize = nTotalOffset - nLastOffset;
			pEntry = new MultipartEntry(nIndex++, &m_Map);;
			pEntry->Set(pBuf + nLastOffset, nSize);
			m_Vector.push_back(pEntry);
			nLastOffset = nTotalOffset;
		}
	}

	return m_Vector.size();
}

void MultipartParser::ClearVector()
{
	// We can't just empty out the vector.  We have to 
	// iterate through the vector and delete each pointer
	// first.
	MultipartVecItr	Itr;

	for(Itr = m_Vector.begin(); Itr != m_Vector.end(); Itr++)
		delete *Itr;

	m_Vector.clear();

	m_Map.clear();
}

MultipartEntry* MultipartParser::operator[](int iIndex)
{
	if(iIndex > (m_Vector.size() - 1))
		throw new OutOfRange("Out Of Range!");

	return m_Vector.at(iIndex);
}

MultipartEntry* MultipartParser::operator[](const char *szName)
{
	MultipartMapItr	itrMap = m_Map.find(std::string(szName));
	
	if(itrMap != m_Map.end())
		return (*itrMap).second;
	else
		return NULL;
}

int MultipartParser::_Chunk(const PBYTE pBuf, int nSizeOfBuf, int nSlack)
{
	// Find the boundry of the current part of the multipart
	// data.
	int	nOffset = nSlack;

	// Basically start from the current buffer pointer (plus an
	// offset--which should be zero for the most part) and walk
	// the buffer.  This is probably not the best way to handle 
	// this but I'm lazy.
	while(m_nSizeOfBound <= nSizeOfBuf)
	{
		if((nOffset + m_nSizeOfBound) >= nSizeOfBuf)
			break;

		if(memcmp(pBuf + nOffset, m_pBound, m_nSizeOfBound) == 0)
			return nOffset;
		else
			nOffset++;
	}

	return -1;
}


int	MultipartParser::GetSize()
{

	return m_Vector.size() - 1;
}