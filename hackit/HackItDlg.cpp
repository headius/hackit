// HackItDlg.cpp : implementation file
//
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

#define CONSOLE_WINDOW_POLL_TIMER 1000
#define WINDOW_LIST_CLEANER_TIMER 1001

using namespace std;

HWND g_hWndDlg;
HWND g_hWndManager;

/////////////////////////////////////////////////////////////////////////////
// AboutDlg dialog used for App About
class AboutDlg
{
	HWND m_hWndParent;
	HWND m_hWnd;
	HINSTANCE m_hInstance;

public:
	void OnOK();
	void OnInitDialog();
	int DoModal();
	static BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	AboutDlg(HWND hWndParent, HINSTANCE hInstance);

	
protected:
};

AboutDlg::AboutDlg(HWND hWndParent, HINSTANCE hInstance) {
	m_hWndParent = hWndParent;
	m_hInstance = hInstance;
}

BOOL CALLBACK AboutDlg::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AboutDlg* theDlg = (AboutDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (AboutDlg*)lParam;
		theDlg->m_hWnd = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
		return TRUE;
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case BN_CLICKED:
			switch (LOWORD(wParam)) {
			case IDOK:
				theDlg->OnOK();
				return TRUE;
				break;
			}
		}
	}

	return FALSE;
}

int AboutDlg::DoModal()
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWndParent, DialogProc,	(LPARAM)this);
}

void AboutDlg::OnOK()
{
	EndDialog(m_hWnd, IDOK);
}

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

HackItDlg::HackItDlg(HINSTANCE hInstance)
{
	m_hIconSM = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAMESM));
	m_hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	m_hInstance = hInstance;
	m_WindowList = NULL;
	m_StatusBar = NULL;
	m_hWndManager = NULL;

	m_RedirectWindow = NULL;
	m_HasListChanged = FALSE;
	m_RedirectWindow = 0;
	m_RedirectSysMenu = 0;

	// Set up critical sections
	InitializeCriticalSection(&m_CS_TrayyedItemMap);
	InitializeCriticalSection(&m_CS_ToppedItemMap);
	InitializeCriticalSection(&m_CS_WindowItemMap);
	InitializeCriticalSection(&m_CS_ReusableIconList);

	m_Settings.LoadSettings();

	if (m_Settings.m_ShowSplash) {
		SplashDlg *theSplash = new SplashDlg(NULL, hInstance);
		theSplash->DoModal();
		delete theSplash;
	}
	
	WNDCLASSEX wndClass;
	
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = m_hIcon;
	wndClass.hIconSm = m_hIconSM;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
	wndClass.lpszClassName = "HackIt";
	
	ATOM result;
	
	// register Hack-It main window class
	result = RegisterClassEx(&wndClass);
	
	if(result == NULL) {
		char astring[STRING_BUFFER_SIZE];
		DWORD error = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, NULL, astring, STRING_BUFFER_SIZE, NULL);
		MessageBox(NULL, astring, astring, MB_OK);
	}

	//m_SecretWindow = CreateWindow("STATIC", "", 0, 0, 0, 0, 0, 0, 0, hInstance, 0);

	m_hWnd = CreateWindowEx(
		WS_EX_CONTEXTHELP | WS_EX_WINDOWEDGE | (m_Settings.m_AlwaysOnTop ? WS_EX_TOPMOST : 0),
		"HackIt",
		"Hack-It v4.0 RC3",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | m_Settings.m_PlacementShow,
		m_Settings.m_PlacementLeft,
		m_Settings.m_PlacementTop,
		m_Settings.m_PlacementRight - m_Settings.m_PlacementLeft,
		m_Settings.m_PlacementBottom - m_Settings.m_PlacementTop,
		NULL,
		NULL,
		hInstance,
		this);

	if(m_hWnd == NULL) {
		char astring[STRING_BUFFER_SIZE];
		DWORD error = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, NULL, astring, STRING_BUFFER_SIZE, NULL);
		MessageBox(NULL, astring, astring, MB_OK);
	} else {
		if (m_Settings.m_Transparent) {
			WindowFunctions::SetTransparency(m_hWnd, m_Settings.m_TransparentPercent);
		} else {
			WindowFunctions::SetTransparency(m_hWnd, 0);
		}

		g_hWndDlg = m_hWnd;
	}
}

LRESULT CALLBACK HackItDlg::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
		case WM_ENDSESSION:
			returnCode = theDlg->OnEndSession(wParam, lParam);
			break;
		case WM_COMMAND:
			returnCode = theDlg->OnCommand(wParam, lParam);
			break;
		case WM_SYSCOMMAND:
			returnCode = theDlg->OnSysCommand(wParam, lParam);
			break;
		case WM_DESTROY:
			if (hWnd == theDlg->m_hWnd) {
				theDlg->OnDestroy();
			} else if (hWnd == theDlg->m_hWndManager) {
				theDlg->OnListThreadDestroy();
			}
			break;
		case WM_MENUSELECT:
			returnCode = theDlg->OnMenuSelect(LOWORD(wParam), HIWORD(wParam), (HMENU)lParam);
			break;
		case WM_SIZE:
			returnCode = theDlg->OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_NOTIFY:
			returnCode = theDlg->OnNotify(wParam, lParam);
			break;
		case WM_CONTEXTMENU:
			returnCode = theDlg->OnContextMenuTree1((HWND)wParam, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_CLOSE:
			returnCode = theDlg->OnClose();
			break;
		case WM_TRAYMESSAGE1:
			returnCode = theDlg->OnTrayNotification(wParam, lParam);
			break;
		case WM_TRAYMESSAGE2:
			returnCode = theDlg->OnCheckTrayyedList(wParam, lParam);
			break;
		/*case WM_CREATEDETECTED:
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
			break;*/
		case WM_HELP:
			theDlg->OnContextHelp(wParam, lParam);
			break;
		/*case WM_TIMER:
			theDlg->OnTimer(wParam, lParam);
			break;*/
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
		
	if (returnCode == -1) {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	} else {
		return returnCode;
	}
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
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
		
	if (returnCode == -1) {
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	} else {
		return returnCode;
	}
}

