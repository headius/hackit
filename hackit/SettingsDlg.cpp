// SettingsDlg.cpp: implementation of the SettingsDlg class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <commctrl.h>
#include <htmlhelp.h>
#include "resource.h"
#include "defines.h"
#include "HackItSettings.h"
#include "SystemHooks.h"
#include "HackItDlg.h"
#include "IllegalWindowEntry.h"
#include "IllegalWindowSpec.h"
#include "AutoEntry.h"
#include "AutoFunction.h"
#include "SettingsDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int (__stdcall *MySetLayeredWindowAttributes)(struct HWND__ *,unsigned long,unsigned char,unsigned long);

SettingsDlg::SettingsDlg(HWND hWndParent, HINSTANCE hInstance)
{
	m_hWndParent = hWndParent;
	m_hInstance = hInstance;
	PROPSHEETPAGE propSheetPage;

	propSheetPage.dwSize = sizeof(propSheetPage);
	propSheetPage.hInstance = m_hInstance;
	propSheetPage.dwFlags = PSP_DEFAULT | PSP_HASHELP;
	propSheetPage.lParam = (LPARAM)this;

	propSheetPage.pfnDlgProc = WindowDlgProc;
	propSheetPage.pszTemplate = MAKEINTRESOURCE(IDD_SETTINGS_WINDOWS);
	m_WindowSettingsPage = CreatePropertySheetPage(&propSheetPage);
	if (m_WindowSettingsPage == NULL) {
		MessageBox(NULL, "Could not create window page", "Settings", MB_OK);
	}

	propSheetPage.pfnDlgProc = GeneralDlgProc;
	propSheetPage.pszTemplate = MAKEINTRESOURCE(IDD_SETTINGS_GENERAL);
	m_GeneralSettingsPage = CreatePropertySheetPage(&propSheetPage);
	if (m_GeneralSettingsPage == NULL) {
		MessageBox(NULL, "Could not create general page", "Settings", MB_OK);
	}

	propSheetPage.pfnDlgProc = IllegalDlgProc;
	propSheetPage.pszTemplate = MAKEINTRESOURCE(IDD_ILLEGALWINDOWDLG);
	m_IllegalSettingsPage = CreatePropertySheetPage(&propSheetPage);
	if (m_IllegalSettingsPage == NULL) {
		MessageBox(NULL, "Could not create illegal page", "Settings", MB_OK);
	}

	propSheetPage.pfnDlgProc = FunctionDlgProc;
	propSheetPage.pszTemplate = MAKEINTRESOURCE(IDD_AUTO);
	m_FunctionSettingsPage = CreatePropertySheetPage(&propSheetPage);
	if (m_FunctionSettingsPage == NULL) {
		MessageBox(NULL, "Could not create function page", "Settings", MB_OK);
	}

	m_NamelessTextFocused = false;
}

SettingsDlg::~SettingsDlg()
{
	vector<AutoFunction*>::iterator iter = m_FuncsToDelete.begin();

	while (iter != m_FuncsToDelete.end()) {
		delete *iter;
		m_FuncsToDelete.erase(iter);
		iter = m_FuncsToDelete.begin();
	}
}

BOOL CALLBACK SettingsDlg::GeneralDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	SettingsDlg* theDlg = (SettingsDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (SettingsDlg*)((PROPSHEETPAGE*)lParam)->lParam;
		theDlg->m_hWndGeneralDlg = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
		theDlg->InitGeneralDlg();
		return TRUE;
		break;
	case WM_COMMAND:
		if (theDlg != NULL) {
			return theDlg->OnCommand(wParam, lParam);
		}
		break;
	case WM_NOTIFY:
		NMHDR *pnmh = (LPNMHDR)lParam;
		switch (pnmh->code) {
		case PSN_RESET:
			theDlg->m_Settings = theDlg->m_OldSettings;
			break;
		case PSN_HELP:
			::HtmlHelp(NULL, "HackIt.chm::/general_tab.html", HH_DISPLAY_TOPIC, 0);
			return true;
			break;
		}
	}

	return FALSE;
}

BOOL CALLBACK SettingsDlg::WindowDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	SettingsDlg* theDlg = (SettingsDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (SettingsDlg*)((PROPSHEETPAGE*)lParam)->lParam;
		theDlg->m_hWndWindowDlg = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
		theDlg->InitWindowDlg();
		return TRUE;
		break;
	case WM_COMMAND:
		if (theDlg != NULL) {
			return theDlg->OnWindowDlgCommand(wParam, lParam);
		}
		break;
	case WM_NOTIFY:
		NMHDR *pNMHDR = (LPNMHDR)lParam;
		switch (pNMHDR->code) {
		case PSN_HELP:
			::HtmlHelp(NULL, "HackIt.chm::/window_list_tab.html", HH_DISPLAY_TOPIC, 0);
			return true;
			break;
		}
		break;
	}

	return FALSE;
}

