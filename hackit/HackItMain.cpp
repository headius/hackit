#include <windows.h>
#include <commctrl.h>
#include <winbase.h>
#include <shellapi.h>
#include <crtdbg.h>
#include <string>
#include "WildCard.h"
#include "resource.h"
#include "HackItSettings.h"
#include "SystemHooks.h"
#include "HackItDlg.h"
#include "Splash.h"
#include <time.h>

int (__stdcall *MyEnumProcessModules)(HANDLE, HMODULE*, DWORD, LPDWORD) = NULL;
unsigned long (__stdcall *MyGetModuleFileNameEx)(HANDLE,HMODULE,char *,unsigned long) = NULL;
unsigned long (__stdcall *MyGetWindowModuleFileName)(HWND,char *,unsigned long) = NULL;
int (__stdcall *MySetLayeredWindowAttributes)(struct HWND__ *,unsigned long,unsigned char,unsigned long) = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HANDLE mutex;

	mutex = CreateMutex(NULL, TRUE, "HackItMutex");

	MyEnumProcessModules = (int (__stdcall *)(HANDLE, HMODULE*, DWORD, LPDWORD))::GetProcAddress(::LoadLibrary("psapi.dll"), "EnumProcessModules");
	MyGetModuleFileNameEx = (unsigned long (__stdcall *)(HANDLE,HMODULE,char *,unsigned long))::GetProcAddress(::GetModuleHandle("psapi.dll"), "GetModuleFileNameExA");
	MyGetWindowModuleFileName = (unsigned long (__stdcall *)(HWND,char *,unsigned long))::GetProcAddress(::GetModuleHandle("user32.dll"), "GetWindowModuleFileNameA");
	MySetLayeredWindowAttributes  = (int (__stdcall *)(struct HWND__ *,unsigned long,unsigned char,unsigned long))::GetProcAddress(::GetModuleHandle("user32.dll"), "SetLayeredWindowAttributes");

	if (GetLastError() != ERROR_ALREADY_EXISTS) {
		HackItDlg theDlg(hInstance);

		theDlg.Start();
	} else {
		HWND hackItWnd = ::FindWindow("HackIt", NULL);
		ShowWindow(hackItWnd, SW_SHOWNORMAL);
		::SetForegroundWindow(hackItWnd);
	}

	ReleaseMutex(mutex);

	_CrtDumpMemoryLeaks();

	/*string wild = "*sdf*rww*rlls*rtlso";
	string full = "sdfwerrdsrwwsrrrwdrlls23v445rtlso";

	char myTime[256];

	for (int i = 0; i < 100000; i++) {
		wildcard(wild, full, FNM_BASHSTYLE);
	}

	sprintf(myTime, "Time in ticks: %d\nTime in seconds: %d", clock(), clock() / CLOCKS_PER_SEC);
	MessageBox(NULL, myTime, "Time", MB_OK);*/

	return 0;
}
