// WindowListService.cpp: implementation of the WindowListService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WindowListService.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern WindowListService *g_WLS;

WindowListService::WindowListService()
{

}

WindowListService::~WindowListService()
{

}

void WINAPI WindowListService::ServiceStart(DWORD argc, LPTSTR *argv)
{
	g_WLS->Start(argc, argv);
}

void WINAPI WindowListService::ServiceControlHandler(DWORD opcode)
{
	g_WLS->Handle(opcode);
}

void WindowListService::Start(DWORD argc, LPTSTR *argv)
{
	DWORD status;
	DWORD specificError;

	m_Status.dwServiceType = SERVICE_WIN32_SHARE_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	m_Status.dwCurrentState = SERVICE_START_PENDING;
	m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	m_Status.dwWin32ExitCode = 0;
	m_Status.dwServiceSpecificExitCode = 0;
	m_Status.dwCheckPoint = 0;
	m_Status.dwWaitHint = 0;

	m_StatusHandle = RegisterServiceCtrlHandler(WINLIST_SERVICE_NAME, WindowListService::ServiceControlHandler);

	if (m_StatusHandle == (SERVICE_STATUS_HANDLE)0) {
		MessageBox(NULL, "blah", "Register service handler failed\n", MB_OK);
		return;
	}

	// initialize, handling errors

	// set to pending here if init takes long, etc
	// initialization complete
	m_Status.dwCurrentState = SERVICE_RUNNING;
	m_Status.dwCheckPoint = 0;
	m_Status.dwWaitHint = 5000;

	if (!SetServiceStatus(m_StatusHandle, &m_Status)) {
		//ErrorMessage();
	}

	// do work in service
	//MessageBox(NULL, "start", "start", MB_OK);

	return;
}

void WindowListService::Handle(DWORD opcode)
{
    DWORD status; 
 
    switch(opcode) 
    { 
        case SERVICE_CONTROL_PAUSE: 
        // Do whatever it takes to pause here. 
            m_Status.dwCurrentState = SERVICE_PAUSED; 
            break; 
 
        case SERVICE_CONTROL_CONTINUE: 
        // Do whatever it takes to continue here. 
            m_Status.dwCurrentState = SERVICE_RUNNING; 
            break; 
 
        case SERVICE_CONTROL_STOP: 
        // Do whatever it takes to stop here. 
            m_Status.dwWin32ExitCode = 0; 
            m_Status.dwCurrentState  = SERVICE_STOPPED; 
            m_Status.dwCheckPoint    = 0; 
            m_Status.dwWaitHint      = 0; 
 
            if (!SetServiceStatus (m_StatusHandle, 
                &m_Status))
            { 
                // output debug error
            } 
 
            return; 
 
        case SERVICE_CONTROL_INTERROGATE: 
        // Fall through to send current status. 
            break; 
 
        default: 
			// output debug error, invalid opcode
			break;
    } 
 
    // Send current status. 
    if (!SetServiceStatus (m_StatusHandle,  &m_Status)) 
    { 
        // output error message
    } 
    return;
}
