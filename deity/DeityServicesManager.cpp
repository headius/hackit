// DeityServicesManager.cpp: implementation of the DeityServicesManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeityServicesManager.h"
#include "WindowListService.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WindowListService *g_WLS = NULL;

DeityServicesManager::DeityServicesManager()
{
	// set up service dispatch table
	m_DispatchTable = new SERVICE_TABLE_ENTRY[2];

	m_DispatchTable[0].lpServiceName = WINLIST_SERVICE_NAME;
	m_DispatchTable[0].lpServiceProc = WindowListService::ServiceStart;

	m_DispatchTable[1].lpServiceName = NULL;
	m_DispatchTable[1].lpServiceProc = NULL;

	// instantiate services
	m_WindowListService = new WindowListService();
	g_WLS = m_WindowListService;
}

DeityServicesManager::~DeityServicesManager()
{
	// free memory
	delete [] m_DispatchTable;
	delete m_WindowListService;
	g_WLS = NULL;
}

void DeityServicesManager::RunServices(int argc, char **argv)
{
	if (!StartServiceCtrlDispatcher(m_DispatchTable)) {
		MessageBox(NULL, "test", "start service dispatcher failed\n", MB_OK);
	}
}
