#pragma	warning(disable: 4786)
#include <windows.h>
#include <objbase.h>
#include <initguid.h>
#include <commctrl.h>
#include <winuser.h>
#include <crtdbg.h>
#include <oleacc.h>
#include <string>
#include <htmlhelp.h>
#include "Wildcard.h"
#include "HackItSettings.h"
#include "SystemHooks.h"
#include "defines.h"
#include "resource.h"
#include "HackItDlg.h"
#include "Splash.h"
#include "AutoFunction.h"
#include "TextEntry.h"
#include "IllegalWindowSpec.h"
#include "SettingsDlg.h"
#include "LoggingSystem.h"
#include "WindowInfo.h"
#include "WindowFunctions.h"
#include "ProcessFunctions.h"

extern int (__stdcall *MySetLayeredWindowAttributes)(struct HWND__ *,unsigned long,unsigned char,unsigned long);
extern UINT WM_ShellHook;

DWORD WINAPI HackItDlg::ListThreadProc(LPVOID data) {
	WNDCLASSEX wndClass;
	HRESULT result;
	HackItDlg *dlg = (HackItDlg*)data;

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = NULL;
	wndClass.lpfnWndProc = HackItDlg::ListThreadWindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = dlg->m_hInstance;
	wndClass.hIcon = NULL;
	wndClass.hIconSm = NULL;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "HackItListManager";
	
	// register list manager window class for list manager thread
	result = RegisterClassEx(&wndClass);
	
	if(result == NULL) {
		char astring[STRING_BUFFER_SIZE];
		DWORD error = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, NULL, astring, STRING_BUFFER_SIZE, NULL);
		MessageBox(NULL, astring, astring, MB_OK);
	}

	// list manager window
	dlg->m_hWndManager = CreateWindow(
		"HackItListManager",
		"HackItListManager",
		WS_OVERLAPPED,
		0,
		0,
		1,
		1,
		NULL,
		NULL,
		dlg->m_hInstance,
		(LPVOID)dlg);

	if (dlg->m_hWndManager) {
		MSG msg;

		// start this thread's message pump
		while (GetMessage(&msg, dlg->m_hWndManager, 0, 0)) {
			// If we're no longer getting valid messages, quit
			//if (!msg.message) break;

			try {
				// else, translate and dispatch the message
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} catch (...) {
				LoggingSystem::log("Exception caught");
			}
		}
	}

	return 0;
}

LRESULT CALLBACK HackItDlg::ListThreadWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HackItDlg* theDlg = NULL;
	LRESULT returnCode = -1;
	CREATESTRUCT *pCreateStruct;

	// Set or get the dialog object properly
	if (uMsg == WM_CREATE) {
		pCreateStruct = (LPCREATESTRUCT)lParam;
		theDlg = (HackItDlg*)(pCreateStruct->lpCreateParams);
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
	} else {
		theDlg = (HackItDlg*)GetWindowLong(hWnd, GWL_USERDATA);
	}

	if (theDlg != NULL) {
		switch (uMsg) {
		case WM_DESTROY:
			theDlg->OnListThreadDestroy();
			break;
		case WM_CREATEDETECTED:
			theDlg->OnCreateDetected(wParam, lParam);
			break;
		case WM_DESTROYDETECTED:
			theDlg->OnDestroyDetected(wParam, lParam);
			break;
		case WM_POSCHANGEDDETECTED:
			theDlg->OnPosChangedDetected(wParam, lParam);
			break;
		case WM_SETTEXTDETECTED:
			theDlg->OnSetTextDetected(wParam, lParam);
			break;
		case WM_SYSMENUDETECTED:
			theDlg->OnSysMenuDetected(wParam, lParam);
			break;
		case WM_HACKITFUNCTIONDETECTED:
			theDlg->OnHackItFunctionDetected(wParam, lParam);
			break;
		case WM_SYSMENUENDED:
			theDlg->OnSysMenuEnded(wParam, lParam);
			break;
		case WM_TIMER:
			theDlg->OnTimer(wParam, lParam);
			break;
		case HM_ADDTRAYYEDITEM:
			theDlg->AddTrayyedItem((HWND)lParam);
			break;
		case HM_ADDTOPPEDITEM:
			theDlg->AddToppedItem((HWND)lParam);
			break;
		case HM_DELETETRAYYEDITEM:
			theDlg->DeleteTrayyedItem((HWND)lParam);
			break;
		case HM_DELETETOPPEDITEM:
			theDlg->DeleteToppedItem((HWND)lParam);
			break;
		case WM_TRAYMESSAGE2:
			returnCode = theDlg->OnCheckTrayyedList(wParam, lParam);
			break;
		default:
			if (uMsg == WM_ShellHook) {
				theDlg->HandleShellHook(wParam, lParam);
			}

			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
		
	if (returnCode == -1) {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	} else {
		return returnCode;
	}
}

void HackItDlg::HandleShellHook(WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam) & 0x00FF)
	{
	case HSHELL_WINDOWCREATED:
		{
			char className[256];
			GetClassName((HWND)lParam, className, 256);
			if (lstrcmp(className, "ConsoleWindowClass") == 0) {
				PostMessage(m_hWndManager, WM_CREATEDETECTED, 0, lParam);
			}
		}
		break;
	case HSHELL_WINDOWDESTROYED:
		break;
	case HSHELL_REDRAW:
		{
			char className[256];
			GetClassName((HWND)wParam, className, 256);
			if (lstrcmp(className, "ConsoleWindowClass") == 0) {
				PostMessage(m_hWndManager, WM_SETTEXTDETECTED, 0, lParam);
			}
		}
		break;
	}
}

