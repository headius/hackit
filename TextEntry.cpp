// TextEntry.cpp : implementation file
//

#include "defines.h"
#include "resource.h"
#include "TextEntry.h"

/////////////////////////////////////////////////////////////////////////////
// TextEntryDlg dialog


TextEntryDlg::TextEntryDlg(HWND hWndParent, HINSTANCE hInstance)
{
	m_hWndParent = hWndParent;
	m_hInstance = hInstance;
	strcpy(m_Text, "");
}

char *TextEntryDlg::GetText(char *buff, UINT size)
{
	return strncpy(buff, m_Text, size);
}

void TextEntryDlg::SetText(char *buff)
{
	strncpy(m_Text, buff, STRING_BUFFER_SIZE);
}

BOOL CALLBACK TextEntryDlg::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TextEntryDlg* theDlg = (TextEntryDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (TextEntryDlg*)lParam;
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

void TextEntryDlg::OnInitDialog()
{
	SetDlgItemText(m_hWnd, IDC_TEXTENTRY, m_Text);
}

void TextEntryDlg::OnOK() {
	EndDialog(m_hWnd, IDOK);
}

void TextEntryDlg::OnCancel() {
	EndDialog(m_hWnd, IDCANCEL);
}

int TextEntryDlg::DoModal()
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_TEXTENTRY), m_hWndParent, DialogProc, (LPARAM)this);
}
