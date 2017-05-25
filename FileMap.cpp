// FileMap.cpp: implementation of the FileMap class.
//
//////////////////////////////////////////////////////////////////////

#include "FileMap.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FileMap::FileMap()
:  m_hMap(NULL),
   m_pMap(NULL),
	m_dwSize(0)
{

}

FileMap::~FileMap()
{
	Close();
}

void FileMap::Close()
{
	if(m_pMap != NULL)
	{
		UnmapViewOfFile((LPCVOID) m_pMap);
		m_pMap = NULL;
	}

	if(m_hMap != NULL)
	{
		CloseHandle(m_hMap);
		m_hMap = NULL;
	}

	m_dwSize = 0;
}

PBYTE FileMap::Map(DWORD dwSize, DWORD dwProtect)
{
	Close();

	m_dwSize = dwSize;

	m_hMap = ::CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, dwProtect,
		0, dwSize, NULL);

	if(m_hMap != NULL)
	{
		m_pMap = reinterpret_cast<PBYTE>(::MapViewOfFile(m_hMap, FILE_MAP_WRITE, 0, 0, 0));
		if(m_pMap == NULL)
			Close();
	}
 
	return m_pMap;
}

PBYTE FileMap::GetBuffer()
{
	return m_pMap;
}

DWORD FileMap::GetLength()
{
	return m_dwSize;
}
