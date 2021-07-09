#include <Windows.h>

#define EXPORTABLE __declspec( dllexport ) 
EXPORTABLE unsigned int Check_Dog() { return 0; }

unsigned int Password_number_table[] = { 1027,2027,3027,2027 };

EXPORTABLE unsigned int Read_Dog(unsigned long password) { return 0; }

EXPORTABLE unsigned int Read_Language() { return 1; /* could be 0-3 */ }