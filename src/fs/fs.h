#pragma once

#ifndef _FSL_EFI_FS_H
#define _FSL_EFI_FS_H

#include <efi.h>
#include <efilib.h>

typedef struct
{
    EFI_BLOCK_IO_PROTOCOL   Handle;
    i32                     DriveSize;
} _storage_drive;

typedef _storage_drive storage_Drive;
typedef _storage_drive *drive_t;
typedef _storage_drive **drives_t;

typedef struct
{
    drives_t drives;
    int len;
} fs_t;

#endif