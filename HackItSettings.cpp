// HackItSettings.cpp: implementation of the HackItSettings class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "defines.h"
#include "HackItSettings.h"
#include "AutoFunction.h"
#include "IllegalWindowSpec.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HackItSettings::HackItSettings()
{
	m_HideOnStart = FALSE;
	m_Transparent = FALSE;
	m_TransparentPercent = 20;
	m_ShowChilds = TRUE;
	m_TaskbarIcon = FALSE;
	m_LowestShowable = '!';
	m_UpdateItems = TRUE;
	m_PlacementTop = 150;
	m_PlacementLeft = 200;
	m_PlacementBottom = 450;
	m_PlacementRight = 500;
	m_ShowSplash = TRUE;
	m_PlacementShow = WS_VISIBLE;
	m_CaseSensitive = FALSE;
	m_UpdateItems = TRUE;
	m_AutoFunctionsEnabled = TRUE;
	m_ChildFunctionsEnabled = FALSE;
	m_ChildrenInTopmostList = FALSE;
	m_AlwaysOnTop = FALSE;
	m_Priority = NORMAL_PRIORITY_CLASS;
	m_FontWasLoaded = false;
	m_SysMenuCapture = TRUE;
	m_SysMenuAppend = FALSE;
	m_SysMenuVisible = TRUE;
	m_SysMenuTopped = TRUE;
	m_SysMenuTrayyed = TRUE;
	m_SysMenuPriority = TRUE;
	m_SysMenuTransparency = TRUE;
	m_UpdateAddRemove = FALSE;
	m_UpdateAuto = FALSE;
	m_NamelessText = "<no title>";
	m_ConsolePolling = TRUE;
	m_ConsolePollingTime = 500;
	m_WindowListCleaning = TRUE;
	m_WindowListCleanerTime = 1000;
}

HackItSettings::~HackItSettings()
{

}