LRESULT HackItDlg::OnTimer(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case WINDOW_LIST_CLEANER_TIMER:
		this->CleanList();
		break;
	}

	return 0;
}
LRESULT HackItDlg::OnListThreadDestroy()
{
	PostQuitMessage(0);
	return 0;
} 

HTREEITEM HackItDlg::AddWindow(HWND Window)
{
	return AddBasicItem(Window, TRUE);
}

void HackItDlg::DeleteWindow(HWND TempWindow)
{
	DeleteBasicItem(TempWindow, TRUE);
}

void HackItDlg::DeleteBasicItem(HWND TempWindow, BOOL deleteRelatedItems)
{
	HTREEITEM item = NULL;
	int iconIndex;
	HWND_HTREEITEM_map::iterator position;

	// Find the item in the window/item map
	EnterCriticalSection(&m_CS_WindowItemMap);
	if ((position = m_WindowItemMap.find(TempWindow)) != m_WindowItemMap.end() && position->second != NULL) {
		// Delete its children
		DeleteChildItems(position->second, deleteRelatedItems);

		// Delete its tree item
		TV_ITEM tvItem;
		tvItem.hItem = position->second;
		tvItem.mask = TVIF_IMAGE;
		TreeView_GetItem(m_WindowList, &tvItem);
		iconIndex = tvItem.iImage;
		PostMessage(m_WindowList, TVM_DELETEITEM, 0, (LPARAM)position->second);

		// Reuse the icon
		if (iconIndex > LAST_ICON_INDEX) {
			CacheIconSlot(iconIndex);
		}

		// Remove it from the window/item map
		m_WindowItemMap.erase(position);
	}
	LeaveCriticalSection(&m_CS_WindowItemMap);

	if (deleteRelatedItems) {
		// Remove any tray icon and item associated with this indow
		DeleteTrayyedItem(TempWindow);
		// Remove any item in the topped branch for this window
		DeleteToppedItem(TempWindow);
	}
}