BOOL CALLBACK SettingsDlg::IllegalDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	SettingsDlg* theDlg = (SettingsDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (SettingsDlg*)((PROPSHEETPAGE*)lParam)->lParam;
		theDlg->m_hWnd = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
		theDlg->InitIllegalDlg();
		return true;
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case BN_CLICKED:
			switch (LOWORD(wParam)) {
			case IDC_ADDILLEGALWINDOW:
				theDlg->AddIllegalWindow();
				return TRUE;
				break;
			case IDC_EDITILLEGALWINDOW:
				theDlg->EditIllegalWindow();
				return TRUE;
				break;
			case IDC_DELETEILLEGALWINDOW:
				theDlg->DeleteIllegalWindow();
				return TRUE;
				break;
			}
			break;
		}
		break;
	case WM_NOTIFY:
		NMHDR *pNMHDR = (LPNMHDR)lParam;
		switch (pNMHDR->code) {
		case PSN_HELP:
			::HtmlHelp(NULL, "HackIt.chm::/illegal_windows_tab.html", HH_DISPLAY_TOPIC, 0);
			return true;
			break;
		}
		break;
	}

	return FALSE;
}

void SettingsDlg::AddIllegalWindow() 
{
	IllegalWindowEntryDlg entry(m_hWnd, m_hInstance);
	if ( entry.DoModal() == IDOK ) {
		IllegalWindowSpec* pSpec = new IllegalWindowSpec();
		pSpec->SetName(entry.GetIllegalName());
		pSpec->SetFilename(entry.GetIllegalFilename());
		pSpec->SetTitle(entry.GetIllegalTitle());
		pSpec->SetClass(entry.GetIllegalClass());
		pSpec->SetCaseSensitive(entry.GetIllegalCaseSensitive());
		m_Settings.m_IllegalWindowSpecList.insert(pSpec);

		LVITEM item;

		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.iItem = ListView_GetItemCount(m_IllegalWindowList);
		item.iSubItem = 0;
		item.lParam = (LPARAM)pSpec;
		item.pszText = (char*)pSpec->GetName().c_str();
		ListView_InsertItem(m_IllegalWindowList, &item);

		item.mask = LVIF_TEXT;
		item.pszText = (char*)pSpec->GetTitle().c_str();
		item.iSubItem = 1;
		ListView_SetItem(m_IllegalWindowList, &item);

		item.pszText = (char*)pSpec->GetClass().c_str();
		item.iSubItem = 2;
		ListView_SetItem(m_IllegalWindowList, &item);

		item.pszText = (char*)pSpec->GetFilename().c_str();
		item.iSubItem = 3;
		ListView_SetItem(m_IllegalWindowList, &item);
	}
}

void SettingsDlg::EditIllegalWindow()
{
	int index ;
	IllegalWindowSpec* pSpec ;

	index = ListView_GetNextItem(m_IllegalWindowList, -1, LVNI_SELECTED);
	if (index != -1) {
		LVITEM item;
		item.mask = LVIF_PARAM;
		item.iItem = index;

		ListView_GetItem(m_IllegalWindowList, &item);
		pSpec = (IllegalWindowSpec*)item.lParam;

		IllegalWindowEntryDlg entry(m_hWnd, m_hInstance);
		entry.Initialize(pSpec);
		if ( entry.DoModal( ) == IDOK ) {
			pSpec->SetFilename(entry.GetIllegalFilename());
			pSpec->SetTitle(entry.GetIllegalTitle());
			pSpec->SetClass(entry.GetIllegalClass());
			pSpec->SetCaseSensitive(entry.GetIllegalCaseSensitive());

			ListView_DeleteItem(m_IllegalWindowList, index);

			LVITEM item;

			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.iItem = index;
			item.iSubItem = 0;
			item.lParam = (LPARAM)pSpec;
			item.pszText = (char*)pSpec->GetName().c_str();
			ListView_InsertItem(m_IllegalWindowList, &item);

			item.mask = LVIF_TEXT;
			item.pszText = (char*)pSpec->GetTitle().c_str();
			item.iSubItem = 1;
			ListView_SetItem(m_IllegalWindowList, &item);

			item.pszText = (char*)pSpec->GetClass().c_str();
			item.iSubItem = 2;
			ListView_SetItem(m_IllegalWindowList, &item);

			item.pszText = (char*)pSpec->GetFilename().c_str();
			item.iSubItem = 3;
			ListView_SetItem(m_IllegalWindowList, &item);
		}
	}
}

void SettingsDlg::DeleteIllegalWindow() 
{
	int index ;
	IllegalWindowSpec* pSpec ;

	index = ListView_GetNextItem(m_IllegalWindowList, -1, LVNI_SELECTED);
	if (index != -1) {
		LVITEM item;
		item.mask = LVIF_PARAM;
		item.iItem = index;

		ListView_GetItem(m_IllegalWindowList, &item);
		pSpec = (IllegalWindowSpec*)item.lParam;

		IllegalWindowSpec_set::iterator position = m_Settings.m_IllegalWindowSpecList.find(pSpec);
		if (position != m_Settings.m_IllegalWindowSpecList.end()) {
			delete *position;
			m_Settings.m_IllegalWindowSpecList.erase(position);
		}
		ListView_DeleteItem(m_IllegalWindowList, index);
	}
}

