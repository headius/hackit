// WindowInfo dialog class

/////////////////////////////////////////////////////////////////////////////
// WindowInfo dialog
class WindowInfoDlg {
	HWND m_hWndParent;
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	HWND targetWindow;

public:
	WindowInfoDlg(HWND hWndParent, HINSTANCE hInstance);
	void OnOK();
	void OnInitDialog();
	void init(HWND window);
	int DoModal();
	static BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
protected:
};

