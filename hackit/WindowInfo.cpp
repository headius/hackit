#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"
#include "defines.h"
#include "WindowInfo.h"

void GetWindowFileName(HWND, char*, int);

WindowInfoDlg::WindowInfoDlg(HWND hWndParent, HINSTANCE hInstance) {
	m_hWndParent = hWndParent;
	m_hInstance = hInstance;
}

BOOL CALLBACK WindowInfoDlg::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WindowInfoDlg* theDlg = (WindowInfoDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (WindowInfoDlg*)lParam;
		theDlg->m_hWnd = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
		theDlg->OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case BN_CLICKED:
			switch (LOWORD(wParam)) {
			case IDOK:
			case IDCANCEL:
				theDlg->OnOK();
				return TRUE;
				break;
			}
		}
	}

	return FALSE;
}

void WindowInfoDlg::init(HWND window) {
	targetWindow = window;
}

int WindowInfoDlg::DoModal()
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_WINDOWINFO), m_hWndParent, DialogProc, (LPARAM)this);
}

void WindowInfoDlg::OnOK()
{
	EndDialog(m_hWnd, IDOK);
}

void WindowInfoDlg::OnInitDialog() {
	char buffer[STRING_BUFFER_SIZE];
	HINSTANCE process;
	WINDOWPLACEMENT placement;
	HINSTANCE process2;

	placement.length = sizeof(WINDOWPLACEMENT);

	GetWindowThreadProcessId(targetWindow, (LPDWORD)&process);
	process2 = (HINSTANCE)OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, (DWORD)process);

	sprintf(buffer, "0x%08x", targetWindow);
	SetDlgItemText(m_hWnd, IDC_WINDOW_HANDLE, buffer);
	GetWindowText(targetWindow, buffer, STRING_BUFFER_SIZE);
	SetDlgItemText(m_hWnd, IDC_WINDOW_TITLE, buffer);
	GetClassName(targetWindow, buffer, STRING_BUFFER_SIZE);
	SetDlgItemText(m_hWnd, IDC_WINDOW_CLASS, buffer);

	GetWindowFileName(targetWindow, buffer, STRING_BUFFER_SIZE);

	SetDlgItemText(m_hWnd, IDC_WINDOW_EXECUTABLE, buffer);
	sprintf(buffer, "0x%08x", process);
	SetDlgItemText(m_hWnd, IDC_WINDOW_PROCESSID, buffer);
	switch (GetPriorityClass(process2)) {
	case REALTIME_PRIORITY_CLASS:
		strcpy(buffer, "Real-Time");
		break;
	case HIGH_PRIORITY_CLASS:
		strcpy(buffer, "High");
		break;
	case NORMAL_PRIORITY_CLASS:
		strcpy(buffer, "Normal");
		break;
	case IDLE_PRIORITY_CLASS:
		strcpy(buffer, "Idle");
		break;
	default:
		strcpy(buffer, "No priority.");
		break;
	}
	SetDlgItemText(m_hWnd, IDC_WINDOW_PROCESSPRIORITY, buffer);
	GetWindowPlacement(targetWindow, &placement);
	sprintf(buffer, "(%d , %d)", placement.rcNormalPosition.top, placement.rcNormalPosition.left);
	SetDlgItemText(m_hWnd, IDC_WINDOW_RESTOREDPOSITION, buffer);
	sprintf(buffer, "(%d x %d)", placement.rcNormalPosition.bottom - placement.rcNormalPosition.top, placement.rcNormalPosition.right - placement.rcNormalPosition.left);
	SetDlgItemText(m_hWnd, IDC_WINDOW_RESTOREDSIZE, buffer);

	CloseHandle(process2);
}