#define DM_CREATEDETECTED WM_USER+3
#define DM_DESTROYDETECTED WM_USER+4
#define DM_SETTEXTDETECTED WM_USER+6

extern "C" void __declspec(dllexport) StartCBTHook();
extern "C" void __declspec(dllexport) StopCBTHook();
extern "C" void __declspec(dllexport) StartCallWndProcRetHook();
extern "C" void __declspec(dllexport) StopCallWndProcRetHook();