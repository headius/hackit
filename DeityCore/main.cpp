#include <windows.h>
#include "ToolsMain.h"
#include "../DeityHook/HookMain.h"
//#include <vector>

#define WINDOWMAP_HASHSIZE 1000
#define LISTENERMAP_HASHSIZE 10

HINSTANCE g_hInstance;
HWND g_hWnd;
LPSTR g_lpCommandLine;

LPSTR deityCoreClass = "DeityCoreClass";
LPSTR deityCoreTitle = "DeityCoreWindow";

//std::vector<HWND> g_listeners = new std::vector<HWND>();
DEITY_MAP* windowMap;
DEITY_MAP* listenerMap;

LRESULT CALLBACK DeityCoreWndProc(HWND, UINT, WPARAM, LPARAM);
void OnCreate(WPARAM, LPARAM);
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

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void OnCreate(WPARAM wParam, LPARAM lParam) {
	windowMap = DeityMapCreate(WINDOWMAP_HASHSIZE);
}
