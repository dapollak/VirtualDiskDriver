#pragma once
#include "defs.h"

DRIVER_DISPATCH DispatchRead;
DRIVER_DISPATCH DispatchWrite;
DRIVER_DISPATCH DispatchIoControl;
DRIVER_DISPATCH DispatchCreate;
DRIVER_DISPATCH DispatchClose;
DRIVER_DISPATCH DispatchNotImplemented;

void initDispatchTable(PDRIVER_OBJECT pDriverObj);