// HackItDlg.h : header file
//
//#include "afxtempl.h"
/////////////////////////////////////////////////////////////////////////////
// HackItDlg dialog
#pragma	warning(disable: 4786)
#include <set>
#include <string>
#include <map>
using namespace std;

class IllegalWindowSpec;
class AutoFunction;

typedef set<IllegalWindowSpec*> IllegalWindowSpec_set;
typedef set<int> int_set;
typedef map<string, AutoFunction*> string_AutoFunction_map;
typedef map<HWND, HTREEITEM> HWND_HTREEITEM_map;

void GetWindowFileName(HWND, char*, int);

class HackItDlg
{
	// Construction
public:
	SystemHooks *m_pHooks;
	HackItSettings m_Settings;

	HWND_HTREEITEM_map m_TrayyedItemMap;
	HWND_HTREEITEM_map m_ToppedItemMap;
	HWND_HTREEITEM_map m_WindowItemMap;

	CRITICAL_SECTION m_CS_TrayyedItemMap;
	CRITICAL_SECTION m_CS_ToppedItemMap;
	CRITICAL_SECTION m_CS_WindowItemMap;
	CRITICAL_SECTION m_CS_ReusableIconList;

	HWND m_hWnd;
	HWND m_hWndManager;
	HANDLE m_ListThreadHandle;
	DWORD m_ListThreadID;
	static BOOL CALLBACK EnumChildWindowsProc(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK ConsolePollProc(HWND hwnd, LPARAM lParam);
	//static DWORD WINAPI ListRefreshProc(void * pParam);
	static LRESULT CALLBACK ListThreadWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI ListThreadProc(LPVOID data);

	int_set m_ReusableIconList;
	UINT m_ItemMenu;
	HWND m_ItemHwnd;
	HMENU m_ItemMainMenu;
	HMENU m_ItemSystemMenu;
	int AddOrReuseIcon( HICON icon );
	//void SetWindowItemIconIndex( HTREEITEM hItem , int index );
	int GetClassIcon( HWND window );
	HICON GetWindowIcon( HWND window );
	void Shutdown( );
	BOOL m_ThreadDie;
	//void SetRefreshSpeed( UINT value );
	HANDLE m_RefreshThread;
	void AboutDialog( );
	HMENU m_hScrollbarMenu;
	HMENU m_hButtonMenu;
	HMENU m_hWindowMenu;
	HMENU m_hTransparencyMenu;
	HMENU m_hProcessMenu;

	void ButtonPress( HWND Button );
	static BOOL CALLBACK ButtonPressEnum(HWND window, LPARAM param);

	HWND m_LastWindow;
	BOOL m_WindowListAtom;
	BOOL m_EditingTitle;
	void UpdateItem(HWND wnd);

	//HWND m_Window;
	HINSTANCE m_TrayManDLL;
	HWND m_RedirectWindow;
	HMENU m_WindowPopup;
	NOTIFYICONDATA m_TrayIconData;
	HMENU m_MainMenu;
HIMAGELIST m_ImageList;

HTREEITEM m_ToppedItem;
	HTREEITEM m_TrayyedItem;
HTREEITEM m_HiddenItem;
HTREEITEM m_Desktop;


	int GetWindowItemIconIndex( HTREEITEM hItem );
	bool WindowFilter(HWND window);
	void DeleteChildItems(HTREEITEM Parent, BOOL deleteRelatedItems);
	//int ShouldNotBeListed( HWND window );

	//void CleanList( );
	void CheckAuto( HWND TempWindow );

	void SetIconData( HWND hwndTgt , UINT hwndCtl );
	BOOL AddIcon( );
	BOOL UpdateIcon( );
	BOOL DeleteIcon( );

	void UpdateMenuChecks( HWND wnd );
	void ResetMenuChecks( );

	HWND GetWindowItemHwnd( HTREEITEM hItem );
	//HICON GetWindowItemIcon( HTREEITEM hItem );
	//char* GetWindowItemText( HTREEITEM hItem );

	//unsigned long SetWindowItemIcon( HTREEITEM hItem , HICON icon );
	void SetWindowItemText( HTREEITEM hItem , char* newText );

	void CreateList( );
	HackItDlg(HINSTANCE hInstance);	// standard constructor

	HWND m_WindowList;

	HWND m_HandleInfo;
	HWND m_ClassInfo;
	HWND m_StatusBar;
private:
	HMENU m_TrayMainMenu;
	HMENU m_TraySystemMenu;
	//HTREEITEM AddChildItem( HWND Window , HTREEITEM Parent );

public:
	HTREEITEM AddBasicItem( HWND Window, BOOL addRelatedItems);
	HTREEITEM AddTrayyedItem(HWND window);
	HTREEITEM AddToppedItem(HWND window);
	void DeleteBasicItem(HWND TempWindow, BOOL deleteRelatedItems);
	void DeleteToppedItem(HWND window);
	void DeleteTrayyedItem(HWND window);
	void CacheIconSlot(int index);
	BOOL m_HackItRunning;
	void WindowInfo();
	void OnContextHelp(WPARAM wParam, LPARAM lParam);
	HMENU m_SysMenuPriorityMenu;
	HMENU m_SysMenuTransparencyMenu;
	LRESULT OnDoubleClickTree1(NMHDR* pNMHDR);
	HMENU m_RedirectSysMenu;
	void OnSysMenuEnded(WPARAM wParam, LPARAM lParam);
	//LRESULT SelChanged(HWND item);
	void OnHackItFunctionDetected(WPARAM wParam, LPARAM lParam);
	void OnSysMenuDetected(WPARAM wParam, LPARAM lParam);
	//int OnNcPaint(WPARAM wParam, LPARAM lParam);
	HWND m_SecretWindow;
	//void RefreshList();
	HANDLE m_RefreshMutex;
	void OnSetTextDetected(WPARAM wParam, LPARAM lParam);
	void ReAddWindow(HWND hWnd);
	void OnPosChangedDetected(WPARAM wParam, LPARAM lParam);
	void DeleteWindow(HWND TempWindow);
	HTREEITEM AddWindow(HWND Window);
	void OnDestroyDetected(WPARAM wParam, LPARAM lParam);
	void OnCreateDetected(WPARAM wParam, LPARAM lParam);
	void CleanList();
	BOOL m_HasListChanged;
	BOOL m_Refreshing;
	void Settings();
	LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	HINSTANCE m_hInstance;
	void Start();
	void QuickMessage(LPCTSTR text);
	
protected:
	HICON m_hIcon;
	HICON m_hIconSM;
	
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnEndSession(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy();
	LRESULT OnListThreadDestroy();
	//LRESULT OnPaint();
	LRESULT OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	LRESULT OnSize(UINT nType, int cx, int cy);
	LRESULT OnSelchangedTree1(NMHDR* pNMHDR);
	LRESULT OnClose();
	LRESULT OnContextMenuTree1(HWND hWnd, int x, int y);
	//LRESULT OnLButtonUp(UINT nFlags, int x, int y);
	//LRESULT OnBeginlabeleditTree1(NMHDR* pNMHDR);
	LRESULT OnRclickTree1(NMHDR* pnmh);
	//LRESULT OnEndlabeleditTree1(NMHDR* pNMHDR);
	//LRESULT OnGetdispinfoTree1(NMHDR* pNMHDR);
	long OnTrayNotification( WPARAM wParam , LPARAM lParam ) ;
	long OnCheckTrayyedList( WPARAM wParam , LPARAM lParam ) ;
};