LRESULT HackItDlg::OnTimer(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case CONSOLE_WINDOW_POLL_TIMER:
		EnumWindows(ConsolePollProc, (LPARAM)m_hWndManager);
		break;
	case WINDOW_LIST_CLEANER_TIMER:
		this->CleanList();
		break;
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// HackItDlg message handlers

LRESULT HackItDlg::OnCreate(WPARAM wParam, LPARAM lParam)
{
	LoggingSystem::log("Initializing Hack-It");

	RECT rect;
	GetClientRect(m_hWnd, &rect);

	m_WindowList = ::CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_TREEVIEW,
		"Window List",
		WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP,
		0, 0,
		rect.right, rect.bottom - 20,
		m_hWnd,
		m_WindowPopup,
		m_hInstance,
		NULL);

	if (m_WindowList == NULL) {
		QuickMessage("WindowList broke");
	}

	m_ListThreadHandle = CreateThread(NULL, NULL, ListThreadProc, (LPVOID)this, NULL, &m_ListThreadID);

	// wait for thread to initialize
	while (m_hWndManager == NULL) Sleep(100);
	g_hWndManager = m_hWndManager;

	// Get default font for non-pro users
	LOGFONT logfont;
	::GetObject((HFONT)::SendMessage(m_WindowList, WM_GETFONT, NULL, NULL), sizeof(logfont), &logfont);

	m_StatusBar = ::CreateStatusWindow(
		WS_CHILD | WS_VISIBLE,
		"",
		m_hWnd,
		IDC_STATUSBAR);

	if (m_StatusBar == NULL) {
		QuickMessage("Static not created.");
	}

	HFONT font;

	if (m_Settings.m_FontWasLoaded) {
		font = ::CreateFontIndirect(&m_Settings.m_LogFont);
		::SendMessage(m_WindowList, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));
	} else {
		m_Settings.m_LogFont = logfont;
	}

	// Add items to the Hack-It system menu
	HMENU pSysMenu = ::GetSystemMenu(m_hWnd, FALSE);
	char strCloseApp[STRING_BUFFER_SIZE];
	LoadString(m_hInstance, IDS_CLOSEAPP, strCloseApp, STRING_BUFFER_SIZE);
	if (strlen(strCloseApp))
	{
		::AppendMenu(pSysMenu, MF_SEPARATOR, NULL, NULL);
		::AppendMenu(pSysMenu, MF_STRING, IDM_CLOSEAPP, strCloseApp);
	}
	
	// Get/create the various menus
	m_MainMenu = ::GetMenu(m_hWnd);
	m_WindowPopup = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_POPUP));
	m_ItemMainMenu = CreatePopupMenu();
	m_ItemSystemMenu = CreatePopupMenu();
	m_TraySystemMenu = CreatePopupMenu();

	// Create the priority menu for custom system menus
	m_SysMenuPriorityMenu = ::CreateMenu();
	::InsertMenu(m_SysMenuPriorityMenu, 0, MF_BYPOSITION | MF_STRING | MF_CHECKED, IDM_LOWPRIORITY, "Low");
	::InsertMenu(m_SysMenuPriorityMenu, 0, MF_BYPOSITION | MF_STRING, IDM_NORMALPRIORITY, "Normal");
	::InsertMenu(m_SysMenuPriorityMenu, 0, MF_BYPOSITION | MF_STRING, IDM_HIGHPRIORITY, "High");
	::InsertMenu(m_SysMenuPriorityMenu, 0, MF_BYPOSITION | MF_STRING, IDM_HIGHESTPRIORITY, "Highest");

	m_SysMenuTransparencyMenu = ::CreateMenu();
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_100, "100%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_90, "90%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_80, "80%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_70, "70%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_60, "60%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_50, "50%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_40, "40%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_30, "30%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_20, "20%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_10, "10%");
	::InsertMenu(m_SysMenuTransparencyMenu, 0, MF_BYPOSITION | MF_STRING, IDM_TRANSPARENCY_NONE, "None");
	
	// initialize the default icons in the image list and set it into the tree view
	if (m_Settings.m_UseLargeIcons) {
		m_ImageList = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 128, 16);
	} else {
		m_ImageList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 128, 16);
	}
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_DESKTOP)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_HIDDEN)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_DEFAULT)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TRAY)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TOPPED)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_BUTTON)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_STATIC)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_EDIT)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_COMBO)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_SCROLL)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_LIST)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_LISTVIEW)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TREEVIEW)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TABCTRL)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TOOLTIP)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_STATUS)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_MDICLIENT)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_TOOLBAR)));
	ImageList_AddIcon(m_ImageList, ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_DIALOG)));
	TreeView_SetImageList(m_WindowList, m_ImageList, TVSIL_NORMAL);
	
	// Load the specialized menus
	m_hButtonMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_BUTTONMENU));
	//m_hScrollbarMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_SCROLLBARMENU));
	m_hProcessMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_PROCESSMENU));
	m_hWindowMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_WINDOWMENU));
	m_hTransparencyMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_TRANSPARENCY));

	LoggingSystem::log("Adding Hack-It icon to the tray");
	SetIconData(m_hWnd, UINT(m_hWnd));
	AddIcon();

	LoggingSystem::log("Populating the window tree with existing windows");
	CreateList();
	::RedrawWindow(m_WindowList, NULL, NULL, RDW_UPDATENOW);
	LoggingSystem::log("before");
	// initialize hooks
	::SetWindowText(m_StatusBar, "Populating the Window List for the first time...");
	::RedrawWindow(m_StatusBar, NULL, NULL, RDW_UPDATENOW);

	m_pHooks = new SystemHooks(m_hWndManager);

	::SetWindowText(m_StatusBar, "Starting up poll timers...");
	::RedrawWindow(m_StatusBar, NULL, NULL, RDW_UPDATENOW);

	if (m_Settings.m_ConsolePolling) {
		SetTimer(m_hWndManager, CONSOLE_WINDOW_POLL_TIMER, m_Settings.m_ConsolePollingTime, NULL);
	}

	if (m_Settings.m_WindowListCleaning) {
		SetTimer(m_hWndManager, WINDOW_LIST_CLEANER_TIMER, m_Settings.m_WindowListCleanerTime, NULL);
	}

	::SetWindowText(m_StatusBar, "");
	::RedrawWindow(m_StatusBar, NULL, NULL, RDW_UPDATENOW);

	LoggingSystem::log("after");

	LoggingSystem::log("Init finished");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT HackItDlg::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	if ((wParam & 0xFFF0) == IDM_ABOUTBOX) {
		AboutDialog();
	} else if ((wParam & 0xFFF0) == SC_CLOSE) {
		ShowWindow(m_hWnd, SW_HIDE);
	} else if ((wParam & 0xFFF0) == IDM_CLOSEAPP) {
		Shutdown();
		DestroyWindow(m_hWnd);
	}

	return -1;
}

LRESULT HackItDlg::OnEndSession(WPARAM wParam, LPARAM lParam)
{
	Shutdown();
	DestroyWindow(m_hWnd);

	return 0;
}

LRESULT HackItDlg::OnDestroy()
{
	m_HackItRunning = FALSE;
	PostQuitMessage(0);
	return 0;
}

LRESULT HackItDlg::OnListThreadDestroy()
{
	PostQuitMessage(0);
	return 0;
} 

// Clean
void HackItDlg::CreateList() {

	EnterCriticalSection(&m_CS_WindowItemMap);
	m_WindowItemMap.clear();
	LeaveCriticalSection(&m_CS_WindowItemMap);
	EnterCriticalSection(&m_CS_TrayyedItemMap);
	m_TrayyedItemMap.clear();
	LeaveCriticalSection(&m_CS_TrayyedItemMap);
	EnterCriticalSection(&m_CS_ToppedItemMap);
	m_ToppedItemMap.clear();
	LeaveCriticalSection(&m_CS_ToppedItemMap);
	
	if (TreeView_DeleteAllItems(m_WindowList) == FALSE) {
		QuickMessage("Could not delete all items!");
	}
	
	TV_INSERTSTRUCT InsertItem;
	
	InsertItem.hParent = TVI_ROOT;
	InsertItem.hInsertAfter = TVI_LAST;
	InsertItem.item.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
	
	InsertItem.item.pszText = "Visible Windows";
	InsertItem.item.lParam = -1;
	InsertItem.item.iImage = InsertItem.item.iSelectedImage = 0;
	
	m_Desktop = TreeView_InsertItem(m_WindowList, (LONG)&InsertItem);
	
	InsertItem.item.pszText = "Hidden Windows";
	InsertItem.item.iImage = InsertItem.item.iSelectedImage = 1;
	
	m_HiddenItem = TreeView_InsertItem(m_WindowList, (LONG)&InsertItem);
	
	InsertItem.item.pszText = "Topmost Windows";
	InsertItem.item.iImage = InsertItem.item.iSelectedImage = 4;
	
	m_ToppedItem = TreeView_InsertItem(m_WindowList, (LONG)&InsertItem);
	
	InsertItem.item.pszText = "Trayyed Windows";
	InsertItem.item.iImage = InsertItem.item.iSelectedImage = 3;
	
	m_TrayyedItem = TreeView_InsertItem(m_WindowList, (LONG)&InsertItem);
	
	::SetWindowText(m_StatusBar,"");
	::RedrawWindow(m_StatusBar, NULL, NULL, RDW_UPDATENOW);
}

