#include <Windows.h>
#include "kitchen_sink/kitchen_sink.h"
#include "gfx.h"
int windowed = 0;

typedef HRESULT(__stdcall* t_SetCooperativeLevel)(int a1, HWND hWnd, DWORD dwFlags);
HRESULT (WINAPI *real_DirectDrawCreate)(GUID FAR *lpGUID, void* FAR *lplpDD, IUnknown FAR *pUnkOuter);
t_SetCooperativeLevel real_SetCooperativeLevel = NULL;

// replace a virtual function slot of an object
BOOL HookVTable(void* iface, int entry, FARPROC replace, FARPROC oldfuncPointer, const char* debugname){
    size_t* pVTable = (size_t*)*(size_t*)iface;
    FARPROC oldPtr = (FARPROC)pVTable[entry];
    if (oldPtr == replace)
        return FALSE; // already hooked, re-hooking would be disastrous
    if (!oldPtr)
        return FALSE; // uh, I guess this would be bad too
    DWORD dwOldProt;
    VirtualProtect(&pVTable[entry], sizeof(size_t), PAGE_EXECUTE_READWRITE, &dwOldProt);
    oldfuncPointer = oldPtr;
    pVTable[entry] = (size_t)replace;
    VirtualProtect(&pVTable[entry], sizeof(size_t), dwOldProt, &dwOldProt);
   // FlushInstructionCache(GetCurrentProcess(), NULL, NULL);

    return TRUE;
}

HRESULT WINAPI HK_SetCooperativeLevel(int This,HWND hWnd, DWORD dwFlags) {
    DBG_printfA("Hooked SetCooperativeLevel");
    //dwFlags = 0x808;

    return real_SetCooperativeLevel(This,hWnd, dwFlags);
}

//DirectDrawCreate
HRESULT WINAPI x_DirectDrawCreate(GUID FAR *lpGUID, void* FAR *lplpDD, IUnknown FAR *pUnkOuter) {
    DBG_printfA("Hooked DirectDrawCreate");
    HRESULT result = real_DirectDrawCreate(lpGUID, lplpDD, pUnkOuter);

   // if(!HookVTable(*lplpDD, 0x50, (FARPROC)HK_SetCooperativeLevel, (FARPROC)&real_SetCooperativeLevel, "DBG_SetCooperativeLevel")){
   //     DBG_printfA("Warning: VTable Hook Fail!");
   // }
    DBG_printfA("Hooked VTable");
    return result;
}

void init_gfx(){
    //HotPatch_patch("ddraw.dll", "DirectDrawCreate", 0x0c, x_DirectDrawCreate, (void**)&real_DirectDrawCreate);
}