void HackItDlg::OnPosChangedDetected(WPARAM wParam, LPARAM lParam)
{
	WINDOWPOS *lpwp = (WINDOWPOS*)wParam;
	HWND hWnd = (HWND)lParam;
	HWND_HTREEITEM_map::iterator position;

	EnterCriticalSection(&m_CS_WindowItemMap);
	position = m_WindowItemMap.find(hWnd);
	HTREEITEM parent = TreeView_GetParent(m_WindowList, position->second);
	LeaveCriticalSection(&m_CS_WindowItemMap);

	if (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) {
		// not a top-level window item, ignore
		return;
	} else {
		EnterCriticalSection(&m_CS_WindowItemMap);
		if (m_WindowItemMap.find(hWnd) == m_WindowItemMap.end()) {
			// we didn't have it in the list before, ignore it
			LeaveCriticalSection(&m_CS_WindowItemMap);
			return;
		}
		LeaveCriticalSection(&m_CS_WindowItemMap);

		::SendMessageTimeout(hWnd, WM_NULL, NULL, NULL, SMTO_ABORTIFHUNG, 500, NULL); // Wait 200ms for it to become responsive
		// check for change in visibility
		if (::IsWindowVisible(hWnd)) {
			if (parent == m_HiddenItem) {
				ReAddWindow(hWnd);
			}
		} else {
			if (parent == m_Desktop) {
				ReAddWindow(hWnd);
			}
		}

		// check for change in topmost status
		if (m_ToppedItemMap.find(hWnd) == m_ToppedItemMap.end()) {
			if (::GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) {
				AddToppedItem(hWnd);
			}
		} else {
			if (!(::GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
				DeleteToppedItem(hWnd);
			}
		}
	}
}

void HackItDlg::ReAddWindow(HWND hWnd)
{
	DeleteBasicItem(hWnd, FALSE);
	AddBasicItem(hWnd, FALSE);
	EnumChildWindows(hWnd, EnumChildWindowsProc, (LPARAM)m_hWndManager);
}

void HackItDlg::OnCreateDetected(WPARAM wParam, LPARAM lParam)
{
	HWND window = (HWND)lParam;
	char windowClass[256];


	GetClassName(window, windowClass, 256);
	if (strcmp(windowClass, "ConsoleWindowClass")) { // some programs in console windows never respond to WM_NULL
		SendMessageTimeout(window, WM_NULL, NULL, NULL, SMTO_ABORTIFHUNG, 500, NULL); // wait 500msfor window to be responsive to messages
	}
	AddWindow(window);
}

void HackItDlg::OnDestroyDetected(WPARAM wParam, LPARAM lParam)
{
	// why is ths here? I don't send messages to or query from the window that has been destroyed...
	//SendMessageTimeout((HWND)lParam, WM_NULL, NULL, NULL, SMTO_ABORTIFHUNG, 500, NULL); // wait 500ms for window to be responsive to messages
	DeleteWindow((HWND)lParam);
}

void HackItDlg::OnSetTextDetected(WPARAM wParam, LPARAM lParam)
{
	SendMessageTimeout((HWND)lParam, WM_NULL, NULL, NULL, SMTO_ABORTIFHUNG, 500, NULL); // wait 200ms for window to be responsive to messages
	UpdateItem((HWND)lParam);
}

void HackItDlg::OnSysMenuDetected(WPARAM wParam, LPARAM lParam)
{
	if (!m_Settings.m_SysMenuCapture) return;
	if (m_RedirectSysMenu) return;

	m_RedirectWindow = (HWND)lParam;

	// To avoid explorer wackiness with system menus
	while (GetWindowLong(m_RedirectWindow, GWL_STYLE) & WS_CHILD) {
		m_RedirectWindow = ::GetParent(m_RedirectWindow);
	}

	m_RedirectSysMenu = (HMENU)wParam;

	if (m_Settings.m_SysMenuAppend) {
		HRESULT hr = ::AppendMenu(m_RedirectSysMenu, MF_SEPARATOR, NULL, NULL);

		if (m_Settings.m_SysMenuVisible) {
			if (::IsWindowVisible(m_RedirectWindow)) {
				::AppendMenu(m_RedirectSysMenu, MF_STRING | MF_CHECKED, IDM_VISIBLEHIDDEN, "Visible");
			} else {
				::AppendMenu(m_RedirectSysMenu, MF_STRING, IDM_VISIBLEHIDDEN, "Visible");
			}
		}

		if (m_Settings.m_SysMenuTopped) {
			if (::GetWindowLong(m_RedirectWindow, GWL_EXSTYLE) & WS_EX_TOPMOST) {
				::AppendMenu(m_RedirectSysMenu, MF_STRING | MF_CHECKED, IDM_TOPPEDUNTOPPED, "Topmost");
			} else {
				::AppendMenu(m_RedirectSysMenu, MF_STRING, IDM_TOPPEDUNTOPPED, "Topmost");
			}
		}

		if (m_Settings.m_SysMenuTrayyed) {
			if (m_TrayyedItemMap.find(m_RedirectWindow) == m_TrayyedItemMap.end()) {
				::AppendMenu(m_RedirectSysMenu, MF_STRING, IDM_TRAYYEDUNTRAYYED, "Tray Icon");
			} else {
				::AppendMenu(m_RedirectSysMenu, MF_STRING | MF_CHECKED, IDM_TRAYYEDUNTRAYYED, "Tray Icon");
			}
		}

		if (m_Settings.m_SysMenuPriority) {
			DWORD process;
			HANDLE tempProcess;
			::GetWindowThreadProcessId(m_RedirectWindow, &process);
			tempProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process);

			switch (::GetPriorityClass(tempProcess)) {
			case REALTIME_PRIORITY_CLASS:
				::CheckMenuRadioItem(m_SysMenuPriorityMenu, 0, 3, 0, MF_BYPOSITION);
				break;
			case HIGH_PRIORITY_CLASS:
				::CheckMenuRadioItem(m_SysMenuPriorityMenu, 0, 3, 1, MF_BYPOSITION);
				break;
			case NORMAL_PRIORITY_CLASS:
				::CheckMenuRadioItem(m_SysMenuPriorityMenu, 0, 3, 2, MF_BYPOSITION);
				break;
			case IDLE_PRIORITY_CLASS:
				::CheckMenuRadioItem(m_SysMenuPriorityMenu, 0, 3, 3, MF_BYPOSITION);
				break;
			}
			CloseHandle(tempProcess);

			::AppendMenu(m_RedirectSysMenu, MF_POPUP, (UINT)m_SysMenuPriorityMenu, "Priority");
		}

		if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes != NULL) {
			::AppendMenu(m_RedirectSysMenu, MF_POPUP, (UINT)m_SysMenuTransparencyMenu, "Transparency");
		}
	} else {
		HRESULT hr = ::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);

		if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes != NULL) {
			::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT)m_SysMenuTransparencyMenu, "Transparency");
		}

		if (m_Settings.m_SysMenuPriority) {
			DWORD process;
			HANDLE tempProcess;
			::GetWindowThreadProcessId(m_RedirectWindow, &process);
			tempProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process);

			switch (::GetPriorityClass(tempProcess)) {
			case REALTIME_PRIORITY_CLASS:
				::CheckMenuRadioItem(m_SysMenuPriorityMenu, 0, 3, 0, MF_BYPOSITION);
				break;
			case HIGH_PRIORITY_CLASS:
				::CheckMenuRadioItem(m_SysMenuPriorityMenu, 0, 3, 1, MF_BYPOSITION);
				break;
			case NORMAL_PRIORITY_CLASS:
				::CheckMenuRadioItem(m_SysMenuPriorityMenu, 0, 3, 2, MF_BYPOSITION);
				break;
			case IDLE_PRIORITY_CLASS:
				::CheckMenuRadioItem(m_SysMenuPriorityMenu, 0, 3, 3, MF_BYPOSITION);
				break;
			}
			CloseHandle(tempProcess);

			::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT)m_SysMenuPriorityMenu, "Priority");
		}

		if (m_Settings.m_SysMenuTrayyed) {
			if (m_TrayyedItemMap.find(m_RedirectWindow) == m_TrayyedItemMap.end()) {
				::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRAYYEDUNTRAYYED, "Tray Icon");
			} else {
				::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_STRING | MF_CHECKED, IDM_TRAYYEDUNTRAYYED, "Tray Icon");
			}
		}

		if (m_Settings.m_SysMenuTopped) {
			if (::GetWindowLong(m_RedirectWindow, GWL_EXSTYLE) & WS_EX_TOPMOST) {
				::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_STRING | MF_CHECKED, IDM_TOPPEDUNTOPPED, "Topmost");
			} else {
				::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TOPPEDUNTOPPED, "Topmost");
			}
		}

		if (m_Settings.m_SysMenuVisible) {
			if (::IsWindowVisible(m_RedirectWindow)) {
				::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_STRING | MF_CHECKED, IDM_VISIBLEHIDDEN, "Visible");
			} else {
				::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_STRING, IDM_VISIBLEHIDDEN, "Visible");
			}
		}
	}
	::DrawMenuBar(m_RedirectWindow);
}

