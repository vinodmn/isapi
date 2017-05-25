#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _AFXDLL
#include <windows.h>
#else
#include <afx.h>
#endif

class FileMap  
{
public:
	FileMap();
	virtual ~FileMap();

	virtual PBYTE		Map(DWORD dwSize, DWORD dwProtect = PAGE_READWRITE);
	virtual PBYTE		GetBuffer();
	virtual DWORD		GetLength();
	virtual void		Close();

protected:

	HANDLE					m_hMap;
	PBYTE					m_pMap;
	DWORD					m_dwSize;
};
