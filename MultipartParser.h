// MultipartParser.h: interface for the MultipartParser class.

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MultipartEntry.h"

class MultipartParser  
{
public:
	MultipartParser();
	virtual ~MultipartParser();

	virtual void			Clear();

	virtual void			SetBoundry(const char* szBound);
	virtual const char*	GetBoundry() const;

	virtual void			Reserve(unsigned long);
	virtual BOOL			Add(const PBYTE, unsigned long);
	virtual int				Parse();

	virtual int				Set(const PBYTE pBuf, int nSizeOfBuf);
	virtual void			ClearVector();

	// This will throw an out_of_range exception object if the index
	// is out of range.
	MultipartEntry*		operator[](int);

	// This will not.
	MultipartEntry*		operator[](const char*);

	virtual int				GetSize();

protected:
	virtual int				_Chunk(const PBYTE pBuf, int nSizeOfBuf, int nSlack = 0);
	MultipartVector		m_Vector;
	MultipartMap			m_Map;
	char*						m_pBound;
	int						m_nSizeOfBound;
	PBYTE						m_pBuffer;
	unsigned long			m_ulBuffer;
	unsigned long			m_ulCurrent;
	FileMap					m_FileMap;
};
