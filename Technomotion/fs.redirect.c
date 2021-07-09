#include <Windows.h>
#include <winternl.h>
#include "kitchen_sink/kitchen_sink.h"

#include "fs.redirect.h"

typedef NTSTATUS __stdcall tNtCreateFile(PHANDLE FileHandle, DWORD DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
static tNtCreateFile* ntdll_NtCreateFile = 0;
NTSTATUS NTAPI x_NtCreateFile(PHANDLE FileHandle, DWORD DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength) {
    if (ObjectAttributes && ObjectAttributes->ObjectName && ObjectAttributes->ObjectName->Buffer) {
        //DBG_printfW(ObjectAttributes->ObjectName->Buffer);
        if (wcsstr(ObjectAttributes->ObjectName->Buffer,L"c:\\techmo\\BlackBox.tab")) {
                   *FileHandle = CreateFileA(".\\BlackBox.tab", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,NULL);
                   return 0;
        }
    }

return ntdll_NtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
}

void init_fs_redirect(){
    HotPatch_patch("ntdll.dll", "NtCreateFile", 0x10, x_NtCreateFile, (void**)&ntdll_NtCreateFile);
}