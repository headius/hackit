class IllegalWindowSpec;

class IllegalWindowEntryDlg
{
// Construction
public:
	HWND m_hWnd;
	HWND m_hWndParent;
	HINSTANCE m_hInstance;
	char* GetIllegalName();
	char* GetIllegalTitle();
	char* GetIllegalClass();
	char* GetIllegalFilename();
	bool GetIllegalCaseSensitive();
	char m_IllegalName[STRING_BUFFER_SIZE];
	char m_IllegalTitle[STRING_BUFFER_SIZE];
	char m_IllegalClass[STRING_BUFFER_SIZE];
	char m_IllegalFilename[STRING_BUFFER_SIZE];
	bool m_IllegalCaseSensitive;
	void OnInitDialog();
	static BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	int DoModal();
	void Initialize(IllegalWindowSpec *pSpec);
	IllegalWindowEntryDlg(HWND hWndParent, HINSTANCE hInstance	);   // standard constructor
protected:
	void OnOK();
	void OnCancel();
};
