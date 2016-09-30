#pragma once

#include <Ntddk.h>
#include <initguid.h>

#define TRACE(format, ...) DbgPrint("Driver Trace %d => " format "\n", PsGetCurrentThreadId(), __VA_ARGS__)
#define MAKE_LARGE_INTEGER(high, low) (LONGLONG)(((high) << sizeof(LONG)) | ((low) & 0xffffffff)))