void HackItDlg::OnHackItFunctionDetected(WPARAM wParam, LPARAM lParam)
{
	if (!m_Settings.m_SysMenuCapture) return;
	if (m_RedirectWindow == NULL) return;

	HWND TempWindow = m_RedirectWindow;

	switch (wParam & 0xFFF0) {
	case IDM_VISIBLEHIDDEN:
		if (::IsWindowVisible(TempWindow)) {
			::SetWindowPos(TempWindow, 0, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		} else {
			::SetWindowPos(TempWindow, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		}
		break;
	case IDM_TOPPEDUNTOPPED:
		if (::GetWindowLong(TempWindow, GWL_EXSTYLE) & WS_EX_TOPMOST) {
			WindowFunctions::UnTopWindow(TempWindow);
			DeleteToppedItem(TempWindow);
		} else {
			WindowFunctions::TopWindow(TempWindow);
			AddToppedItem(TempWindow);
		}
		break;
	case IDM_TRAYYEDUNTRAYYED:
		if (m_TrayyedItemMap.find(TempWindow) == m_TrayyedItemMap.end()) {
			AddTrayyedItem(TempWindow);
		} else {
			DeleteTrayyedItem(TempWindow);
		}
		break;
	case IDM_HIGHESTPRIORITY:
		ProcessFunctions::SetWindowPriority(TempWindow, REALTIME_PRIORITY_CLASS);
		//ResetMenuChecks();
		//UpdateMenuChecks(TempWindow);
		break;
	case IDM_HIGHPRIORITY:
		ProcessFunctions::SetWindowPriority(TempWindow, HIGH_PRIORITY_CLASS);
		//ResetMenuChecks();
		//UpdateMenuChecks(TempWindow);
		break;
	case IDM_NORMALPRIORITY:
		ProcessFunctions::SetWindowPriority(TempWindow, NORMAL_PRIORITY_CLASS);
		//ResetMenuChecks();
		//UpdateMenuChecks(TempWindow);
		break;
	case IDM_LOWPRIORITY:
		ProcessFunctions::SetWindowPriority(TempWindow, IDLE_PRIORITY_CLASS);
		//ResetMenuChecks();
		//UpdateMenuChecks(TempWindow);
		break;
	case IDM_TRANSPARENCY_NONE:
		WindowFunctions::SetTransparency(TempWindow, 0);
		break;
	case IDM_TRANSPARENCY_10:
		WindowFunctions::SetTransparency(TempWindow, 10);
		break;
	case IDM_TRANSPARENCY_20:
		WindowFunctions::SetTransparency(TempWindow, 20);
		break;
	case IDM_TRANSPARENCY_30:
		WindowFunctions::SetTransparency(TempWindow, 30);
		break;
	case IDM_TRANSPARENCY_40:
		WindowFunctions::SetTransparency(TempWindow, 40);
		break;
	case IDM_TRANSPARENCY_50:
		WindowFunctions::SetTransparency(TempWindow, 50);
		break;
	case IDM_TRANSPARENCY_60:
		WindowFunctions::SetTransparency(TempWindow, 60);
		break;
	case IDM_TRANSPARENCY_70:
		WindowFunctions::SetTransparency(TempWindow, 70);
		break;
	case IDM_TRANSPARENCY_80:
		WindowFunctions::SetTransparency(TempWindow, 80);
		break;
	case IDM_TRANSPARENCY_90:
		WindowFunctions::SetTransparency(TempWindow, 90);
		break;
	case IDM_TRANSPARENCY_100:
		WindowFunctions::SetTransparency(TempWindow, 100);
		break;
	}

	m_RedirectWindow = NULL;

	if (!m_Settings.m_SysMenuCapture) return;
	if (m_RedirectSysMenu == NULL) return;
	
	if (m_Settings.m_SysMenuVisible) {
		RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
	}
	if (m_Settings.m_SysMenuTopped) {
		RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
	}
	if (m_Settings.m_SysMenuTrayyed) {
		RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
	}
	if (m_Settings.m_SysMenuPriority) {
		RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
	}
	if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes != NULL) {
		RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
	}
	// remove seperator
	RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);

	m_RedirectSysMenu = NULL;
}

void HackItDlg::OnSysMenuEnded(WPARAM wParam, LPARAM lParam)
{
	if (!m_Settings.m_SysMenuCapture) return;
	if (m_RedirectSysMenu == NULL) return;
	
	if (m_Settings.m_SysMenuAppend) {
		int menu_item_count = GetMenuItemCount(m_RedirectSysMenu) - 1;

		if (m_Settings.m_SysMenuVisible) {
			RemoveMenu(m_RedirectSysMenu, menu_item_count--, MF_BYPOSITION);
		}
		if (m_Settings.m_SysMenuTopped) {
			RemoveMenu(m_RedirectSysMenu, menu_item_count--, MF_BYPOSITION);
		}
		if (m_Settings.m_SysMenuTrayyed) {
			RemoveMenu(m_RedirectSysMenu, menu_item_count--, MF_BYPOSITION);
		}
		if (m_Settings.m_SysMenuPriority) {
			RemoveMenu(m_RedirectSysMenu, menu_item_count--, MF_BYPOSITION);
		}
		if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes != NULL) {
			RemoveMenu(m_RedirectSysMenu, menu_item_count--, MF_BYPOSITION);
		}
		// remove seperator
		RemoveMenu(m_RedirectSysMenu, menu_item_count--, MF_BYPOSITION);
	} else {
		if (m_Settings.m_SysMenuVisible) {
			RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
		}
		if (m_Settings.m_SysMenuTopped) {
			RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
		}
		if (m_Settings.m_SysMenuTrayyed) {
			RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
		}
		if (m_Settings.m_SysMenuPriority) {
			RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
		}
		if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes != NULL) {
			RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
		}
		// remove seperator
		RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
	}

	m_RedirectSysMenu = NULL;
}

