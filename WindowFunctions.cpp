// WindowFunctions.cpp: implementation of the WindowFunctions class.
//
//////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include "defines.h"
#include "WindowFunctions.h"
#include "TextEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WindowFunctions::WindowFunctions()
{

}

WindowFunctions::~WindowFunctions()
{

}

void WindowFunctions::TopWindow(HWND window)
{
	::SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WindowFunctions::UnTopWindow(HWND window)
{
	::SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WindowFunctions::MaximizeWindow(HWND Window)
{
	WINDOWPLACEMENT WindPlace;
	
	WindPlace.length = sizeof(WindPlace);
	
	::GetWindowPlacement(Window, &WindPlace);
	WindPlace.showCmd = SW_MAXIMIZE;
	::SetWindowPlacement(Window, &WindPlace);
}

void WindowFunctions::MinimizeWindow(HWND Window)
{
	WINDOWPLACEMENT WindPlace;
	
	WindPlace.length = sizeof(WindPlace);
	
	::GetWindowPlacement(Window, &WindPlace);
	WindPlace.showCmd = SW_MINIMIZE;
	::SetWindowPlacement(Window, &WindPlace);
}

void WindowFunctions::RestoreWindow(HWND Window)
{
	WINDOWPLACEMENT WindPlace;
	
	WindPlace.length = sizeof(WindPlace);
	
	::GetWindowPlacement(Window, &WindPlace);
	WindPlace.showCmd = SW_RESTORE;
	::SetWindowPlacement(Window, &WindPlace);
}

void WindowFunctions::CloseWindow(HWND Window)
{
	::SendMessageTimeout(Window, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 100, NULL);
}

void WindowFunctions::WindowDisable(HWND Window)
{
	::EnableWindow(Window, FALSE);
}

void WindowFunctions::WindowEnable(HWND Window)
{
	::EnableWindow(Window, TRUE);
}

extern int (__stdcall *MySetLayeredWindowAttributes)(struct HWND__ *,unsigned long,unsigned char,unsigned long);

void WindowFunctions::SetTransparency(HWND Window, int transparency)
{
	if (MySetLayeredWindowAttributes != NULL) {
		if (transparency == 0) {
			SetWindowLong(Window, GWL_EXSTYLE, GetWindowLong(Window, GWL_EXSTYLE) & !WS_EX_LAYERED);
		} else {
			SetWindowLong(Window, GWL_EXSTYLE, GetWindowLong(Window, GWL_EXSTYLE) | WS_EX_LAYERED);

			MySetLayeredWindowAttributes(Window, NULL, (int)((100 - transparency) * 2.55), LWA_ALPHA);
		}
	}
}

void WindowFunctions::WindowChangeText(HWND Window)
{
	/*TextEntryDlg entry(m_hWnd, m_hInstance);
	char title[STRING_BUFFER_SIZE];
	
	::GetWindowText(Window, title, STRING_BUFFER_SIZE);
	entry.SetText(title);
	if (entry.DoModal() == IDOK) {
		entry.GetText(title, STRING_BUFFER_SIZE);
		::SetWindowText(Window, title);
	}*/
}

// Clean
void WindowFunctions::WindowSetFont(HWND Window)
{
/*	HFONT hFont;
	CFont *pFont;
	LOGFONT logfont;
	
	hFont = (HFONT)::SendMessage(Window, WM_GETFONT, 0, 0);
	if (hFont == 0) {
		MessageBox(m_hWnd, "This window does not report a font. Be careful trying to change it.", "Font Change Warning", MB_OK);
	} else {
		pFont = CFont::FromHandle(hFont);
		if (pFont == NULL) {
			QuickMessage("This window did not report a valid font.\nI will use a blank font dialog.");
		} else {
			if (pFont->GetLogFont(&logfont) == NULL) {
				QuickMessage("Could not get font characteristics.\nI will use a blank font dialog.");
				delete pFont;
				pFont = NULL;
			}
		}

	}

	CFontDialog* fdlgp;
	if (pFont != NULL) {
		fdlgp = new CFontDialog(&logfont);
	} else {
		fdlgp = new CFontDialog();
	}
	
	if (fdlgp->DoModal() == IDOK) {
		fdlgp->GetCurrentFont(&logfont);
		pFont = new CFont();
		if (pFont->CreateFontIndirect(&logfont) == NULL) {
			QuickMessage("Could not set font.");
		} else {
			::SendMessage(Window, WM_SETFONT, (LONG)(HFONT)*pFont, MAKELPARAM(TRUE, 0));
		}
	}

	delete fdlgp;*/
}

void WindowFunctions::WindowResize(HWND window, int width, int height)
{
	SetWindowPos(window, 0, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
}

void WindowFunctions::WindowMove(HWND window, int x, int y)
{
	SetWindowPos(window, 0, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}

