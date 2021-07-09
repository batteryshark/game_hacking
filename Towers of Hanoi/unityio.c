#include <Windows.h>

#define EXPORTABLE __declspec( dllexport ) 

EXPORTABLE int init_io() { return 1; }

EXPORTABLE void fini_io() {};