#include <metahook.h>

class IFileSystem;

extern HMODULE g_hClientDll;
extern HINSTANCE g_hThisModule, g_hEngineModule;
extern PVOID g_dwEngineBase;
extern DWORD g_dwEngineSize;
extern PVOID g_dwEngineTextBase;
extern DWORD g_dwEngineTextSize;
extern PVOID g_dwEngineDataBase;
extern DWORD g_dwEngineDataSize;
extern PVOID g_dwEngineRdataBase;
extern DWORD g_dwEngineRdataSize;
extern DWORD g_dwEngineBuildnum;
extern int g_iEngineType;
extern IFileSystem *g_pFileSystem;
extern PVOID g_dwClientBase;
extern DWORD g_dwClientSize;
extern bool g_IsClientVGUI2;