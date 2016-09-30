#include "mount.h"
#include <mountmgr.h>
#include <mountdev.h>

NTSTATUS handleMountIrp(PIRP irp) {
	PIO_STACK_LOCATION pStackLocation;
	ULONG ioCode;
	NTSTATUS status;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	ioCode = pStackLocation->Parameters.DeviceIoControl.IoControlCode;

	switch (ioCode) {
	case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
		return handleQueryDeviceName(irp);
	case IOCTL_MOUNTDEV_QUERY_UNIQUE_ID:
		return handleQueryUniqueId(irp);
	//case IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME:
	//	return handleQueryLinkName(irp);
	}

	TRACE("In handleMountIrp. Not implemented IO Code - %x", ioCode);
	status = STATUS_NOT_IMPLEMENTED;

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryDeviceName(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	PMOUNTDEV_NAME systemBuffer;
	UNICODE_STRING devName;

	RtlInitUnicodeString(&devName, L"\\Device\\VirtVol");

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	TRACE("In handleQueryDeviceName. Buffer length - %x", outputBufferLength);

	if (outputBufferLength < sizeof(MOUNTDEV_NAME)) {
		// buffer can't even contain length of name
		status = STATUS_INVALID_PARAMETER;
	}
	else {
		systemBuffer->NameLength = devName.Length;

		if (outputBufferLength < sizeof(USHORT) + devName.Length) {
			// buffer can't contain full data. only length
			irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
			status = STATUS_BUFFER_OVERFLOW;
		}
		else {
			// buffer size is enough.
			irp->IoStatus.Information = sizeof(USHORT) + devName.Length;
			RtlCopyMemory(systemBuffer->Name, devName.Buffer, devName.Length);
			status = STATUS_SUCCESS;
		}
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}


NTSTATUS handleQueryUniqueId(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	PMOUNTDEV_UNIQUE_ID systemBuffer;
	ANSI_STRING uniqueId;

	RtlInitAnsiString(&uniqueId, "AABBCCDD");

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	TRACE("In handleQueryUniqueId. Buffer length - %x", outputBufferLength);

	if (outputBufferLength < sizeof(MOUNTDEV_UNIQUE_ID)) {
		// buffer can't even contain length of name
		status = STATUS_INVALID_PARAMETER;
	}
	else {
		systemBuffer->UniqueIdLength = uniqueId.Length;

		if (outputBufferLength < sizeof(USHORT) + uniqueId.Length) {
			// buffer can't contain full data. only length
			irp->IoStatus.Information = sizeof(MOUNTDEV_UNIQUE_ID);
			status = STATUS_BUFFER_OVERFLOW;
		}
		else {
			// buffer size is enough.
			RtlCopyMemory(systemBuffer->UniqueId, uniqueId.Buffer, uniqueId.Length);
			irp->IoStatus.Information = sizeof(USHORT) + uniqueId.Length;
			status = STATUS_SUCCESS;
		}
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryLinkName(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	PMOUNTDEV_SUGGESTED_LINK_NAME systemBuffer;
	UNICODE_STRING linkName;

	RtlInitUnicodeString(&linkName, "\\DosDevices\\V:");

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	TRACE("In handleQueryLinkName. Buffer length - %x", outputBufferLength);

	if (outputBufferLength < sizeof(MOUNTDEV_SUGGESTED_LINK_NAME)) {
		// buffer can't even contain length of name
		status = STATUS_INVALID_PARAMETER;
	}
	else {

		systemBuffer->UseOnlyIfThereAreNoOtherLinks = FALSE;
		systemBuffer->NameLength = linkName.Length*sizeof(WCHAR);

		if (outputBufferLength < sizeof(BOOLEAN) + sizeof(USHORT) + linkName.Length*sizeof(WCHAR)) {
			// buffer can't contain full data. only length
			irp->IoStatus.Information = sizeof(BOOLEAN) + sizeof(USHORT) + linkName.Length*sizeof(WCHAR);
			status = STATUS_BUFFER_OVERFLOW;
		}
		else {
			// buffer size is enough.
			RtlCopyMemory(systemBuffer->Name, linkName.Buffer, linkName.Length*sizeof(WCHAR));
			irp->IoStatus.Information = sizeof(BOOLEAN) + sizeof(USHORT) + linkName.Length*sizeof(WCHAR);
			status = STATUS_SUCCESS;
		}
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}