// ProcessFunctions.cpp: implementation of the ProcessFunctions class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "ProcessFunctions.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ProcessFunctions::ProcessFunctions()
{

}

ProcessFunctions::~ProcessFunctions()
{

}

void ProcessFunctions::SetWindowPriority(HWND TempWindow, DWORD priority)
{
	DWORD TempProcess;
	HANDLE process;
	
	GetWindowThreadProcessId(TempWindow, &TempProcess);
	process = OpenProcess(PROCESS_SET_INFORMATION, FALSE, TempProcess);

	if (SetPriorityClass(process, priority)) {
	} else {
		//QuickMessage("Error in set priority");
	}

	CloseHandle(process);
}

void ProcessFunctions::EndTask(HWND TempWindow)
{
	DWORD TempProcess;
	HANDLE process;
	
	GetWindowThreadProcessId(TempWindow, &TempProcess);
	process = OpenProcess(PROCESS_TERMINATE, FALSE, TempProcess);

	TerminateProcess((HANDLE)process, 0);

	CloseHandle(process);
}
