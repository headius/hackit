// DeityServicesManager.h: interface for the DeityServicesManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEITYSERVICESMANAGER_H__0AC25E1E_F855_471E_B4AF_D1A661F511BE__INCLUDED_)
#define AFX_DEITYSERVICESMANAGER_H__0AC25E1E_F855_471E_B4AF_D1A661F511BE__INCLUDED_

#include "WindowListService.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DeityServicesManager  
{
public:
	void RunServices(int argc, char** argv);
	DeityServicesManager();
	virtual ~DeityServicesManager();

protected:
	WindowListService *m_WindowListService;
	SERVICE_TABLE_ENTRY* m_DispatchTable;
};

#endif // !defined(AFX_DEITYSERVICESMANAGER_H__0AC25E1E_F855_471E_B4AF_D1A661F511BE__INCLUDED_)
