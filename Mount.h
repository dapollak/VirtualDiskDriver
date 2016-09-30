#pragma once
#include "defs.h"

NTSTATUS handleMountIrp(PIRP irp);
NTSTATUS handleQueryDeviceName(PIRP irp);
NTSTATUS handleQueryUniqueId(PIRP irp);
NTSTATUS handleQueryLinkName(PIRP irp);