#include <windows.h>
#include "../DeityTools/ToolsMain.h"
#include "../DeityHook/HookMain.h"
//#include <vector>

#define WINDOWMAP_HASHSIZE 1000
#define LISTENERMAP_HASHSIZE 10
#define DEITY_LOGFILE "C:\\deity.log"

HINSTANCE g_hInstance;
HWND g_hWnd;
LPSTR g_lpCommandLine;

LPSTR deityCoreClass = "DeityCoreClass";
LPSTR deityCoreTitle = "DeityCoreWindow";

//std::vector<HWND> g_listeners = new std::vector<HWND>();
DEITY_MAP* windowMap;
DEITY_MAP* listenerMap;

HANDLE logFile = NULL;

LRESULT CALLBACK DeityCoreWndProc(HWND, UINT, WPARAM, LPARAM);
void OnCreate(WPARAM, LPARAM);
void AddWindow(HWND);
void RemoveWindow(HWND);
void DeityDisplayError(LPSTR);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCommandLine, int nCmdShow) {
	g_hInstance = hInstance;
	g_lpCommandLine = lpCommandLine;

	WNDCLASS *wndClass = new WNDCLASS();
	OSVERSIONINFO *osVersionInfo = new OSVERSIONINFO();

	// zero them out
	memset(wndClass, 0, sizeof(WNDCLASS));
	memset(osVersionInfo, 0, sizeof(OSVERSIONINFO));

	GetVersionEx(osVersionInfo);

	wndClass->lpszClassName = deityCoreClass;
	wndClass->lpfnWndProc = DeityCoreWndProc;
	wndClass->hInstance = hInstance;

	ATOM result = RegisterClass(wndClass);

	if (result == NULL) {
		DeityDisplayError("RegisterClass");
		return 0;
	}
	
	g_hWnd = CreateWindow(
		deityCoreClass,
		deityCoreTitle,
		WS_POPUP,
		0, 0,
		0, 0,
		(osVersionInfo->dwMajorVersion >= 5 ? HWND_MESSAGE : NULL),
		NULL,
		hInstance,
		NULL);

	if (g_hWnd == NULL) {
		DeityDisplayError("CreateWindow");
		return 0;
	}

	// free up a few bytes
	delete osVersionInfo;
	delete wndClass;

	logFile = CreateFile(DEITY_LOGFILE, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!logFile) {
		DeityDisplayError("Creating log file");
	}

	// start hooks
	StartCBTHook();
	//StartCallWndProcRetHook();

	MSG msg;

	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK DeityCoreWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
			OnCreate(wParam, lParam);
			break;
		case DM_CREATEDETECTED:
			AddWindow((HWND)wParam);
			break;
		case DM_DESTROYDETECTED:
			RemoveWindow((HWND)wParam);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void OnCreate(WPARAM wParam, LPARAM lParam) {
	windowMap = DeityMapCreate(WINDOWMAP_HASHSIZE);
}

void AddWindow(HWND wnd) {
	DEITY_WINDOW* dWindow = (DEITY_WINDOW*)DeityAlloc(sizeof(DEITY_WINDOW));
	dWindow->handle = wnd;

	char className[256];

	GetClassName(wnd, className, 256);
	
	if (CompareString(
			LOCALE_USER_DEFAULT,
			NORM_IGNOREWIDTH,
			className,
			lstrlen(className),
			"Notepad",
			lstrlen("Notepad")) == CSTR_EQUAL) {
		CloseWindow(wnd);
	}

	DEITY_NODE* node = DeityNodeCreate();
	DeityNodeSetElement(node, (int)dWindow);
	DeityNodeSetHash(node, (int)wnd);
	DeityMapPut(windowMap, node);

	DWORD bytes;
}

void RemoveWindow(HWND wnd) {
	DEITY_NODE* node = DeityMapGet(windowMap, (int)wnd);

	if (node != NULL) {
		DeityMapRemove(windowMap, (int)wnd);
	}

	DeityFree(node);
}