HTREEITEM HackItDlg::AddTrayyedItem(HWND window)
{	// TO DO: check for already added
	TV_INSERTSTRUCT TempItem;
	char TempText1[ STRING_BUFFER_SIZE ];
	HTREEITEM TempHItem = 0;
	HTREEITEM item = 0;
	
	// Don't add if it's not in the window map
	EnterCriticalSection(&m_CS_WindowItemMap);
	if (m_WindowItemMap.count(window) != 1) {
		LeaveCriticalSection(&m_CS_WindowItemMap);
		return NULL;
	}
	item = m_WindowItemMap.find(window)->second;
	LeaveCriticalSection(&m_CS_WindowItemMap);

	// Don't add if it's already in the topped map
	EnterCriticalSection(&m_CS_TrayyedItemMap);
	if (m_ToppedItemMap.count(window) == 1) {
		LeaveCriticalSection(&m_CS_TrayyedItemMap);
		return NULL;
	}
	LeaveCriticalSection(&m_CS_TrayyedItemMap);

	// Perform the add
	SetIconData(m_hWnd, (UINT)window);
	if (AddIcon()) {
		TempItem.hInsertAfter = TVI_SORT;
		TempItem.item.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
		
		::GetWindowText(window, TempText1, 255);
		if (strlen(TempText1) == 0)
			strcpy(TempText1, m_Settings.m_NamelessText.c_str());
		TempItem.item.pszText = TempText1;
		TempItem.item.iImage = TempItem.item.iSelectedImage = GetWindowItemIconIndex(item);
		TempItem.item.lParam = long(window);
		TempItem.hParent = m_TrayyedItem;
		TempHItem = TreeView_InsertItem(m_WindowList, (LONG)&TempItem);

		EnterCriticalSection(&m_CS_TrayyedItemMap);
		m_TrayyedItemMap[window] = TempHItem;
		LeaveCriticalSection(&m_CS_TrayyedItemMap);

		RECT itemRect;

		if (TreeView_GetItemRect(m_WindowList, TreeView_GetParent(m_WindowList, TempHItem), &itemRect, FALSE)) {
			RedrawWindow(m_WindowList, &itemRect, NULL, RDW_INVALIDATE);
		}

		::SetWindowText(m_StatusBar, "Icon successfully added to the tray!");
	} else {
		::SetWindowText(m_StatusBar, "That program already has an icon on the tray.");
	}


	return TempHItem;
}

HTREEITEM HackItDlg::AddToppedItem(HWND window)
{	// TO DO: check for already added.
	// Only add children to topmost list if specified
	HTREEITEM item = 0;

	// Don't add if it's a child and we're not supposed to add children
	if ((GetWindowLong(window, GWL_STYLE) & WS_CHILD) && !m_Settings.m_ChildrenInTopmostList)
		return NULL;

	// Don't add if it's not in the window map
	EnterCriticalSection(&m_CS_WindowItemMap);
	if (m_WindowItemMap.count(window) != 1) {
		LeaveCriticalSection(&m_CS_WindowItemMap);
		return NULL;
	}
	item = m_WindowItemMap.find(window)->second;
	LeaveCriticalSection(&m_CS_WindowItemMap);

	// Don't add if it's already in the topped map
	EnterCriticalSection(&m_CS_ToppedItemMap);
	if (m_ToppedItemMap.count(window) == 1) {
		LeaveCriticalSection(&m_CS_ToppedItemMap);
		return NULL;
	}
	LeaveCriticalSection(&m_CS_ToppedItemMap);

	// Perform the add
	TV_INSERTSTRUCT TempItem;
	char TempText1[ STRING_BUFFER_SIZE ];
	HTREEITEM TempHItem = 0;
	
	TempItem.hInsertAfter = TVI_SORT;
	TempItem.item.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
	
	::GetWindowText(window, TempText1, 255);
	if (strlen(TempText1) == 0)
		strcpy(TempText1, m_Settings.m_NamelessText.c_str());
	TempItem.item.pszText = TempText1;
	TempItem.item.iImage = TempItem.item.iSelectedImage = GetWindowItemIconIndex(item);
	TempItem.item.lParam = long(window);
	TempItem.hParent = m_ToppedItem;
	TempHItem = TreeView_InsertItem(m_WindowList, (LONG)&TempItem);

	EnterCriticalSection(&m_CS_ToppedItemMap);
	m_ToppedItemMap[window] = TempHItem;
	LeaveCriticalSection(&m_CS_ToppedItemMap);

	RECT itemRect;

	if (TreeView_GetItemRect(m_WindowList, TreeView_GetParent(m_WindowList, TempHItem), &itemRect, FALSE)) {
		RedrawWindow(m_WindowList, &itemRect, NULL, RDW_INVALIDATE);
	}

	return TempHItem;
}

