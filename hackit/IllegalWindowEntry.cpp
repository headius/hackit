// IllegalWindowEntry.cpp : implementation file
//

#include <windows.h>
#include "resource.h"
#include "defines.h"
#include "IllegalWindowEntry.h"
#include "IllegalWindowSpec.h"

void GetWindowFileName(HWND window, char* file, int size);

/////////////////////////////////////////////////////////////////////////////
// IllegalWindowEntryDlg dialog


IllegalWindowEntryDlg::IllegalWindowEntryDlg(HWND hWndParent, HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_hWndParent = hWndParent;

    strcpy(m_IllegalName, "");
    strcpy(m_IllegalTitle, "");
    strcpy(m_IllegalClass, "");
    strcpy(m_IllegalFilename, "");
	m_IllegalCaseSensitive = false;
}

/////////////////////////////////////////////////////////////////////////////
// IllegalWindowEntryDlg message handlers

void IllegalWindowEntryDlg::OnOK() 
{
	m_IllegalCaseSensitive = IsDlgButtonChecked(m_hWnd, IDC_ILLEGALCASESENSITIVE) == BST_CHECKED;
	GetDlgItemText(m_hWnd, IDC_ILLEGALNAME, m_IllegalName, STRING_BUFFER_SIZE);
	GetDlgItemText(m_hWnd, IDC_ILLEGALTITLECOMBO, m_IllegalTitle, STRING_BUFFER_SIZE);
	GetDlgItemText(m_hWnd, IDC_ILLEGALCLASSCOMBO, m_IllegalClass, STRING_BUFFER_SIZE);
	GetDlgItemText(m_hWnd, IDC_ILLEGALFILENAMECOMBO, m_IllegalFilename, STRING_BUFFER_SIZE);

	if (strlen(m_IllegalName) == 0) {
		MessageBox(m_hWnd, "You must enter a unique name for this specification",
			"Missing Name",
			MB_OK | MB_ICONEXCLAMATION);

		return;
	}

	if (strlen(m_IllegalTitle) == 0 && strlen(m_IllegalClass) == 0 && strlen(m_IllegalFilename) == 0) {
		MessageBox(m_hWnd, "You have not entered any criteria to match. If you want this specification to match all windows,\nyou should enter a '*' character in at least one field.",
			"Global Function Warning",
			MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	EndDialog(m_hWnd, IDOK);
}

void IllegalWindowEntryDlg::OnCancel()
{
	EndDialog(m_hWnd, IDCANCEL);
}

void IllegalWindowEntryDlg::Initialize(IllegalWindowSpec *pSpec)
{
	m_IllegalCaseSensitive = pSpec->GetCaseSensitive();
	strncpy(m_IllegalName, pSpec->GetName().c_str(), STRING_BUFFER_SIZE);
	strncpy(m_IllegalTitle, pSpec->GetTitle().c_str(), STRING_BUFFER_SIZE);
	strncpy(m_IllegalClass, pSpec->GetClass().c_str(), STRING_BUFFER_SIZE);
	strncpy(m_IllegalFilename, pSpec->GetFilename().c_str(), STRING_BUFFER_SIZE);
}

int IllegalWindowEntryDlg::DoModal()
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_ILLEGALWINDOWENTRY), m_hWndParent, DialogProc, (LPARAM)this);
}

BOOL CALLBACK IllegalWindowEntryDlg::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IllegalWindowEntryDlg* theDlg = (IllegalWindowEntryDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (IllegalWindowEntryDlg*)lParam;
		theDlg->m_hWnd = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
		theDlg->OnInitDialog();
		return true;
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case BN_CLICKED:
			switch (LOWORD(wParam)) {
			case IDOK:
				theDlg->OnOK();
				return TRUE;
				break;
			case IDCANCEL:
				theDlg->OnCancel();
				return TRUE;
				break;
			}
			break;
		}
		break;
	}

	return FALSE;
}

void IllegalWindowEntryDlg::OnInitDialog()
{
	HWND window;
	char buffer[256];

	window = GetWindow(GetDesktopWindow(), GW_CHILD);

	while (window != NULL) {
		GetWindowText(window, buffer, 256);
		if (buffer[0] >= '!' && SendDlgItemMessage(m_hWnd, IDC_ILLEGALTITLECOMBO, CB_FINDSTRINGEXACT, 0, (LPARAM)buffer) == CB_ERR) {
			SendDlgItemMessage(m_hWnd, IDC_ILLEGALTITLECOMBO, CB_ADDSTRING, 0, (LONG)buffer);
		}

		GetWindowFileName(window, buffer, 256);
		if (buffer[0] >= '!' && SendDlgItemMessage(m_hWnd, IDC_ILLEGALFILENAMECOMBO, CB_FINDSTRINGEXACT, 0, (LPARAM)buffer) == CB_ERR) {
			SendDlgItemMessage(m_hWnd, IDC_ILLEGALFILENAMECOMBO, CB_ADDSTRING, 0, (LONG)buffer);
		}

		GetClassName(window, buffer, 256);
		if (buffer[0] >= '!' && SendDlgItemMessage(m_hWnd, IDC_ILLEGALCLASSCOMBO, CB_FINDSTRINGEXACT, 0, (LPARAM)buffer) == CB_ERR) {
			SendDlgItemMessage(m_hWnd, IDC_ILLEGALCLASSCOMBO, CB_ADDSTRING, 0, (LONG)buffer);
		}

		window = GetWindow(window, GW_HWNDNEXT);
	}

	SetDlgItemText(m_hWnd, IDC_ILLEGALNAME, m_IllegalName);
	SetDlgItemText(m_hWnd, IDC_ILLEGALTITLECOMBO, m_IllegalTitle);
	SetDlgItemText(m_hWnd, IDC_ILLEGALCLASSCOMBO, m_IllegalClass);
	SetDlgItemText(m_hWnd, IDC_ILLEGALFILENAMECOMBO, m_IllegalFilename);

	if (strlen(m_IllegalName) != 0) {
		EnableWindow(GetDlgItem(m_hWnd, IDC_ILLEGALNAME), FALSE);
	}

	if (m_IllegalCaseSensitive) {
		CheckDlgButton(m_hWnd, IDC_ILLEGALCASESENSITIVE, BST_CHECKED);
	}
}

char* IllegalWindowEntryDlg::GetIllegalName()
{
	return m_IllegalName;
}

char* IllegalWindowEntryDlg::GetIllegalTitle()
{
	return m_IllegalTitle;
}

char* IllegalWindowEntryDlg::GetIllegalClass()
{
	return m_IllegalClass;
}

char* IllegalWindowEntryDlg::GetIllegalFilename()
{
	return m_IllegalFilename;
}

bool IllegalWindowEntryDlg::GetIllegalCaseSensitive()
{
	return m_IllegalCaseSensitive;
}

