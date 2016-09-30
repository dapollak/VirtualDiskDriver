#include "defs.h"
#include "ioctls.h"
#include <mountmgr.h>

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(INIT, DriverUnload)

PDEVICE_OBJECT pPdo = NULL;
PDEVICE_OBJECT pFdo = NULL;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING  pRegistryPath) {
	NTSTATUS status;
	PDEVICE_OBJECT retVal;
	UNICODE_STRING devName, registrationSymLink;
	UNREFERENCED_PARAMETER(pRegistryPath);

	TRACE("Driver Entry");

	status = STATUS_SUCCESS;
	pDriverObject->DriverUnload = DriverUnload;

	// create PDO
	status = IoReportDetectedDevice(pDriverObject, InterfaceTypeUndefined, -1, -1, NULL, NULL, FALSE, &pPdo);

	if (!NT_SUCCESS(status)) {
		TRACE("Error in IoReportDetectedDevice, code %x", status);
		status = STATUS_UNSUCCESSFUL;
		goto clean1;
	}

	// create FDO

	RtlInitUnicodeString(&devName, L"\\Device\\VirtVol");

	status = IoCreateDevice(pDriverObject, 0, &devName, FILE_DEVICE_DISK, 0, FALSE, &pFdo);

	if (!NT_SUCCESS(status)) {
		TRACE("Error in IoCreateDevice, code %x", status);
		status = STATUS_UNSUCCESSFUL;
		goto clean1;
	}

	pFdo->Flags |= DO_BUFFERED_IO;
	pFdo->Flags &= (~DO_DEVICE_INITIALIZING);

	// attach FDO to PDO

	retVal = IoAttachDeviceToDeviceStack(pFdo, pPdo);

	if (retVal == NULL) {
		TRACE("Error in IoAttachDeviceToDeviceStack");
		status = STATUS_UNSUCCESSFUL;
		goto clean2;
	}

	// Ask for mount
	status = IoRegisterDeviceInterface(pPdo, &MOUNTDEV_MOUNTED_DEVICE_GUID, NULL, &registrationSymLink);

	if (!NT_SUCCESS(status)) {
		TRACE("Error in IoRegisterDeviceInterface, code %x", status);
		status = STATUS_UNSUCCESSFUL;
		goto clean3;
	}

	status = IoSetDeviceInterfaceState(&registrationSymLink, TRUE);

	if (!NT_SUCCESS(status)) {
		TRACE("Error in IoSetDeviceInterfaceState, code %x", status);
		status = STATUS_UNSUCCESSFUL;
		goto clean3;
	}

	initDispatchTable(pDriverObject);

	goto exit;

clean3:
	IoDetachDevice(pPdo);
clean2:
	IoDeleteDevice(pFdo);
clean1:
	IoDeleteDevice(pPdo);
exit:
	return status;
}

VOID DriverUnload(PDRIVER_OBJECT  DriverObject) {
	TRACE("Driver Unload");
	IoDeleteDevice(pPdo);
	IoDeleteDevice(pFdo);
}