void HackItDlg::DeleteToppedItem(HWND window)
{
	HWND_HTREEITEM_map::iterator position;
	
	EnterCriticalSection(&m_CS_ToppedItemMap);
	if ((position = m_ToppedItemMap.find(window)) != m_ToppedItemMap.end() && position->second != NULL) {
		PostMessage(m_WindowList, TVM_DELETEITEM, 0, (LPARAM)position->second);
		m_ToppedItemMap.erase(position);
	}
	LeaveCriticalSection(&m_CS_ToppedItemMap);
}

void HackItDlg::DeleteTrayyedItem(HWND window)
{
	HWND_HTREEITEM_map::iterator position;
	NOTIFYICONDATA TempData;

	TempData.cbSize = sizeof(NOTIFYICONDATA);
	TempData.hWnd = m_hWnd;
	TempData.uID = (UINT)window;
	TempData.uFlags = 0;

	EnterCriticalSection(&m_CS_TrayyedItemMap);
	if (Shell_NotifyIcon(NIM_DELETE, &TempData)) {
		if (window != m_hWnd) {
			::SetWindowPos(window, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		} else {
			::SetWindowText(m_StatusBar, "You cannot untray the Hack-It window!");
			LeaveCriticalSection(&m_CS_TrayyedItemMap);
			return;
		}
		if ((position = m_TrayyedItemMap.find(window)) != m_TrayyedItemMap.end() && position->second != NULL) {
			PostMessage(m_WindowList, TVM_DELETEITEM, 0, (LPARAM)position->second);
			m_TrayyedItemMap.erase(position);
		}
	}
	LeaveCriticalSection(&m_CS_TrayyedItemMap);
}

long HackItDlg::OnCheckTrayyedList(WPARAM wParam, LPARAM lParam) {
	EnterCriticalSection(&m_CS_TrayyedItemMap);
	long result = (m_TrayyedItemMap.count((HWND)wParam) != 0);
	LeaveCriticalSection(&m_CS_TrayyedItemMap);
	return result;
}

HTREEITEM HackItDlg::AddBasicItem(HWND Window, BOOL addRelatedItems)
{
	TV_INSERTSTRUCT TempItem;
	char TempText1[STRING_BUFFER_SIZE];
	HTREEITEM TempHItem;
 	HWND hWndParent;
	HWND_HTREEITEM_map::iterator position;
	
	if ((position = m_WindowItemMap.find(Window)) == m_WindowItemMap.end()) {
		if (WindowFilter(Window))
		{
			TempItem.hInsertAfter = TVI_SORT;
			TempItem.item.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
			
			if (::GetWindowTextLength(Window) > 0) {
				::GetWindowText(Window, TempText1, STRING_BUFFER_SIZE);
			} else {
				strcpy(TempText1, m_Settings.m_NamelessText.c_str());
			}

			TempItem.item.pszText = TempText1;

			// Get the icon for this window
			unsigned long TempIcon = NULL;

			TempIcon = (DWORD)GetWindowIcon(Window);
			// if we got a valid icon, add it
			if (TempIcon != NULL) {
				TempItem.item.iSelectedImage = AddOrReuseIcon((HICON)TempIcon);
			}

			// if add failed or we didn't get an icon, use class icon or default
			if (TempIcon == NULL || TempItem.item.iSelectedImage == -1) {
				TempItem.item.iSelectedImage = GetClassIcon(Window);
			}
			TempItem.item.iImage = TempItem.item.iSelectedImage;
			
			// Decide where to put it
			TempItem.item.lParam = long(Window);
			if (::IsWindowVisible(Window)) {
				TempItem.hParent = m_Desktop;
			} else TempItem.hParent = m_HiddenItem;

			// locate parent item if it's a real child
			if (GetWindowLong(Window, GWL_STYLE) & WS_CHILD) {
				hWndParent = GetParent(Window);
				if ((position = m_WindowItemMap.find(hWndParent)) != m_WindowItemMap.end()) {
					TempItem.hParent = position->second;
				} else {
					TempItem.hParent = AddWindow(GetParent(Window));
				}
				if (!TempItem.hParent) {
					// Parent is illegal to add for whatever reason, don't add child
					return NULL;
				}
			}

			TempHItem = TreeView_InsertItem(m_WindowList, &TempItem);
			if (TempHItem == FALSE) {
				return NULL;
			}

			RECT itemRect;

			if (TreeView_GetItemRect(m_WindowList, TreeView_GetParent(m_WindowList, TempHItem), &itemRect, FALSE)) {
				RedrawWindow(m_WindowList, &itemRect, NULL, RDW_INVALIDATE);
			}

			EnterCriticalSection(&m_CS_WindowItemMap);
			m_WindowItemMap[Window] = TempHItem;
			LeaveCriticalSection(&m_CS_WindowItemMap);

			if (addRelatedItems) {
				if (::GetWindowLong(Window, GWL_EXSTYLE) & WS_EX_TOPMOST) {
					AddToppedItem(Window);
				}
			}

			if (m_Settings.m_AutoFunctionsEnabled && (!(GetWindowLong(Window, GWL_STYLE) & WS_CHILD) || m_Settings.m_ChildFunctionsEnabled)) {
				CheckAuto(Window);
			}
		} else {
			return NULL;
		}
	} else {
		TempHItem = position->second;
	}
	
	return TempHItem;
}

void HackItDlg::CleanList()
{
	HWND_HTREEITEM_map::iterator pos1, pos2, end;
	HWND_HTREEITEM_map::iterator position;

	set< HWND > windowsToDelete;
	set< HWND > windowsToReAdd;

	EnterCriticalSection(&m_CS_WindowItemMap);
	pos1 = m_WindowItemMap.begin();

	while (pos1 != m_WindowItemMap.end()) {
		if (!::IsWindow(pos1->first)) {
			//pos2 = pos1;
			windowsToDelete.insert(windowsToDelete.end(), pos1->first);
			pos1++;
			//DeleteWindow(pos2->first);
		} else {
			// only perform the following checks for top-level windows
			if (!(GetWindowLong(pos1->first, GWL_STYLE) & WS_CHILD)) {
				if (!::IsWindowVisible(pos1->first) && TreeView_GetParent(m_WindowList, pos1->second) == m_Desktop) {
					// window is not visible and is listed as such, move it
					//pos2 = pos1;
					windowsToReAdd.insert(windowsToReAdd.end(), pos1->first);
					pos1++;
					//ReAddWindow(pos2->first);
				} else if (::IsWindowVisible(pos1->first) && TreeView_GetParent(m_WindowList, pos1->second) == m_HiddenItem) {
					//pos2 = pos1;
					windowsToReAdd.insert(windowsToReAdd.end(), pos1->first);
					pos1++;
					//ReAddWindow(pos2->first);
				} else {
					pos1++;
				}
			} else {
				pos1++;
			}
		}
	}
	LeaveCriticalSection(&m_CS_WindowItemMap);

	// Execute discovered actions
	while (windowsToDelete.begin() != windowsToDelete.end()) {
		DeleteWindow(*windowsToDelete.begin());
		windowsToDelete.erase(windowsToDelete.begin());
	}

	while (windowsToReAdd.begin() != windowsToReAdd.end()) {
		ReAddWindow(*windowsToReAdd.begin());
		windowsToReAdd.erase(windowsToReAdd.begin());
	}
}

// Always called from within a WindowItemMap critical section, so no need critical section WindowItemMap code here
void HackItDlg::DeleteChildItems(HTREEITEM Parent, BOOL deleteRelatedItems)
{
	HTREEITEM item = NULL;
	HWND TempWindow;
	int nIcon;
	HWND_HTREEITEM_map::iterator position;
	HWND_HTREEITEM_map::iterator position2;

	// Get the first child item
	item = TreeView_GetChild(m_WindowList, Parent);

	// While there are children left to clean up...
	while (item != NULL) {
		// Delete the grandchildren
		DeleteChildItems(item, deleteRelatedItems);
		TempWindow = GetWindowItemHwnd(item);
		nIcon = GetWindowItemIconIndex(item);
		
		if (nIcon > LAST_ICON_INDEX) {
			CacheIconSlot(nIcon);
		}
	
		// Remove it from the window/item map
		if ((position = m_WindowItemMap.find(TempWindow)) != m_WindowItemMap.end() && position->second != NULL) {
			m_WindowItemMap.erase(position);
		}

		// Get the next child
		item = TreeView_GetNextSibling(m_WindowList, item);
	}

	// If specified, delete related items as well
	if (deleteRelatedItems) {
		DeleteTrayyedItem(TempWindow);
		DeleteToppedItem(TempWindow);
	}
}

bool HackItDlg::WindowFilter(HWND window)
{
	char winclass[STRING_BUFFER_SIZE], wintitle[STRING_BUFFER_SIZE];
	::GetClassName(window, winclass, STRING_BUFFER_SIZE);
	::GetWindowText(window, wintitle, STRING_BUFFER_SIZE);

	bool usable = true;
	IllegalWindowSpec_set::iterator current;

	// Check to make sure it's actually a window
	if (!(::IsWindow(window))) {
		return false;
	}

	// Check for title of '\0' or unprintable character and don't add it if specified
	// Only applies to top-level windows currently
	if (!(GetWindowLong(window, GWL_STYLE) & WS_CHILD)) {
		if (wintitle[0] < m_Settings.m_LowestShowable) {
			return false;
		}
	}

	// Don't show Hack-It window
	if (strcmp(winclass, "HackIt") == 0) {
		return false;
	}

	// Don't show Hack-It List Manager Thread window
	if (strcmp(winclass, "HackItListManager") == 0) {
		return false;
	}

	// Process the illegal window spec list
	if (m_Settings.m_IllegalWindowSpecList.empty()) {
		// It's empty, we're done.
		return usable;
	}

	IllegalWindowSpec* pIllWinSpec;
	current = m_Settings.m_IllegalWindowSpecList.begin();

	while (usable && (current != m_Settings.m_IllegalWindowSpecList.end())) {
		pIllWinSpec = *current;

		usable = !pIllWinSpec->match(window);

		if (!usable) {
			return usable;
		}

		current++;
	}
	
	return usable;
}

void HackItDlg::UpdateItem(HWND wnd)
{
	// Danger: assuming it it on the list.
	HWND window = wnd;
	char title[STRING_BUFFER_SIZE];
	char buffer[STRING_BUFFER_SIZE];
	TV_ITEM tempItem;
	HWND_HTREEITEM_map::iterator position, position2;
	HTREEITEM item;
	
	EnterCriticalSection(&m_CS_WindowItemMap);
	position = m_WindowItemMap.find(window);
	if (position == m_WindowItemMap.end()) {
		LeaveCriticalSection(&m_CS_WindowItemMap);
		// window has not been added, add it if necessary
		if (m_Settings.m_UpdateAddRemove && !(GetWindowLong(wnd, GWL_STYLE) & WS_CHILD)) {
			AddWindow(wnd);
			EnumChildWindows(wnd, EnumChildWindowsProc, (LPARAM)m_hWnd);
		}
		return;
	} else {
		item = position->second;
		LeaveCriticalSection(&m_CS_WindowItemMap);
	}
	
	tempItem.hItem = item;
	tempItem.pszText = buffer;
	tempItem.mask = TVIF_TEXT;
	tempItem.cchTextMax = STRING_BUFFER_SIZE;
	
	TreeView_GetItem(m_WindowList, &tempItem);

	::GetWindowText((HWND)window, title, STRING_BUFFER_SIZE);
	if (strlen(title) == 0) {
		strcpy(title, m_Settings.m_NamelessText.c_str());
	}

	if (strcmp(tempItem.pszText, title) != 0 && m_Settings.m_UpdateItems) {
		SetWindowItemText(item, title);
		
		tempItem.hItem = item;
		tempItem.mask = TVIF_IMAGE;
		
		TreeView_GetItem(m_WindowList, &tempItem);

		EnterCriticalSection(&m_CS_TrayyedItemMap);
		if ((position2 = m_TrayyedItemMap.find(window)) != m_TrayyedItemMap.end()) {
			SetWindowItemText(position2->second, title);
			
			SetIconData(m_hWnd, (UINT)window);
			UpdateIcon();
		}
		LeaveCriticalSection(&m_CS_TrayyedItemMap);
		
		EnterCriticalSection(&m_CS_ToppedItemMap);
		if ((position2 = m_ToppedItemMap.find(window)) != m_ToppedItemMap.end()) {
			SetWindowItemText(position2->second, title);
		}
		LeaveCriticalSection(&m_CS_ToppedItemMap);
		
		NOTIFYICONDATA TempData;
		TempData.cbSize = sizeof(NOTIFYICONDATA);
		TempData.hWnd = m_hWnd;
		TempData.uID = (UINT)window;
		strncpy(TempData.szTip, title, 64);
		TempData.uFlags = NIF_TIP;
		Shell_NotifyIcon(NIM_MODIFY, &TempData);
	
		if (m_Settings.m_UpdateAuto) {
			CheckAuto(wnd);
		}
	}
}

void HackItDlg::ButtonPress(HWND Button)
{
	::SendMessageTimeout(Button, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(1,1), SMTO_ABORTIFHUNG, 500, NULL);
	::PostMessage(Button, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(1,1));
}

BOOL CALLBACK HackItDlg::ButtonPressEnum(HWND window, LPARAM param) {
	char text[1024];

	GetWindowText(window, text, 1024);
	if (!strcmp((LPCSTR)param, text)) {
		::SendMessageTimeout(window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(1,1), SMTO_ABORTIFHUNG, 500, NULL);
		::PostMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(1,1));
		return FALSE;
	} else {
		return TRUE;
	}
}