BOOL CALLBACK SettingsDlg::FunctionDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	SettingsDlg* theDlg = (SettingsDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (SettingsDlg*)((PROPSHEETPAGE*)lParam)->lParam;
		theDlg->m_hWndFunctionDlg = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
		theDlg->InitFunctionDlg();
		return TRUE;
		break;
	case WM_COMMAND:
		switch (HIWORD(wParam)) {	
		case BN_CLICKED:	
			switch (LOWORD(wParam)) {
			case IDC_ADDITEM:
				theDlg->AddFunction();
				return TRUE;
				break;
			case IDC_EDITITEM:
				theDlg->EditFunction();
				return TRUE;
				break;
			case IDC_DELETEITEM:
				theDlg->DeleteFunction();
				return TRUE;
				break;
			}
			break;
		case LBN_SELCHANGE:
			switch (LOWORD(wParam)) {
			case IDC_AUTOENTRIES:
				theDlg->SelchangeFunctions();
				return TRUE;
				break;
			}
			break;
		}
		break;
		case WM_NOTIFY:
			NMHDR *pNMHDR = (LPNMHDR)lParam;
			switch (pNMHDR->code) {
			case PSN_HELP:
				::HtmlHelp(NULL, "HackIt.chm::/automatic_functions_tab.html", HH_DISPLAY_TOPIC, 0);
				return true;
				break;
			}
			break;
	}

	return FALSE;
}

void SettingsDlg::AddFunction()
{
	int result;

	AutoEntryDlg entryDlg(m_hWndFunctionDlg, m_hInstance);
	entryDlg.Initialize(this, -1);

	result = entryDlg.DoModal();
	if(result == -1) {
		char astring[256];
		DWORD error = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, NULL, astring, 256, NULL);
		MessageBox(NULL, astring, astring, MB_OK);
	}

	SelchangeFunctions();
}

void SettingsDlg::DeleteFunction() 
{
	AutoFunction *af;

	int index = SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_GETCURSEL, 0, 0);
	if (index == -1) {
		return;
	}

	char astring[STRING_BUFFER_SIZE];

	SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_GETTEXT, index, (LPARAM)astring);
	af = (AutoFunction*)SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_GETITEMDATA, index, 0);
	if (af != NULL) {
		m_FuncsToDelete.push_back(af);
		SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_SETITEMDATA, index, 0);
	}

	string newString(astring);

	string_AutoFunction_map::iterator position = m_Settings.m_AutoFunctions.find(newString);
	
	if (position != m_Settings.m_AutoFunctions.end()) {
		m_Settings.m_AutoFunctions.erase(position);
		SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_DELETESTRING, index, 0);
		SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_SETCURSEL, -1, 0);
		SetDlgItemText(m_hWndFunctionDlg, IDC_INFOEDIT, "");
	}
}


void SettingsDlg::EditFunction() 
{
	AutoEntryDlg entryDlg(m_hWndFunctionDlg, m_hInstance);

	entryDlg.Initialize(this, SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_GETCURSEL, 0, 0));
	entryDlg.DoModal();
	SetDlgItemText(m_hWnd, IDC_INFOEDIT, "");

	SelchangeFunctions();
}

