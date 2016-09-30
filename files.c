#include "files.h"

NTSTATUS openFile(PWCHAR fileName, PHANDLE fileHandle) {
	OBJECT_ATTRIBUTES fileAttributes;
	IO_STATUS_BLOCK statusBlock;
	NTSTATUS status;
	UNICODE_STRING fileNameUncdStr;

	status = STATUS_SUCCESS;

	RtlInitUnicodeString(&fileNameUncdStr, fileName);
	InitializeObjectAttributes(&fileAttributes, &fileNameUncdStr,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	status = ZwCreateFile(fileHandle, FILE_READ_DATA | FILE_WRITE_DATA, &fileAttributes,
		&statusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE, NULL, 0);

	if (!NT_SUCCESS(status)) {
		TRACE("Error in ZwCreateFile, ret - %x, status code %x, information %x", status, statusBlock.Status, statusBlock.Information);
		status = statusBlock.Status;
	}

	return status;
}

NTSTATUS writeFile(HANDLE fileHandle, PVOID buffer, PLARGE_INTEGER offset, ULONG length) {
	NTSTATUS status;
	IO_STATUS_BLOCK statusBlock;

	status = STATUS_SUCCESS;

	status = ZwWriteFile(fileHandle, NULL, NULL, NULL, &statusBlock, buffer, length, offset, NULL);

	if (!NT_SUCCESS(status)) {
		TRACE("Error in ZwWriteFile, code %x", status);
	}

	return status;
}