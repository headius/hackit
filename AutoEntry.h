class SettingsDlg ;
/////////////////////////////////////////////////////////////////////////////
// AutoEntryDlg dialog
#include "AutoFunction.h"

class AutoEntryDlg
{
	HWND m_hWndParent;
	HWND m_hWnd;
	HINSTANCE m_hInstance;
// Construction
public:
	void OnCancel();
	int DoModal();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Initialize(SettingsDlg* pdlg , int function);
	SettingsDlg* m_pSettingsDlg;
	int m_Function ;
	AutoEntryDlg(HWND hWndParent, HINSTANCE hInstance);   // standard constructor

protected:
	void OnOK();
	BOOL OnInitDialog(WPARAM wParam, LPARAM lParam);
};
