#include "windows.h"

class TextEntryDlg
{
// Construction
public:
	char m_Text[STRING_BUFFER_SIZE];
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HWND m_hWndParent;
	void SetText(char *buff);
	char *GetText(char *buff, UINT size);
	TextEntryDlg(HWND hWndParent, HINSTANCE hInstance);   // standard constructor
	void OnOK();
	void OnCancel();
	static BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnInitDialog();
	int DoModal();
// Implementation
protected:
};

