// ProcessFunctions.h: interface for the ProcessFunctions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSFUNCTIONS_H__35727863_A82F_11D3_8EB8_00E029270E87__INCLUDED_)
#define AFX_PROCESSFUNCTIONS_H__35727863_A82F_11D3_8EB8_00E029270E87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ProcessFunctions  
{
public:
	ProcessFunctions();
	virtual ~ProcessFunctions();

	static void EndTask( HWND TempWindow );
	static void SetWindowPriority( HWND TempWindow , DWORD priority );
};

#endif // !defined(AFX_PROCESSFUNCTIONS_H__35727863_A82F_11D3_8EB8_00E029270E87__INCLUDED_)
