// HackItSettings.h: interface for the HackItSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HACKITSETTINGS_H__071647F2_5A28_11D3_8E81_0008C75CA3DB__INCLUDED_)
#define AFX_HACKITSETTINGS_H__071647F2_5A28_11D3_8E81_0008C75CA3DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma	warning(disable: 4786)
#include <set>
#include <string>
#include <map>
using namespace std;

class IllegalWindowSpec;
class AutoFunction;

typedef set<IllegalWindowSpec*> IllegalWindowSpec_set;
typedef map<string, AutoFunction*> string_AutoFunction_map;

class HackItSettings  
{
public:
	IllegalWindowSpec_set m_IllegalWindowSpecList;
	string m_NamelessText;
	char m_LowestShowable;
	BOOL m_UseLargeIcons;
	BOOL m_WindowIconFirst;
	BOOL m_UpdateItems;
	BOOL m_ShowChilds;
	BOOL m_HideOnStart;
	BOOL m_Transparent;
	int m_TransparentPercent;
	BOOL m_CaseSensitive;
	BOOL m_ShowSplash;
	bool m_FontWasLoaded;
	DWORD m_Priority;
	BOOL m_AlwaysOnTop;
	BOOL m_TaskbarIcon;
	BOOL m_ChildFunctionsEnabled;
	BOOL m_AutoFunctionsEnabled;
	LONG m_PlacementShow;
	LONG m_PlacementRight;
	LONG m_PlacementBottom;
	LONG m_PlacementLeft;
	LONG m_PlacementTop;
	string_AutoFunction_map m_AutoFunctions;
	LOGFONT m_LogFont;
	BOOL m_ChildrenInTopmostList;
	BOOL m_SysMenuTransparency;
	BOOL m_SysMenuPriority;
	BOOL m_SysMenuTrayyed;
	BOOL m_SysMenuTopped;
	BOOL m_SysMenuVisible;
	BOOL m_SysMenuCapture;
	BOOL m_SysMenuAppend;
	BOOL m_UpdateAddRemove;
	BOOL m_UpdateAuto;
	BOOL m_WindowListCleaning;
	int m_WindowListCleanerTime;
	HackItSettings();
	virtual ~HackItSettings();
	void LoadSettings();
	void SaveSettings();
	BOOL WriteRegistryString(HKEY key, LPCTSTR value, LPCTSTR stringValue);
	BOOL WriteRegistryInt(HKEY key, LPCTSTR value, int intValue);
	DWORD GetRegistryString(HKEY key, LPCTSTR value, LPCTSTR defaultString, LPTSTR target, DWORD size);
	UINT GetRegistryInt(HKEY key, LPCTSTR value, int defaultInt);
};

#endif // !defined(AFX_HACKITSETTINGS_H__071647F2_5A28_11D3_8E81_0008C75CA3DB__INCLUDED_)