void SettingsDlg::SelchangeFunctions() 
{
	AutoFunction *af;
	char astring[STRING_BUFFER_SIZE];
	char info[STRING_BUFFER_SIZE_LARGE];
	char infobuf[STRING_BUFFER_SIZE_LARGE];

	strcpy(info, "");
	strcpy(infobuf, "");
	strcpy(astring, "");

	int index = SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_GETCURSEL, 0, 0);

	if (index != LB_ERR) {
		SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_GETTEXT, index, (UINT)(LPCTSTR)astring);
		af = (AutoFunction*)SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_GETITEMDATA, index, 0);
		
		if (af->getWinClass().length() > 0) {
			sprintf(infobuf, "Class: \t\t%s\r\n", af->getWinClass().c_str());
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getTitle().length() > 0) {
			sprintf(infobuf, "Title: \t\t%s\r\n", af->getTitle().c_str());
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getFilename().length() > 0) {
			sprintf(infobuf, "File Name: \t%s\r\n", af->getFilename().c_str());
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getCaseSensitive()) {
			sprintf(infobuf, "Case Sensitive: \tTrue\r\n");
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getWindowAction()) {
			switch (af->getWindowAction()) {
			case AUTO_ACTION_RESTORE:
				sprintf(infobuf, "Window Action: \tRestore\r\n");
				break;
			case AUTO_ACTION_MINIMIZE:
				sprintf(infobuf, "Window Action: \tMinimize\r\n");
				break;
			case AUTO_ACTION_MAXIMIZE:
				sprintf(infobuf, "Window Action: \tMaximize\r\n");
				break;
			}
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getTopmostAction()) {
			switch (af->getTopmostAction()) {
			case AUTO_ACTION_NOTOPMOST:
				sprintf(infobuf, "Topmost Action: \tStandard\r\n");
				break;
			case AUTO_ACTION_TOPMOST:
				sprintf(infobuf, "Topmost Action: \tTopmost\r\n");
				break;
			case AUTO_ACTION_BOTTOM:
				sprintf(infobuf, "Topmost Action: \tBottom\r\n");
				break;
			}
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getVisibilityAction()) {
			switch (af->getVisibilityAction()) {
			case AUTO_ACTION_VISIBLE:
				sprintf(infobuf, "Visibility Action: \tVisible\r\n");
				break;
			case AUTO_ACTION_HIDDEN:
				sprintf(infobuf, "Visibility Action: \tHidden\r\n");
				break;
			}
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (MySetLayeredWindowAttributes) {
			switch (af->getTransparency()) {
			case 0:
				sprintf(infobuf, "Transparency: \tNone\r\n");
				break;
			default:
				sprintf(infobuf, "Transparency: \t%d0%%\r\n", af->getTransparency());
				break;
			}
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getPriorityAction()) {
			switch (af->getPriorityAction()) {
			case AUTO_ACTION_IDLE:
				sprintf(infobuf, "Priority Action: \tIdle\r\n");
				break;
			case AUTO_ACTION_NORMAL:
				sprintf(infobuf, "Priority Action: \tNormal\r\n");
				break;
			case AUTO_ACTION_HIGH:
				sprintf(infobuf, "Priority Action: \tHigh\r\n");
				break;
			case AUTO_ACTION_REALTIME:
				sprintf(infobuf, "Priority Action: \tReal-Time\r\n");
				break;
			}
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getDestructionAction()) {
			switch (af->getDestructionAction()) {
			case AUTO_ACTION_CLOSEWINDOW:
				sprintf(infobuf, "Destruction Action: \tClose Window\r\n");
				break;
			case AUTO_ACTION_ENDTASK:
				sprintf(infobuf, "Destruction Action: \tEnd Task\r\n");
				break;
			}
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getResize()) {
			sprintf(infobuf, "Resize Width: \t%d\r\n", af->getResizeW());
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
			sprintf(infobuf, "Resize Height: \t%d\r\n", af->getResizeH());
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getMove()) {
			sprintf(infobuf, "Move X: \t\t%d\r\n", af->getMoveX());
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
			sprintf(infobuf, "Move Y: \t\t%d\r\n", af->getMoveY());
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getSpecialAction()) {
			switch (af->getSpecialAction()) {
			case AUTO_ACTION_PRESSBUTTON:
				sprintf(infobuf, "Special Action: \tPress Button\r\n");
				break;
			}
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
			sprintf(infobuf, "Special Data: \t%s\r\n", af->getSpecialData().c_str());
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getAlternateTarget()) {
			switch (af->getAlternateTarget()) {
			case AUTO_TARGET_CHILD:
				sprintf(infobuf, "Alternate Target: \tChild\r\n");
				strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
				sprintf(infobuf, "Child: \t\t%s\r\n", af->getAlternateData().c_str());
				break;
			case AUTO_TARGET_PARENT:
				sprintf(infobuf, "Alternate Target: \tParent\r\n");
				break;
			}
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getTrayIcon()) {
			sprintf(infobuf, "Tray Icon: \tTrue\r\n");
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}

		if (af->getTopOnly()) {
			sprintf(infobuf, "Top-level Only: \tTrue\r\n");
			strncat(info, infobuf, STRING_BUFFER_SIZE_LARGE);
		}
	}
	
	SetDlgItemText(m_hWndFunctionDlg, IDC_INFOEDIT, info);
}