// Clean
void HackItDlg::SetWindowItemText(HTREEITEM hItem, char* newText)
{
	TV_ITEM tempItem;
	char buffer[STRING_BUFFER_SIZE];
	
	tempItem.hItem = hItem;
	tempItem.pszText = buffer;

	if (tempItem.pszText == NULL) {
		return;
	}
	
	if (newText == NULL) {
		strcpy(tempItem.pszText, m_Settings.m_NamelessText.c_str());
	} else {
		strcpy(tempItem.pszText, newText);
	}
	
	tempItem.mask = TVIF_TEXT;
	
	TreeView_SetItem(m_WindowList, &tempItem);
}

// Clean
LRESULT HackItDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) 
{
	char MenuString[STRING_BUFFER_SIZE];
	HMENU pPopup = ::GetSubMenu(m_WindowPopup, 0);
	//ASSERT(pPopup != NULL);

	::SetWindowText(m_StatusBar,"");
	
	if ((nFlags & MF_POPUP) && (hSysMenu == pPopup)) {
		::GetMenuString(pPopup, nItemID, MenuString, STRING_BUFFER_SIZE, MF_BYPOSITION);
		if (strcmp(MenuString, "System Menu") == 0) {
			m_ItemMenu = 1;
		} else if (strcmp(MenuString, "Main Menu") == 0) {
			m_ItemMenu = 2;
		} else {
			m_ItemMenu = 0;
		}
	}

	switch (nItemID)
	{
	case ID_VISIBLEHIDDEN:
		::SetWindowText(m_StatusBar, "Toggle visibility for the selected window");
		m_ItemMenu = 0;
		break;
	case ID_TOPPEDUNTOPPED:
		::SetWindowText(m_StatusBar, "Toggle topmost status for the selected window");
		m_ItemMenu = 0;
		break;
	case ID_TRAYYEDUNTRAYYED:
		::SetWindowText(m_StatusBar, "Toggle the tray icon for the selected window");
		break;
	case ID_MAXIMIZE:
		::SetWindowText(m_StatusBar, "Maximize the selected window");
		break;
	case ID_MINIMIZE:
		::SetWindowText(m_StatusBar, "Minimize the selected window");
		break;
	case ID_RESTORE:
		::SetWindowText(m_StatusBar, "Restore the selected window");
		break;
	case ID_CLOSE:
		::SetWindowText(m_StatusBar, "Close the selected window");
		break;
	case ID_PROCESS_REALTIME:
		::SetWindowText(m_StatusBar, "Set the selected window's process to Real-Time priority");
		break;
	case ID_PROCESS_HIGH:
		::SetWindowText(m_StatusBar, "Set the selected window's process to High priority");
		break;
	case ID_PROCESS_NORMAL:
		::SetWindowText(m_StatusBar, "Set the selected window's process to Normal priority");
		break;
	case ID_PROCESS_IDLE:
		::SetWindowText(m_StatusBar, "Set the selected window's process to Idle priority");
		break;
	case ID_PROCESS_ENDTASK:
		::SetWindowText(m_StatusBar, "End the process associated with the selected window");
		break;
	case ID_WINDOW_DISABLED:
		::SetWindowText(m_StatusBar, "Disable or enable user interaction with the selected window");
		break;
	case ID_WINDOWINFO:
		::SetWindowText(m_StatusBar, "Display useful information about the selected window");
		break;
	case ID_SETTINGS:
		::SetWindowText(m_StatusBar, "Opens up the Hack-It Settings and allow you to configure Hack-It");
		break;
	case ID_HELP_TOPICS:
		::SetWindowText(m_StatusBar, "Opens up Help and displays the topics for Hack-It");
		break;
	case ID_ABOUT:
		::SetWindowText(m_StatusBar, "Displays the About box for Hack-It");
		break;
	/*case ID_HELP_INDEX:
		::SetWindowText(m_StatusBar, "Opens the Hack-It help file index");
		break;*/
	}
	::RedrawWindow(m_StatusBar, NULL, NULL, RDW_UPDATENOW);
	return 0;
}
	
