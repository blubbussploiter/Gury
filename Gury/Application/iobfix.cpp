#include <iostream>

#pragma comment(lib, "legacy_stdio_definitions.lib")

FILE _iob[] = { *stdin, *stdout, *stderr };
FILE * __cdecl __iob_func(void) { return _iob; }