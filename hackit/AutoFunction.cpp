// AutoFunction.cpp: implementation of the AutoFunction class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable: 4786)
#include <windows.h>
#include <objbase.h>
#include <commctrl.h>
#include <winuser.h>
#include "Wildcard.h"
#include "AutoFunction.h"
#include "SystemHooks.h"
#include "HackItSettings.h"
#include "HackItDlg.h"
#include "WindowFunctions.h"
#include "ProcessFunctions.h"
#include "defines.h"

#define MAXPARAMLEN 256
#define MAXERRORLEN 256
#define NONWHITESPACE "$=\"(){}abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
#define ALPHANUMERIC "$abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
#define OPERATOR "=!><&|"

extern int (__stdcall *MySetLayeredWindowAttributes)(struct HWND__ *,unsigned long,unsigned char,unsigned long);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AutoFunction::AutoFunction()
{
}

struct ChildFinderEnumStruct {
	HWND parent;
	const char *title;
	HWND handle;
};

/*BOOL __stdcall ChildFinderEnumProc(HWND window, LPARAM lParam) {
	ChildFinderEnumStruct *cfe = (ChildFinderEnumStruct*)lParam;

	//if (::GetParent(window) == cfe->parent) {
		char realTitle[256];
		::GetWindowText(window, realTitle, 256);
		if (wildcard(cfe->title, realTitle, FNM_BASHSTYLE | FNM_NOESCAPE)) {
			cfe->handle = window;

			return FALSE;
		}
	//}

	return TRUE;
}*/

AutoFunction::AutoFunction(string f, string t, string c) {
	filename = f;
	title = t;
	winClass = c;
}

void AutoFunction::execute(HackItDlg *dlg, AutoFunction *af, HWND window) {
	if ((af->getTopOnly()) && (GetWindowLong(window, GWL_STYLE) & WS_CHILD)) {
		return;
	}

	switch (af->getAlternateTarget()) {
	case AUTO_TARGET_CHILD:
		window = ::FindWindowEx(window, NULL, NULL, af->getAlternateData().c_str());
		/*ChildFinderEnumStruct cfe;
		cfe.parent = window;
		cfe.title = af->getAlternateData().c_str();
		EnumChildWindows(window, ChildFinderEnumProc, (LPARAM)&cfe);
		window = cfe.handle;*/
		break;
	case AUTO_TARGET_PARENT:
		window = ::GetParent(window);
		break;
	}

	if (window == NULL) {
		return; // cannot find alternate target
	}

	switch (af->getDestructionAction()) {
	case AUTO_ACTION_CLOSEWINDOW:
		WindowFunctions::CloseWindow(window);
		break;
	case AUTO_ACTION_ENDTASK:
		ProcessFunctions::EndTask(window);
		break;
	}

	switch (af->getPriorityAction()) {
	case AUTO_ACTION_IDLE:
		ProcessFunctions::SetWindowPriority(window, IDLE_PRIORITY_CLASS);
		break;
	case AUTO_ACTION_NORMAL:
		ProcessFunctions::SetWindowPriority(window, NORMAL_PRIORITY_CLASS);
		break;
	case AUTO_ACTION_HIGH:
		ProcessFunctions::SetWindowPriority(window, HIGH_PRIORITY_CLASS);
		break;
	case AUTO_ACTION_REALTIME:
		ProcessFunctions::SetWindowPriority(window, REALTIME_PRIORITY_CLASS);
		break;
	}

	switch (af->getTopmostAction()) {
	case AUTO_ACTION_TOPMOST:
		WindowFunctions::TopWindow(window);
		break;
	case AUTO_ACTION_NOTOPMOST:
		WindowFunctions::TopWindow(window);
		break;
	case AUTO_ACTION_BOTTOM:
		WindowFunctions::BottomWindow(window);
		break;
	}

	switch (af->getWindowAction()) {
	case AUTO_ACTION_RESTORE:
		WindowFunctions::RestoreWindow(window);
		break;
	case AUTO_ACTION_MINIMIZE:
		WindowFunctions::MinimizeWindow(window);
		break;
	case AUTO_ACTION_MAXIMIZE:
		WindowFunctions::MaximizeWindow(window);
		break;
	}

	switch (af->getVisibilityAction()) {
	case AUTO_ACTION_VISIBLE:
		::ShowWindow(window, SW_SHOW);
		break;
	case AUTO_ACTION_HIDDEN:
		::ShowWindow(window, SW_HIDE);
		break;
	}

	if (MySetLayeredWindowAttributes) {
		WindowFunctions::SetTransparency(window, af->getTransparency() * 10);
	}

	switch (af->getSpecialAction()) {
	case AUTO_ACTION_PRESSBUTTON:
		// acts on child windows, so wait a bit for them all to be created
		Sleep(500);
		EnumChildWindows(window, HackItDlg::ButtonPressEnum, (LPARAM)af->getSpecialData().c_str());
		break;
	case AUTO_ACTION_ENTERTEXT:
		break;
	}

	if (af->getTrayIcon()) {
		dlg->AddTrayyedItem(window);
	}

	if (af->getResize()) {
		WindowFunctions::WindowResize(window, af->getResizeW(), af->getResizeH());
	}

	if (af->getMove()) {
		WindowFunctions::WindowMove(window, af->getMoveX(), af->getMoveY());
	}
}

BOOL AutoFunction::match(HWND window)
{
//## begin AutoFunction::Match%-160473114.body preserve=yes
	
	if ( !::IsWindow( window ) ) {
		return FALSE ;
	} else if (m_LastWindows.find(window) != m_LastWindows.end()) {
		return FALSE ;
//	} else if ( GetWindowThreadProcessId(window, NULL) == GetWindowThreadProcessId(theDlg->m_hWnd, NULL) ) {
//		return FALSE ;
	} else {
		BOOL match = TRUE ;
		BOOL checkedSomething = FALSE ;

		if (title.length()) {
			checkedSomething = TRUE ;
			char title[256] ;
			::GetWindowText(window, title, 256) ;
			string Title( title ) ;
			if (!(wildcard(getTitle(), Title, (caseSensitive ? FNM_BASHSTYLE | FNM_NOESCAPE : FNM_BASHSTYLE | FNM_CASEFOLD | FNM_NOESCAPE) ))) {
				match = FALSE ;
			}
		}

		if (winClass.length()) {
			checkedSomething = TRUE ;
			char winclass[256] ;
			::GetClassName(window, winclass, 256 ) ;
			string Class(winclass) ;
			if (!(wildcard(getWinClass(), Class, (caseSensitive ? FNM_BASHSTYLE | FNM_NOESCAPE : FNM_BASHSTYLE | FNM_CASEFOLD | FNM_NOESCAPE) ))) {
				match = FALSE ;
			}
		}

		if (filename.length()) {
			checkedSomething = TRUE ;
			char file[256] ;

			GetWindowFileName(window, file, 256);

			string File(file) ;
			if (!(wildcard(getFilename(), File, (caseSensitive ? FNM_BASHSTYLE | FNM_NOESCAPE : FNM_BASHSTYLE | FNM_CASEFOLD | FNM_NOESCAPE) ))) {
				match = FALSE ;
			}
		}

		if ( !checkedSomething ) {
			match = FALSE ;
		}

		if ( match == TRUE ) {
			m_LastWindows.insert(window) ;
		}
		
		return match ;
	}
//## end AutoFunction::Match%-160473114.body
}
