// deity.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DeityServicesManager.h"

DeityServicesManager *g_DSM;

void InstallServices(int argc, char** argv);
void UninstallServices();

int main(int argc, char* argv[])
{
	// check for install/uninstall on command line
	if (argc >= 2) {
		if (stricmp(argv[1], "/install") == 0) {
			InstallServices(argc, argv);
		} else if (stricmp(argv[1], "/uninstall") == 0) {
			UninstallServices();
		} else {
			printf("Invalid command line. Valid options are:\n/install\tInstalls services in the service manager\n/uninstall\tUninstalls services from the service manager.\n\n");
		}
		return 0;
	}
	
	g_DSM = new DeityServicesManager();
	
	g_DSM->RunServices(argc, argv);
	
	delete g_DSM;
	
	return 0;
}

VOID InstallServices(int argc, char** argv) 
{ 
    char BinaryPathName[256];
	
	GetFullPathName(argv[0], 256, BinaryPathName, NULL); 
	
	SC_HANDLE schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
	
	if (schSCManager == NULL) 
		printf("failed to get scm manager\n");
	
	SC_HANDLE schService = CreateService( 
        schSCManager,              // SCManager database 
        WINLIST_SERVICE_NAME,              // name of service 
        WINLIST_SERVICE_DISPLAY_NAME,           // service name to display 
        GENERIC_EXECUTE,        // desired access 
        SERVICE_WIN32_SHARE_PROCESS | SERVICE_INTERACTIVE_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        BinaryPathName,        // service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 
	
    if (schService == NULL) 
        printf("CreateService FAILED.\n"); 
    else 
        printf("CreateService SUCCESS.\n"); 
	
    CloseServiceHandle(schService); 
}

void UninstallServices() {
	SC_HANDLE schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
	
	if (schSCManager == NULL) 
		printf("failed to get scm manager\n");
	
	SC_HANDLE schService = OpenService( 
        schSCManager,              // SCManager database 
        WINLIST_SERVICE_NAME,              // name of service 
        SERVICE_ALL_ACCESS);
	
	DeleteService(schService);
	
    if (schService == NULL) 
        printf("OpenService FAILED.\n"); 
    else 
        printf("OpenService SUCCESS.\n"); 
	
    CloseServiceHandle(schService); 
}
