#pragma once
#include "defs.h"

/*
 56c008 - IOCTL_VOLUME_ONLINE
 560038 - IOCTL_VOLUME_GET_GPT_ATTRIBUTES
 70048  - IOCTL_DISK_GET_PARTITION_INFO_EX
 4d0018 - IOCTL_MOUNTDEV_QUERY_STABLE_GUID
 70000  - IOCTL_DISK_GET_DRIVE_GEOMETRY
 74804  - IOCTL_DISK_MEDIA_REMOVAL
 70024  - IOCTL_DISK_IS_WRITABLE
 560000 - IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS
 7405c -  IOCTL_DISK_GET_LENGTH_INFO

 */

NTSTATUS handleQueryIrp(PIRP irp);

NTSTATUS handleQueryVolumeOnline(PIRP irp);
NTSTATUS handleQueryVolumeGptAttributes(PIRP irp);
NTSTATUS handleQueryVolumePartitionInfoEx(PIRP irp);
NTSTATUS handleQueryDriverGeometry(PIRP irp);
NTSTATUS handleQueryIsDiskWriteable(PIRP irp);
NTSTATUS handleQueryVolumeDiskExtends(PIRP irp);
NTSTATUS handleQueryVolumeLengthInfo(PIRP irp);
NTSTATUS handleQueryDiskVerify(PIRP irp);
NTSTATUS handleQueryDiskMediaRemoval(PIRP irp);
NTSTATUS handleQueryDiskCheckVerify(PIRP irp);
NTSTATUS handleQuerySetPartitionInfo(PIRP irp);