int HackItDlg::AddOrReuseIcon(HICON icon)
{
	int iconIndex;
	int_set::iterator position;

	char logentry[256];
	
	// if there are slots not being used anymore, reuse one of them
	EnterCriticalSection(&m_CS_ReusableIconList);
	if ((position = m_ReusableIconList.begin()) != m_ReusableIconList.end()) {
		sprintf(logentry, "Total reusable slots: %d", m_ReusableIconList.size());
		LoggingSystem::log(logentry);
		sprintf(logentry, "Image list size: %d", ImageList_GetImageCount(m_ImageList));
		LoggingSystem::log(logentry);
		iconIndex = *position;
		sprintf(logentry, "Using slot #%d", iconIndex);
		LoggingSystem::log(logentry);
		ImageList_ReplaceIcon(m_ImageList, iconIndex, icon);
		m_ReusableIconList.erase(position);
	} else {
		// otherwise add a new slot
		iconIndex = ImageList_AddIcon(m_ImageList, icon);
	}
	LeaveCriticalSection(&m_CS_ReusableIconList);

	return iconIndex;
}

extern int (__stdcall *MyEnumProcessModules)(HANDLE, HMODULE*, DWORD, LPDWORD);
extern unsigned long (__stdcall *MyGetModuleFileNameEx)(HANDLE,HMODULE,char *,unsigned long);
extern unsigned long (__stdcall *MyGetWindowModuleFileName)(HWND,char *,unsigned long);

