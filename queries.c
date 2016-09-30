#include "queries.h"
#include <ntddvol.h>
#include <ntdddisk.h>


NTSTATUS handleQueryIrp(PIRP irp) {
	PIO_STACK_LOCATION pStackLocation;
	ULONG ioCode;
	NTSTATUS status;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	ioCode = pStackLocation->Parameters.DeviceIoControl.IoControlCode;

	switch (ioCode) {
	case IOCTL_VOLUME_ONLINE:
		return handleQueryVolumeOnline(irp);
	case IOCTL_VOLUME_GET_GPT_ATTRIBUTES:
		return handleQueryVolumeGptAttributes(irp);
	case IOCTL_DISK_GET_PARTITION_INFO_EX:
		return handleQueryVolumePartitionInfoEx(irp);
	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
		return handleQueryDriverGeometry(irp);
	case IOCTL_DISK_IS_WRITABLE:
		return handleQueryIsDiskWriteable(irp);
	case IOCTL_DISK_GET_LENGTH_INFO:
		return handleQueryVolumeLengthInfo(irp);
	case IOCTL_DISK_VERIFY:
		return handleQueryDiskVerify(irp);
	case IOCTL_DISK_MEDIA_REMOVAL:
		return handleQueryDiskMediaRemoval(irp);
	case IOCTL_DISK_CHECK_VERIFY:
		return handleQueryDiskCheckVerify(irp);
	case IOCTL_DISK_SET_PARTITION_INFO:
		return handleQuerySetPartitionInfo(irp);
	}

	TRACE("In handleMountIrp. Not implemented IO Code - %x", ioCode);
	status = STATUS_NOT_IMPLEMENTED;

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryVolumeOnline(PIRP irp) {
	NTSTATUS status;
	
	TRACE("In handleQueryVolumeOnline");
	status = STATUS_SUCCESS;

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryIsDiskWriteable(PIRP irp) {
	NTSTATUS status;

	TRACE("In handleQueryIsDiskWriteable");
	status = STATUS_SUCCESS;

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryVolumeGptAttributes(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION systemBuffer;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	TRACE("In handleQueryVolumeGptAttributes");

	if (outputBufferLength < sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION)) {
		status = STATUS_INVALID_PARAMETER;
		irp->IoStatus.Information = 0;
	}
	else {
		systemBuffer->GptAttributes = 0;
		status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION);
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryVolumePartitionInfoEx(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	PPARTITION_INFORMATION_EX systemBuffer;
	PPARTITION_INFORMATION_MBR mbrInfo;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	TRACE("In handleQueryVolumePartitionInfoEx. Buffer length - %u", outputBufferLength);

	if (outputBufferLength < sizeof(PARTITION_INFORMATION_EX)) {
		status = STATUS_BUFFER_TOO_SMALL;
		irp->IoStatus.Information = 0;
	}
	else {
		systemBuffer->PartitionStyle = PARTITION_STYLE_MBR;
		systemBuffer->StartingOffset.QuadPart = 128*512;
		systemBuffer->PartitionLength.QuadPart = 0x700000;
		systemBuffer->PartitionNumber = 1;
		systemBuffer->RewritePartition = FALSE;
		
		mbrInfo = &(systemBuffer->Mbr);
		mbrInfo->PartitionType = PARTITION_HUGE;
		mbrInfo->BootIndicator = FALSE;
		mbrInfo->HiddenSectors = 0x80;
		mbrInfo->RecognizedPartition = TRUE;

		status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(PARTITION_INFORMATION_EX);
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryDriverGeometry(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	PDISK_GEOMETRY systemBuffer;
	PPARTITION_INFORMATION_MBR mbrInfo;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	TRACE("In handleQueryDriverGeometry");

	if (outputBufferLength < sizeof(DISK_GEOMETRY)) {
		status = STATUS_BUFFER_TOO_SMALL;
	}
	else {
		systemBuffer->Cylinders.QuadPart = 1;
		systemBuffer->MediaType = FixedMedia;
		systemBuffer->TracksPerCylinder = 0xff;
		systemBuffer->SectorsPerTrack = 0x3f;
		systemBuffer->BytesPerSector = 512;

		status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryVolumeLengthInfo(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	PGET_LENGTH_INFORMATION systemBuffer;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	TRACE("In handleQueryVolumeLengthInfo");

	if (outputBufferLength < sizeof(GET_LENGTH_INFORMATION)) {
		status = STATUS_BUFFER_TOO_SMALL;
	}
	else {
		systemBuffer->Length.QuadPart = 0x700000;
		status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryDiskVerify(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	PVERIFY_INFORMATION systemBuffer;
	ULONGLONG offset;
	ULONG length;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	offset = systemBuffer->StartingOffset.QuadPart;
	length = systemBuffer->Length;

	TRACE("In handleQueryDiskVerify. Offset format - %x, length - %x", offset, length);
	status = STATUS_SUCCESS;

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryDiskMediaRemoval(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	ULONG outputBufferLength;
	PPREVENT_MEDIA_REMOVAL systemBuffer;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	outputBufferLength = pStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	TRACE("In handleQueryDiskMediaRemoval. Prevent - %x", systemBuffer->PreventMediaRemoval);
	status = STATUS_SUCCESS;

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQueryDiskCheckVerify(PIRP irp) {
	NTSTATUS status;

	TRACE("In handleQueryDiskCheckVerify");
	status = STATUS_SUCCESS;

	irp->IoStatus.Status = status;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handleQuerySetPartitionInfo(PIRP irp) {
	NTSTATUS status;
	PIO_STACK_LOCATION pStackLocation;
	PSET_PARTITION_INFORMATION systemBuffer;

	pStackLocation = IoGetCurrentIrpStackLocation(irp);
	systemBuffer = irp->AssociatedIrp.SystemBuffer;

	TRACE("In handleQuerySetPartitionInfo. Set type to %x", systemBuffer->PartitionType);
	status = STATUS_SUCCESS;

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}