void SettingsDlg::InitWindowDlg() {
	::CheckDlgButton(m_hWndWindowDlg, IDC_CHILDREN, (m_Settings.m_ShowChilds ? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_TITLELESS, ((m_Settings.m_LowestShowable == '!') ? MF_UNCHECKED : MF_CHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_REFRESHTEXT, (m_Settings.m_UpdateItems ? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_TOPMOSTCHILDREN, (m_Settings.m_ChildrenInTopmostList? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_UPDATEADDREMOVE, (m_Settings.m_UpdateAddRemove? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_UPDATEAUTO, (m_Settings.m_UpdateAuto? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_CONSOLEPOLL, (m_Settings.m_ConsolePolling? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_CLEANLIST, (m_Settings.m_WindowListCleaning? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_USELARGEICONS, (m_Settings.m_UseLargeIcons? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_WINDOWICONFIRST, (m_Settings.m_WindowIconFirst? MF_CHECKED : MF_UNCHECKED));

	SetDlgItemInt(m_hWndWindowDlg, IDC_CONSOLEPOLLINGTIME, m_Settings.m_ConsolePollingTime, FALSE);
	SetDlgItemInt(m_hWndWindowDlg, IDC_WINDOWLISTCLEANERTIME, m_Settings.m_WindowListCleanerTime, FALSE);
	//SendDlgItemMessage(m_hWndWindowDlg, IDC_DELAYSPIN, UDM_SETRANGE, 0, MAKELONG(10, 1));
	SetDlgItemText(m_hWndWindowDlg, IDC_NAMELESSTEXT, m_Settings.m_NamelessText.c_str());

	::CheckDlgButton(m_hWndWindowDlg, IDC_SYSMENUCHECK, (m_Settings.m_SysMenuCapture ? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_SYSMENU_APPEND, (m_Settings.m_SysMenuAppend ? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_SYSMENUVISIBLE, (m_Settings.m_SysMenuVisible ? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_SYSMENUTOPPED, (m_Settings.m_SysMenuTopped ? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_SYSMENUTRAYYED, (m_Settings.m_SysMenuTrayyed ? MF_CHECKED : MF_UNCHECKED));
	::CheckDlgButton(m_hWndWindowDlg, IDC_SYSMENUPRIORITY, (m_Settings.m_SysMenuPriority ? MF_CHECKED : MF_UNCHECKED));
	if (MySetLayeredWindowAttributes) {
		::CheckDlgButton(m_hWndWindowDlg, IDC_SYSMENUTRANSPARENCY, (m_Settings.m_SysMenuTransparency ? MF_CHECKED : MF_UNCHECKED));
	} else {
		HWND sysmenuTransparency = GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTRANSPARENCY);
		EnableWindow(sysmenuTransparency, FALSE);
	}

	if (m_Settings.m_SysMenuCapture) {
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUVISIBLE), TRUE);
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTOPPED), TRUE);
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTRAYYED), TRUE);
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUPRIORITY), TRUE);
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENU_APPEND), TRUE);
		if (MySetLayeredWindowAttributes) {
			EnableWindow(GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTRANSPARENCY), TRUE);
		}
	} else {
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUVISIBLE), FALSE);
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTOPPED), FALSE);
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTRAYYED), FALSE);
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUPRIORITY), FALSE);
		::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENU_APPEND), FALSE);
		EnableWindow(GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTRANSPARENCY), FALSE);
	}
}

void SettingsDlg::InitGeneralDlg() {
	::CheckDlgButton(m_hWndGeneralDlg, IDC_SHOWSPLASH, (m_Settings.m_ShowSplash ? BST_CHECKED : BST_UNCHECKED));
	::CheckDlgButton(m_hWndGeneralDlg, IDC_HIDEONSTART, (m_Settings.m_HideOnStart ? BST_CHECKED : BST_UNCHECKED));
	::CheckDlgButton(m_hWndGeneralDlg, IDC_AUTOFUNCTIONSENABLED, (m_Settings.m_AutoFunctionsEnabled ? BST_CHECKED : BST_UNCHECKED));
	::CheckDlgButton(m_hWndGeneralDlg, IDC_CHILDFUNCTIONSENABLED, (m_Settings.m_ChildFunctionsEnabled ? BST_CHECKED : BST_UNCHECKED));
	::CheckDlgButton(m_hWndGeneralDlg, IDC_HACKITONTOP, (m_Settings.m_AlwaysOnTop ? BST_CHECKED : BST_UNCHECKED));

	if (MySetLayeredWindowAttributes) {
		::CheckDlgButton(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT, (m_Settings.m_Transparent ? BST_CHECKED : BST_UNCHECKED));
		SetDlgItemInt(m_hWndWindowDlg, IDC_HACKITTRANSPARENT_PERCENT, m_Settings.m_TransparentPercent, FALSE);

		HWND m_hWndTransparentPercent = GetDlgItem(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT_PERCENT);
		HWND m_hWndTransparentSpin = GetDlgItem(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT_SPIN);

		SendMessage(m_hWndTransparentSpin, UDM_SETRANGE, 0, MAKELONG(100, 0));
		SendMessage(m_hWndTransparentSpin, UDM_SETPOS, 0, MAKELONG(m_Settings.m_TransparentPercent, 0));

		if (!m_Settings.m_Transparent) {
			EnableWindow(m_hWndTransparentPercent, FALSE);
			EnableWindow(m_hWndTransparentSpin, FALSE);
		}
	} else {
		HWND transparent = GetDlgItem(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT);
		HWND m_hWndTransparentPercent = GetDlgItem(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT_PERCENT);
		HWND m_hWndTransparentSpin = GetDlgItem(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT_SPIN);

		EnableWindow(transparent, FALSE);
		EnableWindow(m_hWndTransparentPercent, FALSE);
		EnableWindow(m_hWndTransparentSpin, FALSE);
		EnableWindow(GetDlgItem(m_hWndGeneralDlg, IDC_TRANSPARENCYPERCENTSTATIC), FALSE);
	}

	UINT priorityID;
	
	switch (m_Settings.m_Priority) {
	case REALTIME_PRIORITY_CLASS:
		priorityID = IDC_HIGHESTPRIORITY;
		break;
	case HIGH_PRIORITY_CLASS:
		priorityID = IDC_HIGHPRIORITY;
		break;
	case NORMAL_PRIORITY_CLASS:
		priorityID = IDC_NORMALPRIORITY;
		break;
	case IDLE_PRIORITY_CLASS:
		priorityID = IDC_LOWPRIORITY;
		break;
	}

	::CheckDlgButton(m_hWndGeneralDlg, priorityID, BST_CHECKED);
}

