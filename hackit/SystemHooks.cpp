// SystemHooks.cpp: implementation of the SystemHooks class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable: 4786)
#include <windows.h>
#include <string>
#include "SystemHooks.h"
#include "trayman/trayman.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SystemHooks::SystemHooks(HWND targetWnd)
{
	m_TargetWindow = targetWnd;

	StartCBTHooking();
	StartCallWndProcHooking();
}

SystemHooks::~SystemHooks()
{
	if (m_CBTHooking) {
		StopCBTHook();
		m_CBTHooking = FALSE;
	}

	if (m_CallWndProcHooking) {
		StopCallWndProcRetHook();
		m_CallWndProcHooking = FALSE;
	}
}

void SystemHooks::StartCBTHooking()
{
	StartCBTHook(m_TargetWindow);
	m_CBTHooking = TRUE;
}

void SystemHooks::StartCallWndProcHooking()
{
	StartCallWndProcRetHook(m_TargetWindow);
	m_CallWndProcHooking = TRUE;
}

void SystemHooks::StopCBTHooking()
{
	StopCBTHook();
	m_CBTHooking = FALSE;
}

void SystemHooks::StopCallWndProcHooking()
{
	//if (m_TrayManDLL) {
	//	HOOKSTOPPER StopCallWndProcRetHooking = (HOOKSTOPPER)GetProcAddress(m_TrayManDLL, "StopCallWndProcRetHook");
		//if (StopCallWndProcRetHooking) {
			StopCallWndProcRetHook();
			m_CallWndProcHooking = FALSE;
		//}
	//}
}

void SystemHooks::Start()
{
	/*if (!(m_TrayManDLL = LoadLibrary("TrayMan.dll"))) {
		LPVOID lpMsgBuf;
		string message;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
		);

		message += "TrayMan.dll could not be loaded. The error message is:\n\n\t";
		message += (char*)lpMsgBuf;
		message += "\nWithout TrayMan.dll, the window list will not update, and windows will not minimize to the tray.";

		// Free the buffer.
		LocalFree( lpMsgBuf );

		::MessageBox(m_TargetWindow, message.c_str(), "Error enabling TrayMan.dll", MB_OK);
	}*/
}

void SystemHooks::Stop()
{
	/*if (m_TrayManDLL) {
		if (m_CBTHooking) {
			HOOKSTOPPER StopCBTHooking = (HOOKSTOPPER)GetProcAddress(m_TrayManDLL, "StopCBTHook");
			if (StopCBTHooking) {
				StopCBTHooking();
				m_CBTHooking = FALSE;
			}
		}

		if (m_CallWndProcHooking) {
			HOOKSTOPPER StopCallWndProcRetHooking = (HOOKSTOPPER)GetProcAddress(m_TrayManDLL, "StopCallWndProcRetHook");
			if (StopCallWndProcRetHooking) {
				StopCallWndProcRetHooking();
				m_CallWndProcHooking = FALSE;
			}
		}

		FreeLibrary(m_TrayManDLL);
		m_TrayManDLL = NULL;
	}*/
}
