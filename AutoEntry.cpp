// AutoEntry.cpp : implementation file
//
#pragma warning(disable: 4768)

#include <windows.h>
#include <objbase.h>
#include <commctrl.h>
#include <winuser.h>
#include "AutoEntry.h"
#include "AutoFunction.h"
#include "HackItSettings.h"
#include "SettingsDlg.h"
#include "defines.h"
#include "resource.h"

extern int (__stdcall *MySetLayeredWindowAttributes)(struct HWND__ *,unsigned long,unsigned char,unsigned long);

/////////////////////////////////////////////////////////////////////////////
// AutoEntryDlg dialog

void GetWindowFileName(struct HWND__ *,char *,int);

AutoEntryDlg::AutoEntryDlg(HWND hWndParent, HINSTANCE hInstance)
{
	m_hWndParent = hWndParent;
	m_hInstance = hInstance;
}

void AutoEntryDlg::Initialize(SettingsDlg *pdlg, int function)
{
	m_pSettingsDlg = pdlg;
	m_Function = function;
}

void AutoEntryDlg::OnOK() 
{
	AutoFunction *function;
	char filename[STRING_BUFFER_SIZE], winclass[STRING_BUFFER_SIZE], title[STRING_BUFFER_SIZE];
	char text[STRING_BUFFER_SIZE];
	char name[STRING_BUFFER_SIZE];
	int result;

	GetDlgItemText(m_hWnd, IDC_ENTRYNAME, name, STRING_BUFFER_SIZE);

	if (strlen(name) == 0) {
		MessageBox(m_hWnd, "You did not enter anything for 'Name'", "Name Field Required", MB_OK);
		return;
	}

	function = new AutoFunction();

	GetDlgItemText(m_hWnd, IDC_CLASSCOMBO, winclass, STRING_BUFFER_SIZE);
	function->setWinClass(winclass);
	
	GetDlgItemText(m_hWnd, IDC_TITLECOMBO, title, STRING_BUFFER_SIZE);
	function->setTitle(title);

	GetDlgItemText(m_hWnd, IDC_FILENAMECOMBO, filename, STRING_BUFFER_SIZE);
	function->setFilename(filename);

	if (strlen(winclass) == 0 && strlen(filename) == 0 && strlen(title) == 0) {
		MessageBox(m_hWnd, "You have not entered any criteria to match. If you want this specification to match all windows,\nyou should enter a '*' character in at least one field.",
			"Global Function Warning",
			MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	GetDlgItemText(m_hWnd, IDC_SPECIALTEXT, text, STRING_BUFFER_SIZE);
	function->setSpecialData(text);

	GetDlgItemText(m_hWnd, IDC_ALTERNATETEXT, text, STRING_BUFFER_SIZE);
	function->setAlternateData(text);

	function->setResize(IsDlgButtonChecked(m_hWnd, IDC_RESIZECHECK));
	function->setResizeH(GetDlgItemInt(m_hWnd, IDC_RESIZEHEIGHT, NULL, FALSE));
	function->setResizeW(GetDlgItemInt(m_hWnd, IDC_RESIZEWIDTH, NULL, FALSE));
	function->setMove(IsDlgButtonChecked(m_hWnd, IDC_MOVECHECK));
	function->setMoveX(GetDlgItemInt(m_hWnd, IDC_MOVEX, NULL, FALSE));
	function->setMoveY(GetDlgItemInt(m_hWnd, IDC_MOVEY, NULL, FALSE));

	function->setWindowAction(SendDlgItemMessage(m_hWnd, IDC_WINDOWCOMBO, CB_GETCURSEL, 0, 0));
	function->setTopmostAction(SendDlgItemMessage(m_hWnd, IDC_TOPMOSTCOMBO, CB_GETCURSEL, 0, 0));
	function->setVisibilityAction(SendDlgItemMessage(m_hWnd, IDC_VISIBILITYCOMBO, CB_GETCURSEL, 0, 0));
	if (MySetLayeredWindowAttributes) {
		function->setTransparency(SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_GETCURSEL, 0, 0));
	}
	function->setPriorityAction(SendDlgItemMessage(m_hWnd, IDC_PRIORITYCOMBO, CB_GETCURSEL, 0, 0));
	function->setDestructionAction(SendDlgItemMessage(m_hWnd, IDC_DESTRUCTIONCOMBO, CB_GETCURSEL, 0, 0));
	function->setSpecialAction(SendDlgItemMessage(m_hWnd, IDC_SPECIALCOMBO, CB_GETCURSEL, 0, 0));
	function->setAlternateTarget(SendDlgItemMessage(m_hWnd, IDC_ALTERNATECOMBO, CB_GETCURSEL, 0, 0));
	function->setTrayIcon(IsDlgButtonChecked(m_hWnd, IDC_TRAYCHECK));
	function->setTopOnly(IsDlgButtonChecked(m_hWnd, IDC_TOPCHECK));
	function->setCaseSensitive(IsDlgButtonChecked(m_hWnd, IDC_CASESENSITIVE));

	result = SendDlgItemMessage(m_hWndParent, IDC_AUTOENTRIES, LB_FINDSTRINGEXACT, -1, (LPARAM)name);
	if (result == LB_ERR ||
		m_Function >= 0) {
		m_pSettingsDlg->AddEditFunction(name, function);
		EndDialog(m_hWnd, IDOK);
	} else {
		MessageBox(m_hWnd, "Your 'Name' field is not unique.", "Unique Name", MB_OK);
	}
}

BOOL AutoEntryDlg::OnInitDialog(WPARAM wParam, LPARAM lParam) 
{
	SendDlgItemMessage(m_hWnd, IDC_WINDOWCOMBO, CB_ADDSTRING, 0, (LONG)"No Change");
	SendDlgItemMessage(m_hWnd, IDC_WINDOWCOMBO, CB_ADDSTRING, 0, (LONG)"Restore");
	SendDlgItemMessage(m_hWnd, IDC_WINDOWCOMBO, CB_ADDSTRING, 0, (LONG)"Minimize");
	SendDlgItemMessage(m_hWnd, IDC_WINDOWCOMBO, CB_ADDSTRING, 0, (LONG)"Maximize");
	SendDlgItemMessage(m_hWnd, IDC_WINDOWCOMBO, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(m_hWnd, IDC_TOPMOSTCOMBO, CB_ADDSTRING, 0, (LONG)"No Change");
	SendDlgItemMessage(m_hWnd, IDC_TOPMOSTCOMBO, CB_ADDSTRING, 0, (LONG)"Stays on Top");
	SendDlgItemMessage(m_hWnd, IDC_TOPMOSTCOMBO, CB_ADDSTRING, 0, (LONG)"Doesn't Stay on Top");
	SendDlgItemMessage(m_hWnd, IDC_TOPMOSTCOMBO, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(m_hWnd, IDC_VISIBILITYCOMBO, CB_ADDSTRING, 0, (LONG)"No Change");
	SendDlgItemMessage(m_hWnd, IDC_VISIBILITYCOMBO, CB_ADDSTRING, 0, (LONG)"Visible");
	SendDlgItemMessage(m_hWnd, IDC_VISIBILITYCOMBO, CB_ADDSTRING, 0, (LONG)"Hidden");
	SendDlgItemMessage(m_hWnd, IDC_VISIBILITYCOMBO, CB_SETCURSEL, 0, 0);
	if (MySetLayeredWindowAttributes) {
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"None");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"10%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"20%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"30%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"40%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"50%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"60%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"70%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"80%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"90%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_ADDSTRING, 0, (LONG)"100%");
		SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_SETCURSEL, 0, 0);
	} else {
		EnableWindow(GetDlgItem(m_hWnd, IDC_TRANSPARENCYCOMBO), FALSE);
		EnableWindow(GetDlgItem(m_hWnd, IDC_TRANSPARENCYSTATIC), FALSE);
	}
	SendDlgItemMessage(m_hWnd, IDC_PRIORITYCOMBO, CB_ADDSTRING, 0, (LONG)"No Change");
	SendDlgItemMessage(m_hWnd, IDC_PRIORITYCOMBO, CB_ADDSTRING, 0, (LONG)"Idle");
	SendDlgItemMessage(m_hWnd, IDC_PRIORITYCOMBO, CB_ADDSTRING, 0, (LONG)"Normal");
	SendDlgItemMessage(m_hWnd, IDC_PRIORITYCOMBO, CB_ADDSTRING, 0, (LONG)"High");
	SendDlgItemMessage(m_hWnd, IDC_PRIORITYCOMBO, CB_ADDSTRING, 0, (LONG)"Real-Time");
	SendDlgItemMessage(m_hWnd, IDC_PRIORITYCOMBO, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(m_hWnd, IDC_DESTRUCTIONCOMBO, CB_ADDSTRING, 0, (LONG)"Don't Destroy");
	//SendDlgItemMessage(m_hWnd, IDC_DESTRUCTIONCOMBO, CB_ADDSTRING, 0, (LONG)"Prevent Creation");
	SendDlgItemMessage(m_hWnd, IDC_DESTRUCTIONCOMBO, CB_ADDSTRING, 0, (LONG)"Close Window");
	SendDlgItemMessage(m_hWnd, IDC_DESTRUCTIONCOMBO, CB_ADDSTRING, 0, (LONG)"End Task");
	SendDlgItemMessage(m_hWnd, IDC_DESTRUCTIONCOMBO, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(m_hWnd, IDC_SPECIALCOMBO, CB_ADDSTRING, 0, (LONG)"Nothing");
	SendDlgItemMessage(m_hWnd, IDC_SPECIALCOMBO, CB_ADDSTRING, 0, (LONG)"Press Button");
	//SendDlgItemMessage(m_hWnd, IDC_SPECIALCOMBO, CB_ADDSTRING, 0, (LONG)"Enter Text");
	SendDlgItemMessage(m_hWnd, IDC_SPECIALCOMBO, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(m_hWnd, IDC_ALTERNATECOMBO, CB_ADDSTRING, 0, (LONG)"None");
	SendDlgItemMessage(m_hWnd, IDC_ALTERNATECOMBO, CB_ADDSTRING, 0, (LONG)"Act on Child");
	SendDlgItemMessage(m_hWnd, IDC_ALTERNATECOMBO, CB_ADDSTRING, 0, (LONG)"Act on Parent");
	SendDlgItemMessage(m_hWnd, IDC_ALTERNATECOMBO, CB_SETCURSEL, 0, 0);
	CheckDlgButton(m_hWnd, IDC_CASESENSITIVE, FALSE);
	CheckDlgButton(m_hWnd, IDC_TOPCHECK, TRUE);

	if (m_Function >= 0) {
		AutoFunction *af;
		char astring[STRING_BUFFER_SIZE];

		SendDlgItemMessage(m_hWndParent, IDC_AUTOENTRIES, LB_GETTEXT, m_Function, (LPARAM)astring);
		SetDlgItemText(m_hWnd, IDC_ENTRYNAME, astring);
		EnableWindow(GetDlgItem(m_hWnd, IDC_ENTRYNAME), FALSE);
		af = (AutoFunction*)SendDlgItemMessage(m_hWndParent, IDC_AUTOENTRIES, LB_GETITEMDATA, m_Function, 0);

		SetDlgItemText(m_hWnd, IDC_TITLECOMBO, af->getTitle().c_str());
		SetDlgItemText(m_hWnd, IDC_CLASSCOMBO, af->getWinClass().c_str());
		SetDlgItemText(m_hWnd, IDC_FILENAMECOMBO, af->getFilename().c_str());
		SetDlgItemText(m_hWnd, IDC_SPECIALTEXT, af->getSpecialData().c_str());
		SetDlgItemText(m_hWnd, IDC_ALTERNATETEXT, af->getAlternateData().c_str());
		SetDlgItemInt(m_hWnd, IDC_RESIZEHEIGHT, af->getResizeH(), FALSE);
		SetDlgItemInt(m_hWnd, IDC_RESIZEWIDTH, af->getResizeW(), FALSE);
		SetDlgItemInt(m_hWnd, IDC_MOVEX, af->getMoveX(), FALSE);
		SetDlgItemInt(m_hWnd, IDC_MOVEY, af->getMoveY(), FALSE);

		SendDlgItemMessage(m_hWnd, IDC_WINDOWCOMBO, CB_SETCURSEL, af->getWindowAction(), 0);
		SendDlgItemMessage(m_hWnd, IDC_TOPMOSTCOMBO, CB_SETCURSEL, af->getTopmostAction(), 0);
		SendDlgItemMessage(m_hWnd, IDC_VISIBILITYCOMBO, CB_SETCURSEL, af->getVisibilityAction(), 0);
		if (MySetLayeredWindowAttributes) {
			SendDlgItemMessage(m_hWnd, IDC_TRANSPARENCYCOMBO, CB_SETCURSEL, af->getTransparency(), 0);
		} else {
			EnableWindow(GetDlgItem(m_hWnd, IDC_TRANSPARENCYCOMBO), FALSE);
			EnableWindow(GetDlgItem(m_hWnd, IDC_TRANSPARENCYSTATIC), FALSE);
		}
		SendDlgItemMessage(m_hWnd, IDC_PRIORITYCOMBO, CB_SETCURSEL, af->getPriorityAction(), 0);
		SendDlgItemMessage(m_hWnd, IDC_DESTRUCTIONCOMBO, CB_SETCURSEL, af->getDestructionAction(), 0);
		SendDlgItemMessage(m_hWnd, IDC_SPECIALCOMBO, CB_SETCURSEL, af->getSpecialAction(), 0);
		SendDlgItemMessage(m_hWnd, IDC_ALTERNATECOMBO, CB_SETCURSEL, af->getAlternateTarget(), 0);

		CheckDlgButton(m_hWnd, IDC_RESIZECHECK, af->getResize());
		CheckDlgButton(m_hWnd, IDC_MOVECHECK, af->getMove());
		CheckDlgButton(m_hWnd, IDC_TRAYCHECK, af->getTrayIcon());
		CheckDlgButton(m_hWnd, IDC_TOPCHECK, af->getTopOnly());
		CheckDlgButton(m_hWnd, IDC_CASESENSITIVE, af->getCaseSensitive());
	}

	HWND window;
	char buffer[256];

	window = GetWindow(GetDesktopWindow(), GW_CHILD);

	while (window != NULL) {
		GetWindowText(window, buffer, 256);
		if (buffer[0] >= '!' && SendDlgItemMessage(m_hWnd, IDC_TITLECOMBO, CB_FINDSTRINGEXACT, 0, (LPARAM)buffer) == CB_ERR) {
			SendDlgItemMessage(m_hWnd, IDC_TITLECOMBO, CB_ADDSTRING, 0, (LONG)buffer);
		}

		GetWindowFileName(window, buffer, 256);
		if (buffer[0] >= '!' && SendDlgItemMessage(m_hWnd, IDC_FILENAMECOMBO, CB_FINDSTRINGEXACT, 0, (LPARAM)buffer) == CB_ERR) {
			SendDlgItemMessage(m_hWnd, IDC_FILENAMECOMBO, CB_ADDSTRING, 0, (LONG)buffer);
		}

		GetClassName(window, buffer, 256);
		if (buffer[0] >= '!' && SendDlgItemMessage(m_hWnd, IDC_CLASSCOMBO, CB_FINDSTRINGEXACT, 0, (LPARAM)buffer) == CB_ERR) {
			SendDlgItemMessage(m_hWnd, IDC_CLASSCOMBO, CB_ADDSTRING, 0, (LONG)buffer);
		}

		window = GetWindow(window, GW_HWNDNEXT);
	}

	return TRUE;
}

BOOL CALLBACK AutoEntryDlg::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AutoEntryDlg* theDlg = (AutoEntryDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (AutoEntryDlg*)lParam;
		theDlg->m_hWnd = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
		theDlg->OnInitDialog(wParam, lParam);
		return TRUE;
	case WM_COMMAND:
		return theDlg->OnCommand(wParam, lParam);
		break;
	}

	return FALSE;
}

BOOL AutoEntryDlg::OnCommand(WPARAM wParam, LPARAM lParam) {
	switch (HIWORD(wParam)) {	
	case BN_CLICKED:	
		switch (LOWORD(wParam)) {
		case IDOK:
			OnOK();
			return TRUE;
			break;
		case IDCANCEL:
			OnCancel();
			return TRUE;
			break;
		}
		break;
	}

	return FALSE;
}

int AutoEntryDlg::DoModal()
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_AUTOENTRY), m_hWndParent, DialogProc, (LPARAM)this);
}

void AutoEntryDlg::OnCancel()
{
	EndDialog(m_hWnd, IDCANCEL);
}