void SettingsDlg::InitIllegalDlg() {
	IllegalWindowSpec* pIllWinSpec ;
	IllegalWindowSpec_set::iterator current = m_Settings.m_IllegalWindowSpecList.begin() ;

	m_IllegalWindowList = GetDlgItem(m_hWnd, IDC_ILLEGALWINDOWLIST);

	LVCOLUMN column;

	column.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt = LVCFMT_LEFT;
	column.cx = 50;
	column.pszText = "Name";
	ListView_InsertColumn(m_IllegalWindowList, 0, &column);
	column.cx = 80;
	column.pszText = "Title";
	ListView_InsertColumn(m_IllegalWindowList, 1, &column);
	column.pszText = "Class";
	ListView_InsertColumn(m_IllegalWindowList, 2, &column);
	column.pszText = "Filename";
	ListView_InsertColumn(m_IllegalWindowList, 3, &column);

	LVITEM item;

	int count = 0;

	while (current != m_Settings.m_IllegalWindowSpecList.end()) {
		pIllWinSpec = *current++;

		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.iItem = count;
		item.iSubItem = 0;
		item.lParam = (LPARAM)pIllWinSpec;
		item.pszText = (char*)pIllWinSpec->GetName().c_str();
		ListView_InsertItem(m_IllegalWindowList, &item);

		item.mask = LVIF_TEXT;
		item.pszText = (char*)pIllWinSpec->GetTitle().c_str();
		item.iSubItem = 1;
		ListView_SetItem(m_IllegalWindowList, &item);

		item.pszText = (char*)pIllWinSpec->GetClass().c_str();
		item.iSubItem = 2;
		ListView_SetItem(m_IllegalWindowList, &item);

		item.pszText = (char*)pIllWinSpec->GetFilename().c_str();
		item.iSubItem = 3;
		ListView_SetItem(m_IllegalWindowList, &item);

		count++;
	}
}

void SettingsDlg::InitFunctionDlg() {
	AutoFunction *af;
	string_AutoFunction_map::iterator pos;
	int index;
	pos = m_Settings.m_AutoFunctions.begin();

	while (pos != m_Settings.m_AutoFunctions.end()) {
		af = pos->second;

		index = SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_ADDSTRING, 0, (UINT)pos->first.c_str());
		SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_SETITEMDATA, index, (UINT)pos->second);
		pos++;;
	}
}

