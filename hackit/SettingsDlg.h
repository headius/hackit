// SettingsDlg.h: interface for the SettingsDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGSDLG_H__6A2E1626_F1D0_11D2_8E4A_0008C75CA3DB__INCLUDED_)
#define AFX_SETTINGSDLG_H__6A2E1626_F1D0_11D2_8E4A_0008C75CA3DB__INCLUDED_

#include "HackItSettings.h"	// Added by ClassView
#include <vector>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class HackItDlg;

using namespace std;

class SettingsDlg  
{
	HWND m_hWndParent;
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	vector<AutoFunction*> m_FuncsToDelete;
	/*char m_LowestShowable;
	BOOL m_UpdateItems;
	BOOL m_ShowChilds;
	BOOL m_HideOnStart;
	BOOL m_CaseSensitive;
	BOOL m_ShowSplash;*/

public:
	HWND m_IllegalWindowList;
	bool m_WindowListCleanerFocused;
	bool m_ConsolePollingTimeFocused;
	bool m_NamelessTextFocused;
	HackItSettings m_OldSettings;
	void SelectFont();
	LRESULT OnWindowDlgCommand(WPARAM wParam, LPARAM lParam);
	HackItSettings m_Settings;
	void AddEditFunction(char *name, AutoFunction *function);
	void DeleteIllegalWindow();
	void EditIllegalWindow();
	void AddIllegalWindow();
	void AddFunction();
	void EditFunction();
	void DeleteFunction();
	void SelchangeFunctions();
	HPROPSHEETPAGE m_FunctionSettingsPage;
	HPROPSHEETPAGE m_IllegalSettingsPage;
	HPROPSHEETPAGE m_GeneralSettingsPage;
	HPROPSHEETPAGE m_WindowSettingsPage;
	HWND m_hWndWindowDlg;
	HWND m_hWndGeneralDlg;
	HWND m_hWndIllegalDlg;
	HWND m_hWndFunctionDlg;
	HackItDlg *m_pHackItDlg;
	void Initialize(HackItSettings& settings);
	PROPSHEETHEADER m_PropSheetHeader;
	int DoModal();
	static BOOL CALLBACK WindowDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK GeneralDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK IllegalDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK FunctionDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	SettingsDlg(HWND hWndParent, HINSTANCE hInstance);
	virtual ~SettingsDlg();

	//void OnOK();
	//void OnCancel();
	void InitWindowDlg();
	void InitGeneralDlg();
	void InitFunctionDlg();
	void InitIllegalDlg();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

#endif // !defined(AFX_SETTINGSDLG_H__6A2E1626_F1D0_11D2_8E4A_0008C75CA3DB__INCLUDED_)