BOOL HackItDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (m_ItemMenu == 1) {
		::PostMessage(m_ItemHwnd, WM_SYSCOMMAND, wParam, lParam);
	} else if (m_ItemMenu == 2) {
		::PostMessage(m_ItemHwnd, WM_COMMAND, wParam, lParam);
	} else {		
		switch (HIWORD(wParam))
		{
		case 0:
			HWND TempWindow = GetWindowItemHwnd(TreeView_GetSelection(m_WindowList));
			::SetWindowText(m_StatusBar, "");
			if (TempWindow != 0 && ::IsWindow(TempWindow)) {
				switch (LOWORD(wParam)) {
				case ID_PROCESS_REALTIME:
					ProcessFunctions::SetWindowPriority(TempWindow, REALTIME_PRIORITY_CLASS);
					ResetMenuChecks();
					UpdateMenuChecks(TempWindow);
					break;
				case ID_PROCESS_HIGH:
					ProcessFunctions::SetWindowPriority(TempWindow, HIGH_PRIORITY_CLASS);
					ResetMenuChecks();
					UpdateMenuChecks(TempWindow);
					break;
				case ID_PROCESS_NORMAL:
					ProcessFunctions::SetWindowPriority(TempWindow, NORMAL_PRIORITY_CLASS);
					ResetMenuChecks();
					UpdateMenuChecks(TempWindow);
					break;
				case ID_PROCESS_IDLE:
					ProcessFunctions::SetWindowPriority(TempWindow, IDLE_PRIORITY_CLASS);
					ResetMenuChecks();
					UpdateMenuChecks(TempWindow);
					break;
				case ID_VISIBLEHIDDEN:
					if (::IsWindowVisible(TempWindow)) {
						::SetWindowPos(TempWindow, 0, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
					} else {
						::SetWindowPos(TempWindow, 0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
					}
					break;
				case ID_TOPPEDUNTOPPED:
					if (::GetWindowLong(TempWindow, GWL_EXSTYLE) & WS_EX_TOPMOST) {
						WindowFunctions::UnTopWindow(TempWindow);
						PostMessage(m_hWndManager, HM_DELETETOPPEDITEM, 0, (LPARAM)TempWindow);
					} else {
						WindowFunctions::TopWindow(TempWindow);

						// Some windows don't allow themselves to be topped, e.g. Desktop, etc
						if (::GetWindowLong(TempWindow, GWL_EXSTYLE) & WS_EX_TOPMOST) {
							PostMessage(m_hWndManager, HM_ADDTOPPEDITEM, 0, (LPARAM)TempWindow);
						} else {
							SetWindowText(m_StatusBar, "That window can not be topped");
						}
					}	
					break;
				case ID_TRAYYEDUNTRAYYED:
					if (m_TrayyedItemMap.find(TempWindow) == m_TrayyedItemMap.end()) {
						PostMessage(m_hWndManager, HM_ADDTRAYYEDITEM, 0, (LPARAM)TempWindow);
					} else {
						PostMessage(m_hWndManager, HM_DELETETRAYYEDITEM, 0, (LPARAM)TempWindow);
					}
					break;
				case ID_MAXIMIZE:
					WindowFunctions::MaximizeWindow(TempWindow);
					break;
				case ID_MINIMIZE:
					WindowFunctions::MinimizeWindow(TempWindow);
					break;
				case ID_RESTORE:
					WindowFunctions::RestoreWindow(TempWindow);
					break;
				case ID_CLOSE:
					WindowFunctions::CloseWindow(TempWindow);
					break;
				case ID_PROCESS_ENDTASK:
					ProcessFunctions::EndTask(TempWindow);
					// Need to delete the window from our lists since we will not be signaled of its untimely demise
					PostMessage(m_hWndManager, WM_DESTROYDETECTED, 0, LPARAM(TempWindow));
					break;
				case ID_BUTTON_PRESS:
					ButtonPress(TempWindow);
					break;
				case ID_WINDOW_DISABLED:
					if (::GetWindowLong(TempWindow, GWL_STYLE) & WS_DISABLED) {
						WindowFunctions::WindowEnable(TempWindow);
					} else {
						WindowFunctions::WindowDisable(TempWindow);
					}
					break;
				case ID_WINDOW_SETFONT:
					WindowFunctions::WindowSetFont(TempWindow);
					break;
				case ID_TRANSPARENCY_NONE:
					WindowFunctions::SetTransparency(TempWindow, 0);
					break;
				case ID_TRANSPARENCY_10:
					WindowFunctions::SetTransparency(TempWindow, 10);
					break;
				case ID_TRANSPARENCY_20:
					WindowFunctions::SetTransparency(TempWindow, 20);
					break;
				case ID_TRANSPARENCY_30:
					WindowFunctions::SetTransparency(TempWindow, 30);
					break;
				case ID_TRANSPARENCY_40:
					WindowFunctions::SetTransparency(TempWindow, 40);
					break;
				case ID_TRANSPARENCY_50:
					WindowFunctions::SetTransparency(TempWindow, 50);
					break;
				case ID_TRANSPARENCY_60:
					WindowFunctions::SetTransparency(TempWindow, 60);
					break;
				case ID_TRANSPARENCY_70:
					WindowFunctions::SetTransparency(TempWindow, 70);
					break;
				case ID_TRANSPARENCY_80:
					WindowFunctions::SetTransparency(TempWindow, 80);
					break;
				case ID_TRANSPARENCY_90:
					WindowFunctions::SetTransparency(TempWindow, 90);
					break;
				case ID_TRANSPARENCY_100:
					WindowFunctions::SetTransparency(TempWindow, 100);
					break;
				/*case ID_WINDOW_CHANGETEXT:
					WindowFunctions::WindowChangeText(TempWindow);
					break;*/
				}
				ResetMenuChecks();
				UpdateMenuChecks(TempWindow);
			}
			
			switch (LOWORD(wParam)) {
			case ID_HELP_TOPICS:
				::HtmlHelp(NULL, "HackIt.chm::/index.html", HH_DISPLAY_TOPIC, 0);
				break;
			case ID_HELP_PLEASEREGISTER:
				::HtmlHelp(NULL, "HackIt.chm::/registration_information.html", HH_DISPLAY_TOPIC, 0);
				break;
			case ID_SETTINGS:
				Settings();
				break;
			case ID_OTHER_SAVENOW:
				m_Settings.SaveSettings();
				break;
			case ID_ABOUT:
				AboutDialog();
				break;
			case ID_WINDOWINFO:
				WindowInfo();
				break;
			default:
				break;
			}
		}
	
	}
	
	UpdateWindow(m_hWnd);
	::UpdateWindow(m_WindowList);
	return -1;
}

HWND HackItDlg::GetWindowItemHwnd(HTREEITEM hItem)
{
	TV_ITEM tempItem;
	BOOL success;
	
	tempItem.hItem = hItem;
	tempItem.mask = TVIF_PARAM;
	
	success = TreeView_GetItem(m_WindowList, &tempItem);
	
	if (success) {
		return HWND(tempItem.lParam);
	} else {
		return 0;
	}
}

long HackItDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	NOTIFYICONDATA TempData;
	
	if (!IsWindow(HWND(wParam)))
	{
		TempData.cbSize = sizeof(NOTIFYICONDATA);
		TempData.hWnd = m_hWnd;
		TempData.uID = wParam;
		TempData.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, &TempData);
	}
	else
	{
		switch (lParam)
		{
		case (WM_LBUTTONUP):
			if (::IsWindowVisible(HWND(wParam)))
			{
				if (HWND(wParam) == m_hWnd) {
					::SetForegroundWindow((HWND)wParam);
				} else {
					::ShowWindow(HWND(wParam), SW_HIDE);
				}
			}
			else
			{
				::ShowWindow(HWND(wParam), SW_SHOW);
				::SetForegroundWindow(HWND(wParam));
			}
			break;
		case (WM_RBUTTONUP):
			HMENU tempMenu;
			HWND window = (HWND)wParam;
			POINT pt;
			UINT returnVal;
			RECT rect;
			rect.top = 0;
			rect.bottom = 0;
			rect.left = 0;
			rect.right = 0;

			int oldcount = ::GetMenuItemCount(m_TraySystemMenu);
			for (int i = 0; i < oldcount; i++) {
				::RemoveMenu(m_TraySystemMenu, 0, MF_BYPOSITION);
			}
			
			if ((tempMenu = ::GetSystemMenu(window, FALSE)) != NULL) {
				int count = ::GetMenuItemCount(tempMenu);
				MENUITEMINFO info;
				char tempStr[STRING_BUFFER_SIZE];
				info.cbSize = sizeof(MENUITEMINFO);
				info.fMask = MIIM_ID | MIIM_TYPE | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_DATA | MIIM_STATE;
				info.dwTypeData = tempStr;

				for (int j = 0; j < count; j++) {
					info.dwTypeData = tempStr;
					info.cch = STRING_BUFFER_SIZE;
					::GetMenuItemInfo(tempMenu, j, TRUE, &info);
					::InsertMenuItem(m_TraySystemMenu, j, TRUE, &info);
				}
			}

			::GetCursorPos(&pt);
			SetForegroundWindow(m_hWnd);
			returnVal = ::TrackPopupMenu(m_TraySystemMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, m_hWnd, &rect);
			if (returnVal) {
				::PostMessage(window, WM_SYSCOMMAND, returnVal, NULL);
			}
			break;
		}
	}
	
	return lParam;
}

