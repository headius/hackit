// SystemHooks.h: interface for the SystemHooks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMHOOKS_H__B6A30C63_5A87_11D3_8EB6_00E029270E87__INCLUDED_)
#define AFX_SYSTEMHOOKS_H__B6A30C63_5A87_11D3_8EB6_00E029270E87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef UINT (CALLBACK* HOOKSTARTER)(HWND);
typedef UINT (CALLBACK* HOOKSTOPPER)();
typedef UINT (CALLBACK* ENABLESYSMENUCAPTURE)(BOOL);

class SystemHooks  
{
public:
	HINSTANCE m_TrayManDLL;
	void Stop();
	void Start();
	void StartCallWndProcHooking();
	void StartCBTHooking();
	void StopCallWndProcHooking();
	void StopCBTHooking();
	BOOL m_CallWndProcHooking;
	BOOL m_ShellHooking;
	BOOL m_CBTHooking;
	HWND m_TargetWindow;
	SystemHooks(HWND targetWnd);
	virtual ~SystemHooks();

};

#endif // !defined(AFX_SYSTEMHOOKS_H__B6A30C63_5A87_11D3_8EB6_00E029270E87__INCLUDED_)