void HackItSettings::SaveSettings()
{
#ifdef FULLVERSION
	WINDOWPLACEMENT Placement;
	AutoFunction *af;
	char regkey[STRING_BUFFER_SIZE];
	string_AutoFunction_map::iterator pos;
	int count = 0;
	LOGFONT font;
	LONG openResult;
	DWORD createResult;
	HKEY topKey;
	HKEY settingsKey;
	HKEY fontKey;
	HKEY scriptsKey;
	HKEY scriptKey;
	HKEY illegalsKey;
	HKEY illegalKey;


	openResult = RegCreateKeyEx(
		HKEY_CURRENT_USER,
		"Software\\Headius\\HackIt",
		0,
		"",
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&topKey,
		&createResult);

	if (openResult == ERROR_SUCCESS) {

		RegDeleteKey(topKey, "Settings");
		openResult = RegCreateKeyEx(
			topKey,
			"Settings",
			0,
			"",
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&settingsKey,
			&createResult);
	
		if (openResult == ERROR_SUCCESS) {
			Placement.length = sizeof(Placement);
			GetWindowPlacement(::FindWindow("HackIt", NULL), &Placement);
		
			WriteRegistryInt(settingsKey, "Hide Hack-It On Start", m_HideOnStart);
			//WriteRegistryInt(settingsKey, "Show Child Windows", m_ShowChilds);
			WriteRegistryInt(settingsKey, "Show Titleless Windows", m_LowestShowable);
			WriteRegistryInt(settingsKey, "Text Refreshing", m_UpdateItems);
			WriteRegistryInt(settingsKey, "Splash", m_ShowSplash);
			WriteRegistryInt(settingsKey, "Top", Placement.rcNormalPosition.top);
			WriteRegistryInt(settingsKey, "Left", Placement.rcNormalPosition.left);
			WriteRegistryInt(settingsKey, "Bottom", Placement.rcNormalPosition.bottom);
			WriteRegistryInt(settingsKey, "Right", Placement.rcNormalPosition.right);
			WriteRegistryInt(settingsKey, "Auto Functions Enabled", m_AutoFunctionsEnabled);
			WriteRegistryInt(settingsKey, "Child Functions Enabled", m_ChildFunctionsEnabled);
			WriteRegistryInt(settingsKey, "Child Windows In Topmost List", m_ChildrenInTopmostList);
			WriteRegistryInt(settingsKey, "Always On Top", m_AlwaysOnTop);
			WriteRegistryInt(settingsKey, "Transparent", m_Transparent);
			WriteRegistryInt(settingsKey, "Transparency Percent", m_TransparentPercent);
			WriteRegistryInt(settingsKey, "Priority", m_Priority);
			WriteRegistryInt(settingsKey, "System Menu Capture", m_SysMenuCapture);
			WriteRegistryInt(settingsKey, "System Menu Append", m_SysMenuAppend);
			WriteRegistryInt(settingsKey, "System Menu Visible", m_SysMenuVisible);
			WriteRegistryInt(settingsKey, "System Menu Topped", m_SysMenuTopped);
			WriteRegistryInt(settingsKey, "System Menu Trayyed", m_SysMenuTrayyed);
			WriteRegistryInt(settingsKey, "System Menu Priority", m_SysMenuPriority);
			WriteRegistryInt(settingsKey, "System Menu Transparency", m_SysMenuTransparency);
			WriteRegistryInt(settingsKey, "Update Adds Or Removes", m_UpdateAddRemove);
			WriteRegistryInt(settingsKey, "Update Checks Auto Functions", m_UpdateAuto);
			WriteRegistryInt(settingsKey, "Console Polling", m_ConsolePolling);
			WriteRegistryInt(settingsKey, "Console Polling Time", m_ConsolePollingTime);
			WriteRegistryInt(settingsKey, "Window List Cleaning", m_WindowListCleaning);
			WriteRegistryInt(settingsKey, "Window List Cleaner Time", m_WindowListCleanerTime);
			WriteRegistryString(settingsKey, "Nameless Text", m_NamelessText.c_str());

			RegCloseKey(settingsKey);
		}

		RegDeleteKey(topKey, "Font");
		openResult = RegCreateKeyEx(
			topKey,
			"Font",
			0,
			"",
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&fontKey,
			&createResult);
	
		if (openResult == ERROR_SUCCESS) {
			font = m_LogFont;
			WriteRegistryInt(fontKey, "Height", (int)font.lfHeight);
			WriteRegistryInt(fontKey, "Width", (int)font.lfWidth);
			WriteRegistryInt(fontKey, "Escapement", (int)font.lfEscapement);
			WriteRegistryInt(fontKey, "Orientation", (int)font.lfOrientation);
			WriteRegistryInt(fontKey, "Weight", (int)font.lfWeight);
			WriteRegistryInt(fontKey, "Italic", (int)font.lfItalic);
			WriteRegistryInt(fontKey, "Underline", (int)font.lfUnderline);
			WriteRegistryInt(fontKey, "StrikeOut", (int)font.lfStrikeOut);
			WriteRegistryInt(fontKey, "CharSet", (int)font.lfCharSet);
			WriteRegistryInt(fontKey, "OutPrecision", (int)font.lfOutPrecision);
			WriteRegistryInt(fontKey, "ClipPrecision", (int)font.lfClipPrecision);
			WriteRegistryInt(fontKey, "Quality", (int)font.lfQuality);
			WriteRegistryInt(fontKey, "PitchAndFamily", (int)font.lfPitchAndFamily);
			WriteRegistryString(fontKey, "FaceName", font.lfFaceName);
			RegCloseKey(fontKey);
		}
		
		RegDeleteKey(topKey, "Scripts");
		openResult = RegCreateKeyEx(
			topKey,
			"Scripts",
			0,
			"",
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&scriptsKey,
			&createResult);
	
		if (openResult == ERROR_SUCCESS) {
			pos = m_AutoFunctions.begin();
			count = 0;
			
			while (pos != m_AutoFunctions.end()) {
				count ++;
				af = pos->second;
				sprintf(regkey, "Script%d", count);
				
				openResult = RegCreateKeyEx(
					scriptsKey,
					regkey,
					0,
					"",
					REG_OPTION_NON_VOLATILE,
					KEY_ALL_ACCESS,
					NULL,
					&scriptKey,
					&createResult);

				if (openResult == ERROR_SUCCESS) {
					WriteRegistryString(scriptKey, "Name", pos->first.c_str());
					WriteRegistryString(scriptKey, "Title", af->getTitle().c_str());
					WriteRegistryString(scriptKey, "Class", af->getWinClass().c_str());
					WriteRegistryString(scriptKey, "Filename", af->getFilename().c_str());
					//WriteRegistryString(scriptKey, "Script", af->GetScript());
					WriteRegistryInt(scriptKey, "Window Action", af->getWindowAction());
					WriteRegistryInt(scriptKey, "TopmostAction", af->getTopmostAction());
					WriteRegistryInt(scriptKey, "Priority Action", af->getPriorityAction());
					WriteRegistryInt(scriptKey, "Destruction Action", af->getDestructionAction());
					WriteRegistryInt(scriptKey, "Visibility Action", af->getVisibilityAction());
					WriteRegistryInt(scriptKey, "Transparency", af->getTransparency());
					WriteRegistryInt(scriptKey, "Special Action", af->getSpecialAction());
					WriteRegistryInt(scriptKey, "Alternate Target", af->getAlternateTarget());
					WriteRegistryInt(scriptKey, "Resize", af->getResize());
					WriteRegistryInt(scriptKey, "Move", af->getMove());
					WriteRegistryInt(scriptKey, "Resize Width", af->getResizeW());
					WriteRegistryInt(scriptKey, "Resize Height", af->getResizeH());
					WriteRegistryInt(scriptKey, "Move X", af->getMoveX());
					WriteRegistryInt(scriptKey, "Move Y", af->getMoveY());
					WriteRegistryString(scriptKey, "Special Data", af->getSpecialData().c_str());
					WriteRegistryString(scriptKey, "Alternate Data", af->getAlternateData().c_str());
					WriteRegistryInt(scriptKey, "Tray Icon", af->getTrayIcon());
					WriteRegistryInt(scriptKey, "Top-level Only", af->getTopOnly());
					WriteRegistryInt(scriptKey, "Case Sensitive", af->getCaseSensitive());
					RegCloseKey(scriptKey);
				}

				pos++;
			}
			WriteRegistryInt(scriptsKey, "Count", count);
			RegCloseKey(scriptsKey);
		}


		RegDeleteKey(topKey, "Illegal Windows");
		openResult = RegCreateKeyEx(
			topKey,
			"Illegal Windows",
			0,
			"",
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&illegalsKey,
			&createResult);
	
		if (openResult == ERROR_SUCCESS) {
			IllegalWindowSpec* pIllWinSpec;
		
			IllegalWindowSpec_set::iterator current = m_IllegalWindowSpecList.begin();
		
			count = 0;
			while (current != m_IllegalWindowSpecList.end()) {
				count ++;
				pIllWinSpec = *current++;
			
				sprintf(regkey, "Illegal%d", count);
				openResult = RegCreateKeyEx(
					illegalsKey,
					regkey,
					0,
					"",
					REG_OPTION_NON_VOLATILE,
					KEY_ALL_ACCESS,
					NULL,
					&illegalKey,
					&createResult);
			
				if (openResult == ERROR_SUCCESS) {
					WriteRegistryInt(illegalKey, "Case Sensitive", pIllWinSpec->GetCaseSensitive()?1:0);
					WriteRegistryString(illegalKey, "Name", pIllWinSpec->GetName().c_str());
					WriteRegistryString(illegalKey, "Title", pIllWinSpec->GetTitle().c_str());
					WriteRegistryString(illegalKey, "Class", pIllWinSpec->GetClass().c_str());
					WriteRegistryString(illegalKey, "Filename", pIllWinSpec->GetFilename().c_str());
					RegCloseKey(illegalKey);
				}
			}
			WriteRegistryInt(illegalsKey, "Count", count);
			RegCloseKey(illegalsKey);
		}
		RegCloseKey(topKey);
	}
#endif
}

