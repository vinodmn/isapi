// MultipartEntry.h: interface for the MultipartEntry class.

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning(disable : 4786)

#include "FileMap.h"
#include <vector>
#include <map>
#include <exception>

class MultipartEntry;

// Need a less<> operator for the STL map<>.  
// This is done case-insensitive.

struct StrLess
{
	bool operator()(const std::string &s1, const std::string &s2) const 
	{ 
		return _stricmp(s1.c_str(), s2.c_str()) < 0; 
	};
};

// Some typedefs so we can maintain sanity in our code.
typedef std::string												String;
typedef std::map<String, MultipartEntry*, StrLess>		MultipartMap;
typedef MultipartMap::iterator								MultipartMapItr;
typedef std::vector<MultipartEntry*>						MultipartVector;
typedef MultipartVector::iterator							MultipartVecItr;
typedef std::out_of_range										OutOfRange;

// Within a multipart form, there should be one or more MultipartEntry.
// This class will represent one part of a multipart form.
class MultipartEntry  
{
public:
	MultipartEntry(int nIndex, MultipartMap *pMap, int nMax = 65536);
	virtual ~MultipartEntry();
	
	virtual  void		Set(const PBYTE, int);
	virtual	int		Find(const PBYTE, int);
	BYTE		&operator[](int iIndex);

	virtual  PBYTE		Buffer() const;
	virtual  int		BufferLength() const;
	virtual  PBYTE		Data() const;
	virtual  int		DataLength() const;
	virtual  LPCTSTR	Name() const;
	virtual  int		NameLength() const;
protected:
	virtual	void		_Clear();

	PBYTE				m_pBuffer;
	int					m_nBuffer;
	PBYTE				m_pData;
	int					m_nData;
	FileMap				m_FileMap;
	int					m_MaxAlloc;
	int					m_nIndex;
	String				m_sName;
	MultipartMap		*m_pMap;
};
