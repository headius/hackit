// WindowListService.h: interface for the WindowListService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWLISTSERVICE_H__94B51482_66DB_425F_B2A5_7500DA399AAF__INCLUDED_)
#define AFX_WINDOWLISTSERVICE_H__94B51482_66DB_425F_B2A5_7500DA399AAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class WindowListService  
{
public:
	void Handle(DWORD opcode);
	void Start(DWORD argc, LPTSTR *argv);
	static void WINAPI ServiceControlHandler(DWORD opcode);
	static void WINAPI ServiceStart(DWORD argc, LPTSTR *argv);
	WindowListService();
	virtual ~WindowListService();

protected:
	SERVICE_STATUS_HANDLE m_StatusHandle;
	SERVICE_STATUS m_Status;
};

#endif // !defined(AFX_WINDOWLISTSERVICE_H__94B51482_66DB_425F_B2A5_7500DA399AAF__INCLUDED_)