LRESULT HackItDlg::OnSize(UINT nType, int cx, int cy) 
{
	if (IsWindow(m_WindowList)) {
		::SetWindowPos(m_WindowList, NULL, 0, 0, cx, cy - 20, SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
	} 

	if (IsWindow(m_StatusBar)) {
		::SetWindowPos(m_StatusBar, NULL, 0, cy - 18, cx, 18, SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
	
	return -1;
}

extern int (__stdcall *MySetLayeredWindowAttributes)(struct HWND__ *,unsigned long,unsigned char,unsigned long);

LRESULT HackItDlg::OnSelchangedTree1(NMHDR* pNMHDR) 
{
	HWND window = (HWND)((NMTREEVIEW*)pNMHDR)->itemNew.lParam;
	HTREEITEM treeitem = (HTREEITEM)((NMTREEVIEW*)pNMHDR)->itemNew.hItem;
	char TempText[STRING_BUFFER_SIZE];
	char FullText[STRING_BUFFER_SIZE];
	HMENU pPopup = ::GetSubMenu(m_WindowPopup, 0);
	
	for (int i = 0; i < 10; i++) {
		::RemoveMenu(pPopup, 0, MF_BYPOSITION);
	}

	if (!::IsWindow(window) || !window) {
		// Not a window, so it shouldn't be in the list if it isn't a root item
		HWND_HTREEITEM_map::iterator itemPosition;

		// Window doesn't exist, delete it from our lists
		PostMessage(m_hWndManager, WM_DESTROYDETECTED, 0, (LPARAM)window);

		treeitem = NULL;

		::AppendMenu(pPopup, MF_STRING | MF_DISABLED, 0, "No Functions Available");
		ResetMenuChecks();
		::SetWindowText(m_StatusBar, "");
		UpdateWindow(m_hWnd);
	} else {
		// It is a window, update accordingly
		::AppendMenu(pPopup, MF_STRING, ID_VISIBLEHIDDEN, "Visible");
		::AppendMenu(pPopup, MF_STRING, ID_TRAYYEDUNTRAYYED, "Tray Icon");
		::AppendMenu(pPopup, MF_STRING, ID_TOPPEDUNTOPPED, "Topmost");
		::AppendMenu(pPopup, MF_SEPARATOR, NULL, NULL);

		HMENU tempMenu;
		m_ItemHwnd = window;
		bool capturedSomething = false;

		if (window != m_hWnd && (tempMenu = ::GetSystemMenu(window, FALSE)) != NULL) {
			int oldcount = ::GetMenuItemCount(m_ItemSystemMenu), count = ::GetMenuItemCount(tempMenu);
			MENUITEMINFO info;
			char tempStr[STRING_BUFFER_SIZE];
			info.cbSize = sizeof(MENUITEMINFO);
			info.fMask = MIIM_ID | MIIM_TYPE | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_DATA | MIIM_STATE;
			info.dwTypeData = tempStr;

			for (int i = 0; i < oldcount; i++) {
				::RemoveMenu(m_ItemSystemMenu, 0, MF_BYPOSITION);
			}
			
			for (int j = 0; j < count; j++) {
				info.dwTypeData = tempStr;
				info.cch = STRING_BUFFER_SIZE;
				::GetMenuItemInfo(tempMenu, j, TRUE, &info);
				::InsertMenuItem(m_ItemSystemMenu, j, TRUE, &info);
			}
			::AppendMenu(pPopup,MF_STRING | MF_POPUP, (UINT)m_ItemSystemMenu, "System Menu");

			capturedSomething = true;
		}

		if (window != m_hWnd && (tempMenu = ::GetMenu(window)) != NULL) {
			int oldcount = ::GetMenuItemCount(m_ItemMainMenu), count = ::GetMenuItemCount(tempMenu);
			MENUITEMINFO info;
			char tempStr[STRING_BUFFER_SIZE];
			info.cbSize = sizeof(MENUITEMINFO);
			info.fMask = MIIM_ID | MIIM_TYPE | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_DATA | MIIM_STATE;
			info.dwTypeData = tempStr;

			for (int i = 0; i < oldcount; i++) {
				::RemoveMenu(m_ItemMainMenu, 0, MF_BYPOSITION);
			}
			
			for (int j = 0; j < count; j++) {
				info.dwTypeData = tempStr;
				info.cch = STRING_BUFFER_SIZE;
				::GetMenuItemInfo(tempMenu, j, TRUE, &info);
				::InsertMenuItem(m_ItemMainMenu, j, TRUE, &info);
			}
			::AppendMenu(pPopup,MF_STRING | MF_POPUP, (UINT)m_ItemMainMenu, "Main Menu");

			capturedSomething = true;
		}

		if (capturedSomething) {
			::AppendMenu(pPopup, MF_SEPARATOR, NULL, NULL);
		}

		::AppendMenu(pPopup, MF_STRING | MF_POPUP, (UINT)m_hProcessMenu, "Process");
		::AppendMenu(pPopup, MF_STRING | MF_POPUP, (UINT)m_hWindowMenu, "Window");

		if (MySetLayeredWindowAttributes != NULL) {
			::AppendMenu(pPopup, MF_STRING | MF_POPUP, (UINT)m_hTransparencyMenu, "Transparency");
		}

		ResetMenuChecks();
		UpdateMenuChecks(window);
		GetClassName(window, TempText, STRING_BUFFER_SIZE);
		
		if (strcmp(TempText, "Button") == 0) {
			::AppendMenu(pPopup, MF_STRING | MF_POPUP, (UINT)m_hButtonMenu, "Button");
		} else if (strcmp(TempText, "Static") == 0) {
		} else if (strcmp(TempText, "Edit") == 0) {
		} else if (strcmp(TempText, "ComboBox") == 0) {
		} else if (strcmp(TempText, "ScrollBar") == 0) {
			//::AppendMenu(pPopup, MF_STRING | MF_POPUP, (UINT)m_hScrollbarMenu, "Scroll Bar");
		} else if (strcmp(TempText, "ListBox") == 0) {
		} else if (strcmp(TempText, "SysListView32") == 0) {
		} else if (strcmp(TempText, "SysTreeView32") == 0) {
		} else if (strcmp(TempText, "SysTabControl32") == 0) {
		} else if (strcmp(TempText, "TrayNotifyWnd") == 0) {
		}
		
	}
	
	if (treeitem == NULL ||
		treeitem == m_Desktop ||
		treeitem == m_HiddenItem ||
		treeitem == m_TrayyedItem ||
		treeitem == m_ToppedItem) {

		//char buffer[256];
		//sprintf(buffer, "%d total windows listed", m_WindowItemMap.size());
		::SetWindowText(m_StatusBar, "");//buffer);
		UpdateWindow(m_hWnd);
	} else {
		sprintf(FullText, "Class: %s", TempText);
		::SetWindowText(m_StatusBar, FullText);
		UpdateWindow(m_hWnd);
	}

	::ModifyMenu(m_MainMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT)pPopup, "Functions");

	return 0;
}

void HackItDlg::ResetMenuChecks()
{
	::CheckMenuItem(m_WindowPopup, ID_PROCESS_REALTIME, MF_BYCOMMAND | MF_UNCHECKED);
	::CheckMenuItem(m_WindowPopup, ID_PROCESS_HIGH, MF_BYCOMMAND | MF_UNCHECKED);
	::CheckMenuItem(m_WindowPopup, ID_PROCESS_NORMAL, MF_BYCOMMAND | MF_UNCHECKED);
	::CheckMenuItem(m_WindowPopup, ID_PROCESS_IDLE, MF_BYCOMMAND | MF_UNCHECKED);
	::CheckMenuItem(m_WindowPopup, ID_TRAYYEDUNTRAYYED, MF_BYCOMMAND | MF_UNCHECKED);
	::CheckMenuItem(m_WindowPopup, ID_VISIBLEHIDDEN, MF_BYCOMMAND | MF_UNCHECKED);
	::CheckMenuItem(m_WindowPopup, ID_TOPPEDUNTOPPED, MF_BYCOMMAND | MF_UNCHECKED);
	::CheckMenuItem(m_WindowPopup, ID_WINDOW_DISABLED, MF_BYCOMMAND | MF_UNCHECKED);
}

BOOL HackItDlg::AddIcon()
{
	BOOL res;
	
	res = Shell_NotifyIcon(NIM_ADD, &m_TrayIconData);
	
	return res;
}

BOOL HackItDlg::UpdateIcon()
{
	BOOL res;
	
	res = Shell_NotifyIcon(NIM_MODIFY, &m_TrayIconData);
	
	return res;
}

BOOL HackItDlg::DeleteIcon()
{
	BOOL res;
	
	res = Shell_NotifyIcon(NIM_DELETE, &m_TrayIconData);
	
	return res;
}

void HackItDlg::SetIconData(HWND hwndTgt, UINT hwndCtl)
{
	m_TrayIconData.cbSize = sizeof(NOTIFYICONDATA);
	m_TrayIconData.hWnd = hwndTgt;
	m_TrayIconData.uID = hwndCtl;
	HWND TempWindow = (HWND)hwndCtl;
	
	char TempText[64];
	::GetWindowText(TempWindow, TempText, 64);
	lstrcpyn(m_TrayIconData.szTip, TempText, 64);
	
	DWORD TempIcon;
	
	TempIcon = (DWORD)GetWindowIcon(TempWindow);

	if (TempIcon == NULL)
		TempIcon = (UINT)ImageList_ExtractIcon(NULL, m_ImageList, 2);
	m_TrayIconData.hIcon = (HICON)TempIcon;
	
	m_TrayIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	m_TrayIconData.uCallbackMessage = WM_USER+1;
}

BOOL CALLBACK HackItDlg::EnumChildWindowsProc(HWND hwnd, LPARAM lParam) {
	::PostMessage((HWND)lParam, WM_CREATEDETECTED, NULL, (LPARAM)hwnd);
	
	return TRUE;
}

BOOL CALLBACK HackItDlg::ConsolePollProc(HWND hwnd, LPARAM lParam) {
	char winClass[256];

	::GetClassName(hwnd, winClass, 256);

	if (!strcmp(winClass, "ConsoleWindowClass")) {
		::PostMessage((HWND)lParam, WM_CREATEDETECTED, NULL, (LPARAM)hwnd);
	}

	return TRUE;
}

void HackItDlg::CheckAuto(HWND TempWindow)
{
	int WARNING; // must synchronize access to autofunction list
	if (::IsWindow(TempWindow)) {
		AutoFunction *af;
		string_AutoFunction_map::iterator pos;
		
		pos = m_Settings.m_AutoFunctions.begin();
		
		while (pos != m_Settings.m_AutoFunctions.end()) {
			af = pos->second;
			if (af->match(TempWindow)) {
				AutoFunction::execute(this, af, TempWindow);
			}
			pos++;
		}
	}
}

LRESULT HackItDlg::OnClose() 
{
	return 0;
}

/*HTREEITEM HackItDlg::AddChildItem(HWND Window, HTREEITEM Parent)
{
	TV_INSERTSTRUCT TempItem;
	char TempText[STRING_BUFFER_SIZE], ClassText[STRING_BUFFER_SIZE];
	HTREEITEM TempHItem = 0;
	
	TempItem.hInsertAfter = TVI_SORT;
	TempItem.item.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_PARAM;
	
	::GetWindowText(Window, TempText, STRING_BUFFER_SIZE);
	if (strlen(TempText) == 0)
		strcpy(TempText, m_Settings.m_NamelessText.c_str());
	TempItem.item.pszText = TempText;

	::GetClassName(Window, ClassText, STRING_BUFFER_SIZE);

	// Get the icon for this window, or default to the flag
	unsigned long TempIcon;
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
	
	TempItem.item.lParam = long(Window);
	TempItem.hParent = Parent;
	TempHItem = TreeView_InsertItem(m_WindowList, &TempItem);

	HWND_HTREEITEM_map::value_type insertPair(Window, TempHItem);
	pair<HWND_HTREEITEM_map::iterator, bool> insertResult;

	LoggingSystem::log("testing");
	insertResult = m_WindowItemMap.insert(insertPair);
	if (!insertResult.second) {
		LoggingSystem::log("duplicate entry found in window/item map");
		m_WindowItemMap.erase(insertResult.first);
		m_WindowItemMap.insert(insertPair);
	}

	if (::GetWindowLong(Window, GWL_EXSTYLE) & WS_EX_TOPMOST) {
		AddToppedItem(TempHItem);
	}
	
	if (m_Settings.m_AutoFunctionsEnabled && m_Settings.m_ChildFunctionsEnabled) {
		CheckAuto(Window);
	}
	
	return TempHItem;
}*/

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

HRESULT HackItDlg::OnContextMenuTree1(HWND hWnd, int x, int y) 
{
	POINT mypoint, mypoint2;

	if (hWnd == m_hWnd) return -1;
	
	GetCursorPos(&mypoint);
	mypoint2 = mypoint;
	
	HMENU pPopup = GetSubMenu(m_WindowPopup, 0);
	
	::TrackPopupMenu(pPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY, mypoint.x, mypoint.y, 0, m_hWnd, NULL);
	
	return -1;
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

long HackItDlg::OnCheckTrayyedList(WPARAM wParam, LPARAM lParam) {
	EnterCriticalSection(&m_CS_TrayyedItemMap);
	long result = (m_TrayyedItemMap.count((HWND)wParam) != 0);
	LeaveCriticalSection(&m_CS_TrayyedItemMap);
	return result;
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

int HackItDlg::GetWindowItemIconIndex(HTREEITEM hItem)
{
	TV_ITEM tempItem;
	
	tempItem.hItem = hItem;
	tempItem.mask = TVIF_IMAGE;
	
	TreeView_GetItem(m_WindowList, &tempItem);
	
	return tempItem.iImage;
}


void HackItDlg::UpdateMenuChecks(HWND wnd) {
	// Can't do much with a null HWND
	if (!wnd) return;

	DWORD TempProcess;
	HANDLE process;
	
	GetWindowThreadProcessId(wnd, &TempProcess);
	process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, TempProcess);

	switch (GetPriorityClass(process)) {
	case REALTIME_PRIORITY_CLASS:
		::CheckMenuRadioItem(m_hProcessMenu, ID_PROCESS_REALTIME, ID_PROCESS_IDLE, ID_PROCESS_REALTIME, MF_BYCOMMAND);
		break;
	case HIGH_PRIORITY_CLASS:
		::CheckMenuRadioItem(m_hProcessMenu, ID_PROCESS_REALTIME, ID_PROCESS_IDLE, ID_PROCESS_HIGH, MF_BYCOMMAND);
		break;
	case NORMAL_PRIORITY_CLASS:
		::CheckMenuRadioItem(m_hProcessMenu, ID_PROCESS_REALTIME, ID_PROCESS_IDLE, ID_PROCESS_NORMAL, MF_BYCOMMAND);
		break;
	case IDLE_PRIORITY_CLASS:
		::CheckMenuRadioItem(m_hProcessMenu, ID_PROCESS_REALTIME, ID_PROCESS_IDLE, ID_PROCESS_IDLE, MF_BYCOMMAND);
		break;
	}

	CloseHandle(process);
	
	if (::IsWindowVisible(wnd))
	{
		::CheckMenuItem(m_WindowPopup, ID_VISIBLEHIDDEN, MF_BYCOMMAND | MF_CHECKED);
	}
	
	if (m_TrayyedItemMap.find(wnd) != m_TrayyedItemMap.end() || wnd == m_hWnd) {
		::CheckMenuItem(m_WindowPopup, ID_TRAYYEDUNTRAYYED, MF_BYCOMMAND | MF_CHECKED);
	}
	
	if (::GetWindowLong(wnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
	{
		::CheckMenuItem(m_WindowPopup, ID_TOPPEDUNTOPPED, MF_BYCOMMAND | MF_CHECKED);
	}
	
	if (::GetWindowLong(wnd, GWL_STYLE) & WS_DISABLED) {
		::CheckMenuItem(m_hWindowMenu, ID_WINDOW_DISABLED, MF_BYCOMMAND | MF_CHECKED);
	}
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

void HackItDlg::AboutDialog()
{
	AboutDlg dlgAbout(m_hWnd, m_hInstance);
	dlgAbout.DoModal();
}

void HackItDlg::Shutdown()
{
	SetWindowText(m_StatusBar, "Shutting Down Hack-It...");
	RedrawWindow(m_StatusBar, NULL, NULL, RDW_UPDATENOW);

	SetIconData(m_hWnd, UINT(m_hWnd));
	DeleteIcon();

	delete m_pHooks; // stop hooking

	// Quit the list manager thread
	//SendMessage(m_hWndManager, WM_QUIT, 0, 0);
	DestroyWindow(m_hWndManager);

	// Untray any trayyed icons
	HWND_HTREEITEM_map::iterator data;

	EnterCriticalSection(&m_CS_TrayyedItemMap);
	data = m_TrayyedItemMap.begin();
	while (data != m_TrayyedItemMap.end()) {
		SetWindowPos(data->first, 0, 0, 0, 0, 0,
			SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		SetIconData(m_hWnd, (LONG)data->first)	;
		DeleteIcon();
		m_TrayyedItemMap.erase(data);
		data = m_TrayyedItemMap.begin();
	}
	LeaveCriticalSection(&m_CS_TrayyedItemMap);
	
	m_Settings.SaveSettings();

	IllegalWindowSpec_set::iterator iter1;
	string_AutoFunction_map::iterator iter2;

	// delete IllegalWindowSpec objects
	iter1 = m_Settings.m_IllegalWindowSpecList.begin();
	while (iter1 != m_Settings.m_IllegalWindowSpecList.end()) {
		delete *iter1;
		iter1++;
	}

	// delete AutoFunction objects
	iter2 = m_Settings.m_AutoFunctions.begin();
	while (iter2 != m_Settings.m_AutoFunctions.end()) {
		delete iter2->second;
		iter2++;
	}

	// Destroy the menus
	::DestroyMenu(m_SysMenuPriorityMenu);
	::DestroyMenu(m_SysMenuTransparencyMenu);
	::DestroyMenu(m_ItemMainMenu);
	::DestroyMenu(m_ItemSystemMenu);

	DestroyIcon(m_hIconSM);
	DestroyIcon(m_hIcon);

	// Delete Critical Sections
	DeleteCriticalSection(&m_CS_TrayyedItemMap);
	DeleteCriticalSection(&m_CS_ToppedItemMap);
	DeleteCriticalSection(&m_CS_WindowItemMap);
	DeleteCriticalSection(&m_CS_ReusableIconList);
}

HICON HackItDlg::GetWindowIcon(HWND window)
{
	HICON hIcon = NULL;
 	
	if (m_Settings.m_UseLargeIcons) {
		if (m_Settings.m_WindowIconFirst) {
			::SendMessageTimeout(window, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 500, (DWORD*)&hIcon);
 			
			if (hIcon <= NULL) {
				hIcon = (HICON)GetClassLong(window, GCL_HICON);
			}
		} else {
			hIcon = (HICON)GetClassLong(window, GCL_HICON);
 			
			if (hIcon <= NULL) {
				::SendMessageTimeout(window, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 500, (DWORD*)&hIcon);
			}
		}

		if (m_Settings.m_WindowIconFirst) {
			if (hIcon <= NULL) {
				::SendMessageTimeout(window, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG, 500, (DWORD*)&hIcon);
			}

			if (hIcon <= NULL) {
				hIcon = (HICON)GetClassLong(window, GCL_HICONSM);
			}
		} else {
			if (hIcon <= NULL) {
				hIcon = (HICON)GetClassLong(window, GCL_HICONSM);
			}

			if (hIcon <= NULL) {
				::SendMessageTimeout(window, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG, 500, (DWORD*)&hIcon);
			}
		}
	} else {
		if (m_Settings.m_WindowIconFirst) {
			::SendMessageTimeout(window, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG, 500, (DWORD*)&hIcon);
 			
			if (hIcon <= NULL) {
				hIcon = (HICON)GetClassLong(window, GCL_HICONSM);
			}
		} else {
			hIcon = (HICON)GetClassLong(window, GCL_HICONSM);
 			
			if (hIcon <= NULL) {
				::SendMessageTimeout(window, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG, 500, (DWORD*)&hIcon);
			}
		}

		if (m_Settings.m_WindowIconFirst) {
			if (hIcon <= NULL) {
				::SendMessageTimeout(window, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 500, (DWORD*)&hIcon);
			}

			if (hIcon <= NULL) {
				hIcon = (HICON)GetClassLong(window, GCL_HICON);
			}
		} else {
			if (hIcon <= NULL) {
				hIcon = (HICON)GetClassLong(window, GCL_HICON);
			}

			if (hIcon <= NULL) {
				::SendMessageTimeout(window, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 500, (DWORD*)&hIcon);
			}
		}
	}

	if (hIcon < NULL) {
		hIcon = NULL;
	}

	return hIcon;//::CopyIcon(hIcon);
}

int HackItDlg::GetClassIcon(HWND window)
{
	int index;
	char classText[STRING_BUFFER_SIZE];

	::GetClassName(window, classText, STRING_BUFFER_SIZE);
	
	if (strcmp(classText, BUTTON_CLASS_TEXT) == 0) {
		index = BUTTON_ICON_INDEX;
	} else if (strcmp(classText, STATIC_CLASS_TEXT) == 0) {
		index = STATIC_ICON_INDEX;
	} else if (strcmp(classText, EDIT_CLASS_TEXT) == 0) {
		index = EDIT_ICON_INDEX;
	} else if (strcmp(classText, COMBOBOX_CLASS_TEXT) == 0) {
		index = COMBOBOX_ICON_INDEX;
	} else if (strcmp(classText, SCROLLBAR_CLASS_TEXT) == 0) {
		index = SCROLLBAR_ICON_INDEX;
	} else if (strcmp(classText, LISTBOX_CLASS_TEXT) == 0) {
		index = LISTBOX_ICON_INDEX;
	} else if (strcmp(classText, LISTVIEW_CLASS_TEXT) == 0) {
		index = LISTVIEW_ICON_INDEX;
	} else if (strcmp(classText, TREEVIEW_CLASS_TEXT) == 0) {
		index = TREEVIEW_ICON_INDEX;
	} else if (strcmp(classText, TABCONTROL_CLASS_TEXT) == 0) {
		index = TABCONTROL_ICON_INDEX;
	} else if (strcmp(classText, TOOLTIP_CLASS_TEXT) == 0) {
		index = TOOLTIP_ICON_INDEX;
	} else if (strcmp(classText, STATUS_CLASS_TEXT) == 0) {
		index = STATUS_ICON_INDEX;
	} else if (strcmp(classText, MDICLIENT_CLASS_TEXT) == 0) {
		index = MDICLIENT_ICON_INDEX;
	} else if (strcmp(classText, TOOLBAR_CLASS_TEXT) == 0) {
		index = TOOLBAR_ICON_INDEX;
	} else if (strcmp(classText, DIALOG_CLASS_TEXT) == 0) {
		index = DIALOG_ICON_INDEX;
	} else if (strcmp(classText, TRAYWINDOW_CLASS_TEXT) == 0) {
		index = TRAYWINDOW_ICON_INDEX;
	} else {
		index = DEFAULT_ICON_INDEX;
	}

	return index;
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

void HackItDlg::QuickMessage(LPCTSTR text)
{
	MessageBox(NULL, text, "QuickMessage", MB_OK | MB_ICONEXCLAMATION);
}

void HackItDlg::Start()
{
	MSG msg;

	OnCreate(NULL, NULL);

	m_HackItRunning = TRUE;

	while (GetMessage(&msg, m_hWnd, 0, 0) && m_HackItRunning) {
		// If we're no longer getting valid messages, quit
		if (!msg.message) break;

		try {
			// else, translate and dispatch the message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} catch (...) {
			LoggingSystem::log("Exception caught");
		}
	}
}

LRESULT HackItDlg::OnRclickTree1(NMHDR* pnmh) {
	POINT mypoint;
	TV_HITTESTINFO tvHitTestInfo;

	GetCursorPos(&mypoint);
	::ScreenToClient(m_WindowList, &mypoint);
	tvHitTestInfo.pt = mypoint;
	TreeView_HitTest(m_WindowList, &tvHitTestInfo);
	TreeView_SelectItem(m_WindowList, tvHitTestInfo.hItem);
	return 0;
}

LRESULT HackItDlg::OnNotify(WPARAM wParam, LPARAM lParam)
{
	int idCtrl = (int)wParam;
	NMHDR *pnmh = (LPNMHDR)lParam;

	if (pnmh->hwndFrom == m_WindowList) {
		switch (pnmh->code) {
		case TVN_SELCHANGED:
			return OnSelchangedTree1(pnmh);
			break;
		case NM_RCLICK:
			return OnRclickTree1(pnmh);
			break;
		case NM_DBLCLK:
			return OnDoubleClickTree1(pnmh);
			break;
		default:
			return -1;
		}
	} else {
		return -1;
	}
}

void HackItDlg::Settings()
{
	int WARNING; // need to synchronize settings since many are live
	int result;

	SettingsDlg settings(m_hWnd, m_hInstance);
	settings.Initialize(m_Settings);

	result = settings.DoModal();

	if (result < 0) {
		char astring[STRING_BUFFER_SIZE];
		DWORD error = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, NULL, astring, STRING_BUFFER_SIZE, NULL);
		MessageBox(NULL, astring, astring, MB_OK);
	} else  {
        // apply new settings, make app changes before saving
		if (settings.m_Settings.m_ConsolePolling) {
			SetTimer(m_hWndManager, CONSOLE_WINDOW_POLL_TIMER, m_Settings.m_ConsolePollingTime, NULL);
		} else if (!settings.m_Settings.m_ConsolePolling && m_Settings.m_ConsolePolling) {
			KillTimer(m_hWndManager, CONSOLE_WINDOW_POLL_TIMER);
		}

		if (settings.m_Settings.m_ConsolePolling && (settings.m_Settings.m_ConsolePollingTime != m_Settings.m_ConsolePollingTime)) {
			KillTimer(m_hWndManager, CONSOLE_WINDOW_POLL_TIMER);
			SetTimer(m_hWndManager, CONSOLE_WINDOW_POLL_TIMER, m_Settings.m_ConsolePollingTime, NULL);
		}

		if (settings.m_Settings.m_WindowListCleaning && !m_Settings.m_WindowListCleaning) {
			SetTimer(m_hWndManager, WINDOW_LIST_CLEANER_TIMER, m_Settings.m_WindowListCleanerTime, NULL);
		} else if (!settings.m_Settings.m_WindowListCleaning && m_Settings.m_WindowListCleaning) {
			KillTimer(m_hWndManager, WINDOW_LIST_CLEANER_TIMER);
		}

		if (settings.m_Settings.m_WindowListCleaning && (settings.m_Settings.m_WindowListCleanerTime != m_Settings.m_WindowListCleanerTime)) {
			KillTimer(m_hWndManager, WINDOW_LIST_CLEANER_TIMER);
			SetTimer(m_hWndManager, WINDOW_LIST_CLEANER_TIMER, m_Settings.m_WindowListCleanerTime, NULL);
		}

		SetPriorityClass(m_hWnd, settings.m_Settings.m_Priority);

		if (settings.m_Settings.m_AlwaysOnTop) {
			::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		} else {
			::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}

		if (settings.m_Settings.m_Transparent && MySetLayeredWindowAttributes) {
			WindowFunctions::SetTransparency(m_hWnd, settings.m_Settings.m_TransparentPercent);
		} else {
			WindowFunctions::SetTransparency(m_hWnd, 0);
		}

		if (settings.m_Settings.m_UseLargeIcons && !m_Settings.m_UseLargeIcons) {
			ImageList_SetIconSize(m_ImageList, 32, 32);

		} else if (!settings.m_Settings.m_UseLargeIcons && m_Settings.m_UseLargeIcons) {
			ImageList_SetIconSize(m_ImageList, 16, 16);
		}

		HFONT font = ::CreateFontIndirect(&(settings.m_Settings.m_LogFont));
		::SendMessage(m_WindowList, WM_SETFONT, (WPARAM)font, NULL);

		// set current settings to new settings
		m_Settings = settings.m_Settings;
		m_Settings.SaveSettings();
	}
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

		if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes) {
			::AppendMenu(m_RedirectSysMenu, MF_POPUP, (UINT)m_SysMenuTransparencyMenu, "Transparency");
		}
	} else {
		HRESULT hr = ::InsertMenu(m_RedirectSysMenu, 0, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);

		if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes) {
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
	if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes) {
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
		if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes) {
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
		if (m_Settings.m_SysMenuTransparency && MySetLayeredWindowAttributes) {
			RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
		}
		// remove seperator
		RemoveMenu(m_RedirectSysMenu, 0, MF_BYPOSITION);
	}

	m_RedirectSysMenu = NULL;
}

static void (WINAPI *SwitchToThisWindow)(HWND, int) = NULL;

LRESULT HackItDlg::OnDoubleClickTree1(NMHDR *pNMHDR)
{
	POINT mypoint;
	TV_HITTESTINFO tvHitTestInfo;
	WINDOWPLACEMENT wndPlace;
	HWND window;

	if (!SwitchToThisWindow) {
		SwitchToThisWindow = (void (WINAPI *)(HWND, int)) GetProcAddress(
			GetModuleHandle("USER32.DLL"),
			"SwitchToThisWindow");
	}

	GetCursorPos(&mypoint);
	::ScreenToClient(m_WindowList, &mypoint);
	tvHitTestInfo.pt = mypoint;
	TreeView_HitTest(m_WindowList, &tvHitTestInfo);
	window = GetWindowItemHwnd(tvHitTestInfo.hItem);
	
	if (::IsIconic(window)) {
		wndPlace.showCmd = SW_RESTORE;
		::SetWindowPlacement(window, &wndPlace);
	}

	SwitchToThisWindow(window, 1);

	return TRUE;
}

void HackItDlg::OnContextHelp(WPARAM wParam, LPARAM lParam)
{
	HELPINFO *pHelpInfo = (LPHELPINFO)lParam;

	switch (pHelpInfo->iContextType) {
	case HELPINFO_MENUITEM:
		break;
	case HELPINFO_WINDOW:
		if (pHelpInfo->hItemHandle == m_WindowList) {
			HtmlHelp(NULL, "HackIt.chm::/WindowList.html", HH_DISPLAY_TOPIC, 0);
		} else if (pHelpInfo->hItemHandle == m_StatusBar) {
			HtmlHelp(NULL, "HackIt.chm::/StatusBar.html", HH_DISPLAY_TOPIC, 0);
		}
		break;
	}
}

void HackItDlg::WindowInfo()
{
	HTREEITEM item;

	item = TreeView_GetSelection(m_WindowList);

	if (item == NULL ||
		item == m_Desktop ||
		item == m_HiddenItem ||
		item == m_ToppedItem ||
		item == m_TrayyedItem) return;


	WindowInfoDlg info(m_hWnd, m_hInstance);

	info.init(GetWindowItemHwnd(item));

	info.DoModal();
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