BOOL SettingsDlg::OnCommand(WPARAM wParam, LPARAM lParam) {

	switch (HIWORD(wParam)) {	
	case EN_UPDATE:
		switch (LOWORD(wParam)) {
		case IDC_HACKITTRANSPARENT_PERCENT:
			m_Settings.m_TransparentPercent = GetDlgItemInt(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT_PERCENT, NULL, FALSE);
			break;
		}
		break;
	case BN_CLICKED:	
		switch (LOWORD(wParam)) {
		case IDC_CHILDREN:
			if (m_Settings.m_ShowChilds) {
				m_Settings.m_ShowChilds = FALSE;
			} else {
				m_Settings.m_ShowChilds = TRUE;
			}
			break;
		case IDC_SHOWSPLASH:
			if (m_Settings.m_ShowSplash) {
				m_Settings.m_ShowSplash = FALSE;
			} else {
				m_Settings.m_ShowSplash = TRUE;
			}
			break;
		case IDC_AUTOFUNCTIONSENABLED:
			if (m_Settings.m_AutoFunctionsEnabled) {
				m_Settings.m_AutoFunctionsEnabled = FALSE;
			} else {
				m_Settings.m_AutoFunctionsEnabled = TRUE;
			}
			break;
		case IDC_CHILDFUNCTIONSENABLED:
			if (m_Settings.m_ChildFunctionsEnabled) {
				m_Settings.m_ChildFunctionsEnabled = FALSE;
			} else {
				m_Settings.m_ChildFunctionsEnabled = TRUE;
			}
			break;
		case IDC_HIDEONSTART:
			if (m_Settings.m_HideOnStart) {
				m_Settings.m_HideOnStart = FALSE;
			} else {
				m_Settings.m_HideOnStart = TRUE;
			}
			break;
		case IDC_HIGHESTPRIORITY:
			m_Settings.m_Priority = REALTIME_PRIORITY_CLASS;
			//SetPriorityClass(m_hWnd, m_Settings.m_Priority);
			break;
		case IDC_HIGHPRIORITY:
			m_Settings.m_Priority = HIGH_PRIORITY_CLASS;
			//SetPriorityClass(m_hWnd, m_Settings.m_Priority);
			break;
		case IDC_NORMALPRIORITY:
			m_Settings.m_Priority = NORMAL_PRIORITY_CLASS;
			//SetPriorityClass(m_hWnd, m_Settings.m_Priority);
			break;
		case IDC_LOWPRIORITY:
			m_Settings.m_Priority = IDLE_PRIORITY_CLASS;
			//SetPriorityClass(m_hWnd, m_Settings.m_Priority);
			break;
		case IDC_FONT:
			SelectFont();
			break;
		case IDC_HACKITONTOP:
			if (m_Settings.m_AlwaysOnTop) {
				m_Settings.m_AlwaysOnTop = FALSE;
				//::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			} else {
				m_Settings.m_AlwaysOnTop = TRUE;
				//::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			}
			break;
		case IDC_HACKITTRANSPARENT:
			if (MySetLayeredWindowAttributes) {
				if (m_Settings.m_Transparent) {
					m_Settings.m_Transparent = FALSE;
					EnableWindow(GetDlgItem(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT_PERCENT), FALSE);
					EnableWindow(GetDlgItem(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT_SPIN), FALSE);
					//::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				} else {
					m_Settings.m_Transparent = TRUE;
					EnableWindow(GetDlgItem(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT_PERCENT), TRUE);
					EnableWindow(GetDlgItem(m_hWndGeneralDlg, IDC_HACKITTRANSPARENT_SPIN), TRUE);
					//::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				}
			}
			break;
		}
		return TRUE;
		break;
	}
	return FALSE;
}

int SettingsDlg::DoModal() {
	HPROPSHEETPAGE hpsp[4];
	int retVal;

	m_PropSheetHeader.dwSize = sizeof(m_PropSheetHeader);
	m_PropSheetHeader.dwFlags = PSH_NOAPPLYNOW;
	m_PropSheetHeader.hwndParent = m_hWndParent;
	m_PropSheetHeader.hInstance = m_hInstance;
	m_PropSheetHeader.pszCaption = "Hack-It Settings";
	m_PropSheetHeader.nPages = 4;
	m_PropSheetHeader.nStartPage = 0;
	m_PropSheetHeader.pStartPage = "Window List";
	m_PropSheetHeader.phpage = hpsp;
	m_PropSheetHeader.phpage[0] = m_GeneralSettingsPage;
	m_PropSheetHeader.phpage[1] = m_WindowSettingsPage;
	m_PropSheetHeader.phpage[2] = m_IllegalSettingsPage;
	m_PropSheetHeader.phpage[3] = m_FunctionSettingsPage;
	retVal = PropertySheet(&m_PropSheetHeader);

	retVal = IDOK;

	return retVal;
}

void SettingsDlg::Initialize(HackItSettings& settings)
{
	m_Settings = settings;
	m_OldSettings = settings;
}


void SettingsDlg::AddEditFunction(char *name, AutoFunction *function)
{
	int index;
	string newString(name);
	string_AutoFunction_map::iterator position;

	if ((index = SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_FINDSTRINGEXACT, -1, (UINT)(LPCTSTR)name)) != LB_ERR) {
		position = m_Settings.m_AutoFunctions.find(newString);
		if (position != m_Settings.m_AutoFunctions.end()) {
			*m_Settings.m_AutoFunctions[newString] = *function;
			delete function;
		}
	} else {
		m_Settings.m_AutoFunctions[newString] = function;
		index = SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_ADDSTRING, 0, (UINT)LPCTSTR(name));
		SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_SETITEMDATA, index, (UINT)function);
	}
	SendDlgItemMessage(m_hWndFunctionDlg, IDC_AUTOENTRIES, LB_SETCURSEL, index, 0);
}