void GetWindowFileName(HWND window, char* file, int size) {
	if (MyEnumProcessModules != NULL && MyGetModuleFileNameEx != NULL) {
		HMODULE module;
		DWORD cbNeeded;
		HINSTANCE process;

		GetWindowThreadProcessId(window, (LPDWORD)&process);
		process = (HINSTANCE)OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, (DWORD)process);
		MyEnumProcessModules(process, &module, sizeof(HMODULE), &cbNeeded);
		MyGetModuleFileNameEx(process, module, file, STRING_BUFFER_SIZE);
		CloseHandle(process);
	} else if (MyGetWindowModuleFileName != NULL) {
		MyGetWindowModuleFileName(window, file, size);
	} else {
		HINSTANCE process;

		GetWindowThreadProcessId(window, (LPDWORD)&process);
		process = (HINSTANCE)OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, (DWORD)process);

		GetModuleFileName(process, file, STRING_BUFFER_SIZE);
		CloseHandle(process);
	}
}

void HackItDlg::CacheIconSlot(int index)
{
	EnterCriticalSection(&m_CS_ReusableIconList);
	// check if this is the last image in the list
	//if (index == (ImageList_GetImageCount(m_ImageList) - 1)) {
		// if so, just remove it
	//	ImageList_Remove(m_ImageList, index);
	//} else {
		// else, add it as a reusable slot
		m_ReusableIconList.insert(m_ReusableIconList.end(), index);
	//}
	LeaveCriticalSection(&m_CS_ReusableIconList);
}
