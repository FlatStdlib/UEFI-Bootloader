#pragma once

#include <efi.h>
#include <efilib.h>
#include "libc/efi_libc.h"

#ifndef _FSL_EFI_H
    #define _FSL_EFI_H
    #define _FSL_EFI_FS_H
#endif

extern EFI_HANDLE gImage;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;

typedef struct {
    int x, y;
} _cordination;

typedef _cordination position;
typedef _cordination cursor_pos_t;

typedef struct {
        map_t           variables;
        i32             var_len;
        cursor_pos_t    cursor;
} fsl_efi;

extern fsl_efi *_FSLEFI_;
public fn EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable, EFI_HANDLE ImageHandle);
public fn read_usb_drive();
public fn input_strip(const string buff, int *size);
private inline UINT64 rdtsc(void);
public fn blink_cursor();
public string  get_line(const string buffer);
public fn fsl_cli();

/* [ System Variables ] variable.c */

/*
*   [ HDD / USB Storage Drive Manager ]
*   Path: ./fs/*
*/
#ifdef _FSL_EFI_FS_H

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

public EFI_BLOCK_IO_PROTOCOL *usb_find_raw_block(void);
public EFI_STATUS usb_read_lba(EFI_BLOCK_IO_PROTOCOL *blk, UINT64 lba, UINTN blocks, VOID **out_buf);
public fn hex_dump(const UINT8 *buf, UINTN size);

#endif