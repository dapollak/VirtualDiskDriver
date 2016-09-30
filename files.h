#pragma once
#include "defs.h"

NTSTATUS openFile(PWCHAR fileName, PHANDLE fileHandle);
NTSTATUS writeFile(HANDLE fileHandle, PVOID buffer, PLARGE_INTEGER offset, ULONG length);