// LoggingSystem.cpp: implementation of the LoggingSystem class.
//
//////////////////////////////////////////////////////////////////////

#include <time.h>
#include <io.h>
#include "LoggingSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
FILE *LoggingSystem::file;

LoggingSystem::LoggingSystem()
{
}

LoggingSystem::~LoggingSystem()
{

}

void LoggingSystem::log(string astring)
{
#ifdef DEBUG
	if (file == NULL) {
		file = fopen("HackIt.log", "w");
	}
	
	time_t myTime = time(NULL);
	
	fprintf(file, "%s\n", astring.c_str());
	fflush(file);
#endif
}

void LoggingSystem::log(const char* astring)
{
#ifdef DEBUG
	if (file == NULL) {
		file = fopen("HackIt.log", "w");
	}
	
	time_t myTime = time(NULL);
	
	fprintf(file, "%s\n", astring);
	fflush(file);
#endif
}
