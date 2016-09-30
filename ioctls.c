#include "ioctls.h"
#include <mountmgr.h>
#include <mountdev.h>
#include <ntddvol.h>
#include <ntdddisk.h>
#include "mount.h"
#include "queries.h"
#include "files.h"

CHAR *volumeMemory;

NTSTATUS DispatchNotImplemented(PDEVICE_OBJECT pDeviceObj, PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	status = STATUS_NOT_IMPLEMENTED;

	TRACE("In DispatchNotImplemented. MJ = %x, MN = %x", pStackLocation->MajorFunction, pStackLocation->MinorFunction);

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

void initDispatchTable(PDRIVER_OBJECT pDriverObj) {
	UINT32 i;

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		pDriverObj->MajorFunction[i] = DispatchNotImplemented;
	}

	pDriverObj->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoControl;
	pDriverObj->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
	pDriverObj->MajorFunction[IRP_MJ_READ] = DispatchRead;

	volumeMemory = ExAllocatePoolWithTag(PagedPool, 0x800000, 0xAAAA);
	RtlZeroMemory(volumeMemory, 0x800000);
}

NTSTATUS DispatchCreate(PDEVICE_OBJECT pDeviceObj, PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	status = STATUS_SUCCESS;

	TRACE("In DispatchCreate");

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT pDeviceObj, PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	status = STATUS_SUCCESS;

	TRACE("In DispatchClose");

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchRead(PDEVICE_OBJECT pDeviceObj, PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	ULONG offset;
	CHAR *systemBuffer;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.Read.Length;
	offset = pStackLocation->Parameters.Read.ByteOffset.QuadPart;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	if (systemBuffer == NULL) {
		if (irp->MdlAddress) {
			systemBuffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
		}
	}

	TRACE("In DispatchRead. Systen buffer - %x, offset - %x, length - %x", systemBuffer, offset, outputBufferLength);

	if (systemBuffer && (offset + outputBufferLength < 0x800000)) {
		RtlCopyMemory(systemBuffer, volumeMemory + offset, outputBufferLength);
		status = STATUS_SUCCESS;
		irp->IoStatus.Information = outputBufferLength;
	}
	else {
		status = STATUS_ACCESS_VIOLATION;
		irp->IoStatus.Information = 0;
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchWrite(PDEVICE_OBJECT pDeviceObj, PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	ULONG offset;
	CHAR *systemBuffer;

	HANDLE fileHandle;
	NTSTATUS status2;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.Write.Length;
	offset = pStackLocation->Parameters.Write.ByteOffset.QuadPart;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	if (systemBuffer == NULL) {
		if (irp->MdlAddress) {
			systemBuffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
		}
	}

	TRACE("In DispatchWrite. Systen buffer - %x, offset - %x, length - %x", systemBuffer, offset, outputBufferLength);

	if (systemBuffer && (offset + outputBufferLength < 0x800000)) {
		RtlCopyMemory(volumeMemory + offset, systemBuffer, outputBufferLength);
		status = STATUS_SUCCESS;
		irp->IoStatus.Information = outputBufferLength;
	}
	else {
		status = STATUS_ACCESS_VIOLATION;
		irp->IoStatus.Information = 0;
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	/*status2 = openFile(L"\\SystemRoot\\virtualFs.bin", &fileHandle);

	if (NT_SUCCESS(status2)) {
		writeFile(fileHandle, volumeMemory, NULL, 0x700000);
		ZwClose(fileHandle);
	}*/

	return status;
}

NTSTATUS DispatchIoControl(PDEVICE_OBJECT pDeviceObj, PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG ioCode;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	ioCode = pStackLocation->Parameters.DeviceIoControl.IoControlCode;

	switch(DEVICE_TYPE_FROM_CTL_CODE(ioCode)) {
	case MOUNTDEVCONTROLTYPE:
		return handleMountIrp(irp);
	case IOCTL_VOLUME_BASE:
	case IOCTL_DISK_BASE:
		return handleQueryIrp(irp);
	default:
		TRACE("In DispatchIoControl. Not implemented IO Code - %x", ioCode);
		status = STATUS_NOT_IMPLEMENTED;
		break;
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}