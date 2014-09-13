//********************************************
// Array3d.h
//********************************************
// class CArray3d
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 30/01/98
// Modified : 11/03/98
//********************************************

#include "..\stdafx.h"

//********************************************
// FreeExtra
//********************************************
template<class T>
void CArray3d<T>::FreeExtra()
{

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ASSERT(m_nSize <= SIZE_T_MAX/sizeof(T*));
#endif
		T** pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (T**) new BYTE[m_nSize * sizeof(T*)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(T*));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}


  


//** EOF **