void HackItSettings::LoadSettings()
{
#ifdef FULLVERSION
	WINDOWPLACEMENT Placement;
	int count = 0;
	char name[STRING_BUFFER_SIZE];
	char filename[STRING_BUFFER_SIZE];
	char winClass[STRING_BUFFER_SIZE];
	char title[STRING_BUFFER_SIZE];
	char fontName[STRING_BUFFER_SIZE];
	char astring[STRING_BUFFER_SIZE];
	AutoFunction *af;
	char index[STRING_BUFFER_SIZE];
	LOGFONT font;
	LONG openResult;
	HKEY topKey;
	HKEY settingsKey;
	HKEY fontKey;
	HKEY scriptsKey;
	HKEY scriptKey;
	HKEY illegalsKey;
	HKEY illegalKey;

	openResult = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		"Software\\Headius\\HackIt",
		0,
		KEY_READ,
		&topKey);

	if (openResult == ERROR_SUCCESS) {
		Placement.length = sizeof(Placement);
		GetWindowPlacement(::FindWindow("HackIt", NULL), &Placement);
	
		openResult = RegOpenKeyEx(
			topKey,
			"Settings",
			0,
			KEY_READ,
			&settingsKey);

		if (openResult == ERROR_SUCCESS) {
			m_HideOnStart = GetRegistryInt(settingsKey, "Hide Hack-It On Start", 0);
			//m_ShowChilds = GetRegistryInt(settingsKey, "Show Child Windows", 0);
			m_LowestShowable = GetRegistryInt(settingsKey, "Show Titleless Windows", '!');
			m_UpdateItems = GetRegistryInt(settingsKey, "Text Refreshing", 0);
			m_PlacementTop = GetRegistryInt(settingsKey, "Top", 150);
			m_PlacementLeft = GetRegistryInt(settingsKey, "Left", 200);
			m_PlacementBottom = GetRegistryInt(settingsKey, "Bottom", 450);
			m_PlacementRight = GetRegistryInt(settingsKey, "Right", 500);
			m_ShowSplash = GetRegistryInt(settingsKey, "Splash", TRUE);
			m_AutoFunctionsEnabled = GetRegistryInt(settingsKey, "Auto Functions Enabled", TRUE);
			m_ChildFunctionsEnabled = GetRegistryInt(settingsKey, "Child Functions Enabled", FALSE);
			m_ChildrenInTopmostList = GetRegistryInt(settingsKey, "Child Windows In Topmost List", FALSE);
			m_AlwaysOnTop = GetRegistryInt(settingsKey, "Always On Top", FALSE);
			m_Transparent = GetRegistryInt(settingsKey, "Transparent", FALSE);
			m_TransparentPercent = GetRegistryInt(settingsKey, "Transparency Percent", 20);
			m_Priority = GetRegistryInt(settingsKey, "Priority", NORMAL_PRIORITY_CLASS);
			m_SysMenuCapture = GetRegistryInt(settingsKey, "System Menu Capture", FALSE);
			m_SysMenuAppend = GetRegistryInt(settingsKey, "System Menu Append", FALSE);
			m_SysMenuVisible = GetRegistryInt(settingsKey, "System Menu Visible", FALSE);
			m_SysMenuTopped = GetRegistryInt(settingsKey, "System Menu Topped", FALSE);
			m_SysMenuTrayyed = GetRegistryInt(settingsKey, "System Menu Trayyed", FALSE);
			m_SysMenuPriority = GetRegistryInt(settingsKey, "System Menu Priority", FALSE);
			m_SysMenuTransparency = GetRegistryInt(settingsKey, "System Menu Transparency", FALSE);
			m_UpdateAddRemove = GetRegistryInt(settingsKey, "Update Adds Or Removes", FALSE);
			m_UpdateAuto = GetRegistryInt(settingsKey, "Update Checks Auto Functions", FALSE);
			m_ConsolePolling = GetRegistryInt(settingsKey, "Console Polling", TRUE);
			m_ConsolePollingTime = GetRegistryInt(settingsKey, "Console Polling Time", 500);
			m_WindowListCleaning = GetRegistryInt(settingsKey, "Window List Cleaning", TRUE);
			m_WindowListCleanerTime = GetRegistryInt(settingsKey, "Window List Cleaner Time", 1000);
			GetRegistryString(settingsKey, "Nameless Text", "<no title>", astring, STRING_BUFFER_SIZE);
			m_NamelessText = astring;
			
			if (m_HideOnStart) {
				m_PlacementShow = 0;
			} else {
				m_PlacementShow = WS_VISIBLE;
			}
			RegCloseKey(settingsKey);
		}

		openResult = RegOpenKeyEx(
			topKey,
			"Font",
			0,
			KEY_READ,
			&fontKey);

		if (openResult == ERROR_SUCCESS) {
			font.lfHeight = GetRegistryInt(fontKey, "Height", 0xfffffff5);
			font.lfWidth = (LONG)GetRegistryInt(fontKey, "Width", 0);
			font.lfEscapement = (LONG)GetRegistryInt(fontKey, "Escapement", 0);
			font.lfOrientation = (LONG)GetRegistryInt(fontKey, "Orientation", 0);
			font.lfWeight = (LONG)GetRegistryInt(fontKey, "Weight", 0x190);
			font.lfItalic = (UCHAR)GetRegistryInt(fontKey, "Italic", 0);
			font.lfUnderline = (BYTE)GetRegistryInt(fontKey, "Underline", 0);
			font.lfStrikeOut = (BYTE)GetRegistryInt(fontKey, "StrikeOut", 0);
			font.lfCharSet = (BYTE)GetRegistryInt(fontKey, "CharSet", 0);
			font.lfOutPrecision = (BYTE)GetRegistryInt(fontKey, "OutPrecision", 0x1);
			font.lfClipPrecision = (BYTE)GetRegistryInt(fontKey, "ClipPrecision", 0x2);
			font.lfQuality = (BYTE)GetRegistryInt(fontKey, "Quality", 0x1);
			font.lfPitchAndFamily = (BYTE)GetRegistryInt(fontKey, "PitchAndFamily", 0x22);
			GetRegistryString(fontKey, "FaceName", "MS Sans Serif", fontName, STRING_BUFFER_SIZE);
			strncpy(font.lfFaceName, fontName, 32);
			m_LogFont = font;
			m_FontWasLoaded = true;

			RegCloseKey(fontKey);
		}
		
		openResult = RegOpenKeyEx(
			topKey,
			"Scripts",
			0,
			KEY_READ,
			&scriptsKey);

		if (openResult == ERROR_SUCCESS) {
			count = GetRegistryInt(scriptsKey, "Count", 0);
			for (; count > 0; count--) {
				sprintf(index, "Script%d", count);
				openResult = RegOpenKeyEx(
					scriptsKey,
					index,
					0,
					KEY_READ,
					&scriptKey);
				
				if (openResult == ERROR_SUCCESS) {
					af = new AutoFunction();
					GetRegistryString(scriptKey, "Name", "", name, STRING_BUFFER_SIZE);
					GetRegistryString(scriptKey, "Title", "", title, STRING_BUFFER_SIZE);
					af->setTitle(title);
					GetRegistryString(scriptKey, "Class", "", winClass, STRING_BUFFER_SIZE);
					af->setWinClass(winClass);
					GetRegistryString(scriptKey, "Filename", "", filename, STRING_BUFFER_SIZE);
					af->setFilename(filename);
					af->setWindowAction(GetRegistryInt(scriptKey, "Window Action", 0));
					af->setTopmostAction(GetRegistryInt(scriptKey, "TopmostAction", 0));
					af->setPriorityAction(GetRegistryInt(scriptKey, "Priority Action", 0));
					af->setDestructionAction(GetRegistryInt(scriptKey, "Destruction Action", 0));
					af->setVisibilityAction(GetRegistryInt(scriptKey, "Visibility Action", 0));
					af->setTransparency(GetRegistryInt(scriptKey, "Transparency", 0));
					af->setSpecialAction(GetRegistryInt(scriptKey, "Special Action", 0));
					af->setAlternateTarget(GetRegistryInt(scriptKey, "Alternate Target", 0));
					af->setResize(GetRegistryInt(scriptKey, "Resize", 0));
					af->setMove(GetRegistryInt(scriptKey, "Move", 0));
					af->setResizeW(GetRegistryInt(scriptKey, "Resize Width", 0));
					af->setResizeH(GetRegistryInt(scriptKey, "Resize Height", 0));
					af->setMoveX(GetRegistryInt(scriptKey, "Move X", 0));
					af->setMoveY(GetRegistryInt(scriptKey, "Move Y", 0));
					GetRegistryString(scriptKey, "Special Data", "", astring, STRING_BUFFER_SIZE);
					af->setSpecialData(astring);
					GetRegistryString(scriptKey, "Alternate Data", "", astring, STRING_BUFFER_SIZE);
					af->setAlternateData(astring);
					af->setTrayIcon(GetRegistryInt(scriptKey, "Tray Icon", 0));
					af->setTopOnly(GetRegistryInt(scriptKey, "Top-level Only", 0));
					af->setCaseSensitive(GetRegistryInt(scriptKey, "Case Sensitive", 0));

					m_AutoFunctions[name] = af;
					RegCloseKey(scriptKey);
				}
			}
			RegCloseKey(scriptsKey);
		}

		openResult = RegOpenKeyEx(
			topKey,
			"Illegal Windows",
			0,
			KEY_READ,
			&illegalsKey);

		if (openResult == ERROR_SUCCESS) {
			count = GetRegistryInt(illegalsKey, "Count", 0);
			char text[STRING_BUFFER_SIZE];
			UINT type, caseSensitive;
			IllegalWindowSpec* pIllWinSpec;
		
			for (int i = 0; i < count; i++) {
				sprintf(index, "Illegal%d", i + 1);
				openResult = RegOpenKeyEx(
					illegalsKey,
					index,
					0,
					KEY_READ,
					&illegalKey);
					
				if (openResult == ERROR_SUCCESS) {
					caseSensitive = GetRegistryInt(illegalKey, "Case Sensitive", -1);

					pIllWinSpec = new IllegalWindowSpec();

					if (caseSensitive == -1) {
						// old style illegal window settings...import
						type = GetRegistryInt(illegalKey, "Type", -1);
						switch (type) {
						case 0: // class spec
							GetRegistryString(illegalKey, "Text", "", text, STRING_BUFFER_SIZE);
							pIllWinSpec->SetClass(text);
							break;
						case 1: // window spec
							GetRegistryString(illegalKey, "Text", "", text, STRING_BUFFER_SIZE);
							pIllWinSpec->SetTitle(text);
							break;
						}
						pIllWinSpec->SetCaseSensitive(false);
						pIllWinSpec->SetName(index);
					} else { // new style
						GetRegistryString(illegalKey, "Name", "", text, STRING_BUFFER_SIZE);
						pIllWinSpec->SetName(text);
						GetRegistryString(illegalKey, "Title", "", text, STRING_BUFFER_SIZE);
						pIllWinSpec->SetTitle(text);
						GetRegistryString(illegalKey, "Class", "", text, STRING_BUFFER_SIZE);
						pIllWinSpec->SetClass(text);
						GetRegistryString(illegalKey, "Filename", "", text, STRING_BUFFER_SIZE);
						pIllWinSpec->SetFilename(text);
						pIllWinSpec->SetCaseSensitive(GetRegistryInt(illegalKey, "Case Sensitive", 0) != 0);
					}
			
					m_IllegalWindowSpecList.insert(pIllWinSpec);
					RegCloseKey(illegalKey);
				}
			}
			RegCloseKey(illegalsKey);
		}
		RegCloseKey(topKey);
	}		