LRESULT SettingsDlg::OnWindowDlgCommand(WPARAM wParam, LPARAM lParam)
{
	char buffer[STRING_BUFFER_SIZE];
	bool changed = false;

	switch (HIWORD(wParam)) {
	case EN_UPDATE:
		switch (LOWORD(wParam)) {
		case IDC_NAMELESSTEXT:
			GetDlgItemText(m_hWndWindowDlg, IDC_NAMELESSTEXT, buffer, STRING_BUFFER_SIZE);
			m_Settings.m_NamelessText = buffer;
			break;
		case IDC_CONSOLEPOLLINGTIME:
			m_Settings.m_ConsolePollingTime = GetDlgItemInt(m_hWndWindowDlg, IDC_CONSOLEPOLLINGTIME, NULL, FALSE);
			break;
		case IDC_WINDOWLISTCLEANERTIME:
			m_Settings.m_WindowListCleanerTime = GetDlgItemInt(m_hWndWindowDlg, IDC_WINDOWLISTCLEANERTIME, NULL, FALSE);
			break;
		}
		break;
	case BN_CLICKED:	
		changed = false;

		switch (LOWORD(wParam)) {
		case IDC_USELARGEICONS:
			if (m_Settings.m_UseLargeIcons) {
				m_Settings.m_UseLargeIcons = FALSE;
			} else {
				m_Settings.m_UseLargeIcons = TRUE;
			}
			break;
		case IDC_WINDOWICONFIRST:
			if (m_Settings.m_WindowIconFirst) {
				m_Settings.m_WindowIconFirst = FALSE;
			} else {
				m_Settings.m_WindowIconFirst = TRUE;
			}
			break;
		case IDC_REFRESHTEXT:
			if (m_Settings.m_UpdateItems) {
				m_Settings.m_UpdateItems = FALSE;
			} else {
				m_Settings.m_UpdateItems = TRUE;
			}
			break;
		case IDC_CONSOLEPOLL:
			if (m_Settings.m_ConsolePolling) {
				m_Settings.m_ConsolePolling = FALSE;
			} else {
				m_Settings.m_ConsolePolling = TRUE;
			}
			break;
		case IDC_CLEANLIST:
			if (m_Settings.m_WindowListCleaning) {
				m_Settings.m_WindowListCleaning = FALSE;
			} else {
				m_Settings.m_WindowListCleaning = TRUE;
			}
			break;
		case IDC_SYSMENUCHECK:
			if (m_Settings.m_SysMenuCapture) {
				m_Settings.m_SysMenuCapture = FALSE;
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUVISIBLE), FALSE);
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTOPPED), FALSE);
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTRAYYED), FALSE);
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUPRIORITY), FALSE);
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENU_APPEND), FALSE);
			} else {
				m_Settings.m_SysMenuCapture = TRUE;
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUVISIBLE), TRUE);
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTOPPED), TRUE);
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUTRAYYED), TRUE);
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENUPRIORITY), TRUE);
				::EnableWindow(::GetDlgItem(m_hWndWindowDlg, IDC_SYSMENU_APPEND), TRUE);
			}
			break;
		case IDC_SYSMENU_APPEND:
			if (m_Settings.m_SysMenuAppend) {
				m_Settings.m_SysMenuAppend = FALSE;
			} else {
				m_Settings.m_SysMenuAppend = TRUE;
			}
			break;
		case IDC_SYSMENUVISIBLE:
			if (m_Settings.m_SysMenuVisible) {
				m_Settings.m_SysMenuVisible = FALSE;
			} else {
				m_Settings.m_SysMenuVisible = TRUE;
			}
			break;
		case IDC_SYSMENUTOPPED:
			if (m_Settings.m_SysMenuTopped) {
				m_Settings.m_SysMenuTopped = FALSE;
			} else {
				m_Settings.m_SysMenuTopped = TRUE;
			}
			break;
		case IDC_SYSMENUTRAYYED:
			if (m_Settings.m_SysMenuTrayyed) {
				m_Settings.m_SysMenuTrayyed = FALSE;
			} else {
				m_Settings.m_SysMenuTrayyed = TRUE;
			}
			break;
		case IDC_SYSMENUPRIORITY:
			if (m_Settings.m_SysMenuPriority) {
				m_Settings.m_SysMenuPriority = FALSE;
			} else {
				m_Settings.m_SysMenuPriority = TRUE;
			}
			break;
		case IDC_SYSMENUTRANSPARENCY:
			if (MySetLayeredWindowAttributes) {
				if (m_Settings.m_SysMenuTransparency) {
					m_Settings.m_SysMenuTransparency = FALSE;
				} else {
					m_Settings.m_SysMenuTransparency = TRUE;
				}
			}
			break;
		case IDC_TITLELESS:
			if (m_Settings.m_LowestShowable == '\0') {
				m_Settings.m_LowestShowable = '!';
			} else {
				m_Settings.m_LowestShowable = '\0';
			}
			break;
		case IDC_TOPMOSTCHILDREN:
			m_Settings.m_ChildrenInTopmostList = !m_Settings.m_ChildrenInTopmostList;
			break;
		case IDC_UPDATEADDREMOVE:
			m_Settings.m_UpdateAddRemove = !m_Settings.m_UpdateAddRemove;
			break;
		case IDC_UPDATEAUTO:
			m_Settings.m_UpdateAuto = !m_Settings.m_UpdateAuto;
			break;
		default:
			changed = false;
			break;
		}
		break;
	}

	if (changed) {
		PropSheet_Changed(m_hWnd, m_hWndWindowDlg);
	}

	return 0;
}

void SettingsDlg::SelectFont()
{
	CHOOSEFONT chooseFont;
	ZeroMemory(&chooseFont, sizeof(CHOOSEFONT));
	chooseFont.lpLogFont = &(m_Settings.m_LogFont);
	chooseFont.lStructSize = sizeof(chooseFont);
	chooseFont.Flags = CF_INITTOLOGFONTSTRUCT | CF_EFFECTS | CF_SCREENFONTS;
	ChooseFont(&chooseFont);
}

