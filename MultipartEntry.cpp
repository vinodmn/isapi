// MultipartEntry.cpp: implementation of the MultipartEntry class.

#include "MultipartEntry.h"
#include <string>

// Construction/Destruction

MultipartEntry::MultipartEntry(int nIndex, MultipartMap *pMap, int nMax)
:  m_pBuffer(NULL),
	m_pData(NULL),
   m_nBuffer(0),
	m_nData(0),
	m_MaxAlloc(nMax),
	m_nIndex(nIndex),
	m_pMap(pMap)
{
}

MultipartEntry::~MultipartEntry()
{
	_Clear();
}

void MultipartEntry::_Clear()
{
	//Free up all resources.
	if(m_pBuffer != NULL)
	{
		if(m_nBuffer >= m_MaxAlloc)
			m_FileMap.Close();
		else
			delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
}



void MultipartEntry::Set(const PBYTE pBuf, int nSize)
{
	_Clear();

	// If the file size is larger than m_MaxAlloc, we should use file mapping
	// objects for our backing store.  Otherwise, just allocate it in
	// memory.  
	m_nBuffer = nSize;
	if(nSize >= m_MaxAlloc)
		m_pBuffer = (PBYTE)m_FileMap.Map((DWORD) m_nBuffer + 2);
	else
		m_pBuffer = new BYTE[m_nBuffer + 2];

	::ZeroMemory(m_pBuffer, m_nBuffer + 2);
	::CopyMemory(m_pBuffer, pBuf, m_nBuffer);

	// By the spec (RFC1867), each multipart will have a boundry,
	// and content information and the actual data.  The seperator
	// between the content information and data is \r\n\r\n.  So
	// we search for it set our data pointer to the address right
	// after the sToken.
	String	sToken("\r\n\r\n");

	int nIndex = Find((const PBYTE)(LPCTSTR)sToken.c_str(), sToken.size());

	if(nIndex != -1)
	{
		nIndex += sToken.size();
		m_pData = m_pBuffer + nIndex;
		m_nData = m_nBuffer - nIndex;
		// We need to account for the extra two bytes that are tacked
		// on at the end of each data portion.  Each data portion has
		// the following tacked on at the end:
		// \r\n<boundry>
		// We've accounted for the size of the <boundry> but now we need
		// to account for the \r\n.
		m_nData -= 2;

		// Find the name of the Content.
		sToken = "name=\"";
		nIndex = Find((const PBYTE)(LPCTSTR)sToken.c_str(), sToken.size());
		if(nIndex == -1)
		{
			char tmp[10];
			m_sName = ::_itoa_s(m_nIndex, tmp, 10);
		}
		else
		{
			nIndex += sToken.size();
			m_sName.assign((const char*)(m_pBuffer + nIndex), 256);
			nIndex = m_sName.find("\"");
			m_sName = m_sName.substr(0, nIndex);
		}

		m_pMap->insert(MultipartMap::value_type(m_sName, this));
	}
	else
	{
		m_pData = NULL;
		m_nData = 0;
	}
}

int MultipartEntry::Find(const PBYTE pBuf, int nBuf)
{
	// A simple wrapper to find an arbitrary sequence of
	// bytes from the internal buffer.  If it finds it,
	// return the index, else return -1.
	for(int i = 0; i < (m_nBuffer - nBuf); i++)
	{
		if(memcmp(m_pBuffer + i, pBuf, nBuf) == 0)
			return i;
	}

	return -1;
}

BYTE &MultipartEntry::operator[](int iIndex)
{
	// Have this behave exactly as if it was a type std::vector<>
	// by throwing a out_of_range() exception if the index is 
	// outside the size of the buffer.  Otherwise, return the byte
	// at the index.
	if(iIndex < 0 || iIndex > m_nBuffer - 1)
		throw new OutOfRange(String("Index out of range of buffer"));
	
	return m_pBuffer[iIndex];
}

PBYTE MultipartEntry::Buffer() const
{
	return m_pBuffer;
}

int MultipartEntry::BufferLength() const
{
	return m_nBuffer;
}

PBYTE MultipartEntry::Data() const
{
	return m_pData;
}

int MultipartEntry::DataLength() const
{
	return m_nData;
}

LPCTSTR MultipartEntry::Name() const
{
	return m_sName.c_str();
}

int MultipartEntry::NameLength() const
{
	return m_sName.size();
}