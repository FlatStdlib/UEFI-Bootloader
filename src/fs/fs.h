#pragma once

#include "../libc/efi_libc.h"
/* [ System Variables ] variable.c */

/*
*   [ HDD / USB Storage Drive Manager ]
*   Path: ./fs/*
*/
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

public fn list_all_storage_drives();
public EFI_BLOCK_IO_PROTOCOL *usb_find_raw_block(void);
public EFI_STATUS usb_read_lba(EFI_BLOCK_IO_PROTOCOL *blk, UINT64 lba, UINTN blocks, VOID **out_buf);
public fn hex_dump(const UINT8 *buf, UINTN size);