#else
	MessageBox(NULL, "You are running the shareware version of Hack-It. No settings will be saved.\n\nPlease see the help file for information on purchasing your copy of Hack-It PRO!", "Hack-It Shareware Version", MB_OK);
#endif
}

UINT HackItSettings::GetRegistryInt(HKEY key, LPCTSTR value, int defaultInt)
{
	DWORD sizeVar;
	DWORD typeVar;
	LONG result;
	UINT buffer;
	string out;

	sizeVar = sizeof(UINT);
	buffer = 0;

	result = RegQueryValueEx(key, value, NULL, &typeVar, (LPBYTE)&buffer, &sizeVar);
	
	if (result == ERROR_SUCCESS) {
		return buffer;
	} else {
		return defaultInt;
	}
}

DWORD HackItSettings::GetRegistryString(HKEY key, LPCTSTR value, LPCTSTR defaultString, LPTSTR target, DWORD size)
{
	DWORD sizeVar;
	DWORD typeVar;
	LONG result;
	char *buffer = new char[size];

	sizeVar = size;
	
	result = RegQueryValueEx(key, value, NULL, &typeVar, (LPBYTE)buffer, &sizeVar);

	if (result == ERROR_SUCCESS) {
		strncpy(target, buffer, size);
	} else {
		strncpy(target, defaultString, size);
	}
	delete [] buffer;
	return size;
}


BOOL HackItSettings::WriteRegistryInt(HKEY key, LPCTSTR value, int intValue)
{
	DWORD sizeVar = sizeof(int);
	DWORD typeVar = REG_DWORD;
	LONG result;

	result = RegSetValueEx(key, value, NULL, typeVar, (LPBYTE)&intValue, sizeVar);

	if (result == ERROR_SUCCESS) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL HackItSettings::WriteRegistryString(HKEY key, LPCTSTR value, LPCTSTR stringValue)
{
	DWORD sizeVar = strlen(stringValue);
	DWORD typeVar = REG_SZ;
	LONG result;

	sizeVar = strlen(stringValue) + 1;
	
	result = RegSetValueEx(key, value, NULL, typeVar, (LPBYTE)stringValue, sizeVar);

	if (result == ERROR_SUCCESS) {
		return TRUE;
	} else {
		return FALSE;
	}
}

