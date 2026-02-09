#pragma once

#include "libc/efi_libc.h"
#include "fs/fs.h"

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

typedef struct
{
    string  name;
    string  cmd;
    i32     argc;
    string  err;
} _cmd;

typedef _cmd *cmd_t;
typedef _cmd **cmds_t;

typedef struct {
    /* Main HDD/USB FS */
    drive_t         hdd_handle;

    /* CLI Stuff */
    cursor_pos_t    cursor;

    /* System Variables */
    map_t           variables;
    i32             var_len;

    /* Commands */
    cmds_t          commands;
    i32             cmd_len;
} fsl_efi;

extern fsl_efi *_FSLEFI_;
public fn EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable, EFI_HANDLE ImageHandle);
public fn switch_to_gui_mode();
public fn read_usb_drive();
public fn input_strip(const string buff, int *size);
private inline UINT64 rdtsc(void);
public fn blink_cursor();
public string  get_line(const string buffer);
public fn fsl_cli();