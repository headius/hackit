#include <windows.h>

class SplashDlg
{
	HINSTANCE m_hInstance;
	HWND m_hWndParent;
	HWND m_hWnd;
// Construction
public:
	int DoModal();
	static BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	SplashDlg(HWND hWndParent, HINSTANCE hInstance);   // standard constructor

protected:
	void OnTimer(UINT nIDEvent);
	BOOL OnInitDialog();
};
