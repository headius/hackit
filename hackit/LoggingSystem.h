// LoggingSystem.h: interface for the LoggingSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGINGSYSTEM_H__702B89D7_1DB6_11D3_8E61_0008C75CA3DB__INCLUDED_)
#define AFX_LOGGINGSYSTEM_H__702B89D7_1DB6_11D3_8E61_0008C75CA3DB__INCLUDED_
#include <string>

using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// logging defines
#define WINDOWLIST_VERBOSE FALSE
#define SETTINGSDLG_VERBOSE FALSE
#define INIT_VERBOSE TRUE
#define LOGGING FALSE

class LoggingSystem  
{
public:
	static void log(string astring);
	static void log(const char* astring);
	LoggingSystem();
	virtual ~LoggingSystem();

private:
	static FILE *file;
};

#endif // !defined(AFX_LOGGINGSYSTEM_H__702B89D7_1DB6_11D3_8E61_0008C75CA3DB__INCLUDED_)
