// Splash.cpp : implementation file
//

#include <windows.h>
#include "Splash.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// SplashDlg dialog


SplashDlg::SplashDlg(HWND hWndParent, HINSTANCE hInstance)
{
	m_hWndParent = hWndParent;
	m_hInstance = hInstance;
}

void SplashDlg::OnTimer(UINT nIDEvent) 
{
	EndDialog(m_hWnd, IDOK);
}

BOOL SplashDlg::OnInitDialog() 
{
	SetTimer(m_hWnd, 1, 2000, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int SplashDlg::DoModal()
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_SPLASH), m_hWndParent, DialogProc,	(LPARAM)this);
}

BOOL CALLBACK SplashDlg::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	SplashDlg* theDlg = (SplashDlg*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMsg) {
	case WM_INITDIALOG:
		theDlg = (SplashDlg*)lParam;
		theDlg->m_hWnd = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)theDlg);
		theDlg->OnInitDialog();
		return TRUE;
	case WM_TIMER:
		theDlg->OnTimer(wParam);
		return TRUE;
	}

	return FALSE;
}
