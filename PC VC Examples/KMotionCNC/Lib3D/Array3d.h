//********************************************
// Array3d.h
// class CArray3d :
//********************************************
// Careful : this array stores pointers
// on elements, but does not free memory
// in destructors, you must call array.Free()
// to do this.
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 30/01/98
// Modified : 18/02/98
//********************************************

#ifndef _ARRAY_3D_
#define _ARRAY_3D_

template<class T>
class CArray3d
{

// Implementation
private:
	T** m_pData;     // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

public:

	// Construction
	CArray3d()
	{
		m_pData = NULL;
		m_nSize = 0;
		m_nMaxSize = 0;
		m_nGrowBy = 0;
	}

	// Destruction
	~CArray3d()
		{
		delete [] (BYTE*)m_pData;
		}

	// Attributes
	int GetSize() { return m_nSize; }
	int GetUpperBound() { return m_nSize-1; }

	// Operations

	// Really clean up
	void Free()
	{
		for(int i=0;i<m_nSize;i++)
			if(m_pData[i] != NULL)
			{
				delete m_pData[i];
			  m_pData[i] = NULL;
			}
		RemoveAll();
	}

	// Clean up pointers array
	void FreeExtra();

	// Clean up pointers array
	void RemoveAll() { SetSize(0); }

	// Accessing elements
	T* GetAt(int nIndex)
	{ 
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; 
	}

	// SetAt
	void SetAt(int nIndex, T* newElement)
	{ 
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; 
	}

	// Has
	int Has(T* pElement)
	{
		int size = GetSize();
		for(int i=0;i<size;i++)
			if(GetAt(i) == pElement)
				return 1;
		return 0;
	}
	
	// Direct Access to the element data (may return NULL)
	T** GetData()	{ return m_pData; }

	// Add
	int Add(T* newElement)
	{ 
		int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; 
	}

	// overloaded operator helpers
	T* operator[](int nIndex) { return GetAt(nIndex); }

	// Potentially growing the array
	void SetAtGrow(int nIndex, T* newElement)
	{
		ASSERT(nIndex >= 0);

		if (nIndex >= m_nSize)
			SetSize(nIndex+1);
		m_pData[nIndex] = newElement;
	}

	// Operations that move elements around
	void InsertAt(int nIndex, T* newElement, int nCount = 1)
	{
		ASSERT(nIndex >= 0);    // will expand to meet need
		ASSERT(nCount > 0);     // zero or negative size not allowed

		if (nIndex >= m_nSize)
		{
			// adding after the end of the array
			SetSize(nIndex + nCount);  // grow so nIndex is valid
		}
		else
		{
			// inserting in the middle of the array
			int nOldSize = m_nSize;
			SetSize(m_nSize + nCount);  // grow it to new size
			// shift old data up to fill gap
			memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
				(nOldSize-nIndex) * sizeof(T*));

			// re-init slots we copied from

			memset(&m_pData[nIndex], 0, nCount * sizeof(T*));

		}

		// insert new value in the gap
		ASSERT(nIndex + nCount <= m_nSize);
		while (nCount--)
			m_pData[nIndex++] = newElement;
	}

	void RemoveAt(int nIndex, int nCount = 1)
	{
		ASSERT(nIndex >= 0);
		ASSERT(nCount >= 0);
		ASSERT(nIndex + nCount <= m_nSize);

		// just remove a range
		int nMoveCount = m_nSize - (nIndex + nCount);

		if (nMoveCount)
			memcpy(&m_pData[nIndex], &m_pData[nIndex + nCount],
				nMoveCount * sizeof(T*));
		m_nSize -= nCount;
	}

	// IndexFrom
	int IndexFrom(T* pElement)
	{
		for(int i=0;i<m_nSize;i++)
			if(m_pData[i] == pElement)
				return i;
		TRACE("IndexFrom : no element\n");
		return -1;
	}

	// SetSize
	void SetSize(int nNewSize, int nGrowBy = -1)
	{
		ASSERT(nNewSize >= 0);

		if(nGrowBy != -1)
			m_nGrowBy = nGrowBy;  // set new size
		
		// shrink to nothing
		if(nNewSize == 0)
		{
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
			m_nSize = 0;
			m_nMaxSize = 0;
		}
		else 
			// create one with exact size
			if(m_pData == NULL)
			{
				m_pData = (T**) new BYTE[nNewSize * sizeof(T*)];
				memset(m_pData, 0, nNewSize * sizeof(T*));  // zero fill
				m_nSize = nNewSize;
				m_nMaxSize = nNewSize;
			}
			else 
				if(nNewSize <= m_nMaxSize)
				{
					// it fits
					if (nNewSize > m_nSize)
					{
						// initialize the new elements
						memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(T*));
					}
					m_nSize = nNewSize;
				}
				else
				{
					// otherwise, grow array
					int nGrowBy = m_nGrowBy;
					if (nGrowBy == 0)
					{
						// heuristically determine growth when nGrowBy == 0
						//  (this avoids heap fragmentation in many situations)
						//  nGrowBy = min(1024, max(4, m_nSize / 8));

						// TK grow exponentially to avoid n^2 copies
						nGrowBy = (m_nSize * 3)/2;
					}
					int nNewMax;
					if (nNewSize < m_nMaxSize + nGrowBy)
						nNewMax = m_nMaxSize + nGrowBy;  // granularity
					else
						nNewMax = nNewSize;  // no slush
					
					ASSERT(nNewMax >= m_nMaxSize);  // no wrap around

					T** pNewData = (T**) new BYTE[nNewMax * sizeof(T*)];
					
					// copy new data from old
					memcpy(pNewData, m_pData, m_nSize * sizeof(T*));
					
					// construct remaining elements
					ASSERT(nNewSize > m_nSize);
					
					memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(T*));
					
					// get rid of old stuff (note: no destructors called)
					delete[] (BYTE*)m_pData;
					m_pData = pNewData;
					m_nSize = nNewSize;
					m_nMaxSize = nNewMax;
				}
	}

};



#endif // _ARRAY_3D_
