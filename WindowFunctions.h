// WindowFunctions.h: interface for the WindowFunctions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWFUNCTIONS_H__35727862_A82F_11D3_8EB8_00E029270E87__INCLUDED_)
#define AFX_WINDOWFUNCTIONS_H__35727862_A82F_11D3_8EB8_00E029270E87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class WindowFunctions  
{
public:
	WindowFunctions();
	virtual ~WindowFunctions();

	static void WindowSetFont(HWND Window);
	static void UnTopWindow(HWND window);
	static void TopWindow(HWND window);
	static void BottomWindow(HWND window);
	static void WindowChangeText(HWND Window);
	static void WindowEnable(HWND Window);
	static void WindowDisable(HWND Window);
	static void CloseWindow(HWND Window);
	static void RestoreWindow(HWND Window);
	static void MinimizeWindow(HWND Window);
	static void MaximizeWindow(HWND Window);
	static void WindowMove(HWND window, int width, int height);
	static void WindowResize(HWND window, int width, int height);
	static void SetTransparency(HWND Window, int transparency);
};

#endif // !defined(AFX_WINDOWFUNCTIONS_H__35727862_A82F_11D3_8EB8_00E029270E87__INCLUDED_)
