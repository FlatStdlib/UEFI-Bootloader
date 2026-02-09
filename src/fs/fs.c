#include "fs.h"

public drive_t init_fs()
{
    EFI_GUID gEfiBlockIoProtocolGuid = {
        0x964e5b21, 0x6459, 0x11d2,
        {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}
    };

    EFI_GUID gEfiSimpleFileSystemProtocolGuid = 
    { 0x0964e5b22, 0x6459, 0x11d2, {0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b} };

    EFI_STATUS st;
    EFI_HANDLE *handles = NULL;
    UINTN count = 0;

    fs_t drive = allocate(0, sizeof(fs_t));
    if(!drive) return NULL;

    st = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiBlockIoProtocolGuid,
        NULL,
        &count,
        &handles
    );

    if(EFI_ERROR(st) || count == 0) {
        println(L"[-] No drives found");
        pfree(drive, 1);
        return NULL;
    }

    double max_size = 0;
    for(UINTN i = 0; i < count; i++) {
        EFI_BLOCK_IO_PROTOCOL *blk = NULL;
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;

        st = gBS->HandleProtocol(handles[i], &gEfiBlockIoProtocolGuid, (VOID **)&blk);
        if(EFI_ERROR(st) || !blk || !blk->Media) continue;

        UINT64 total_bytes = blk->Media->BlockSize * ((UINT64)blk->Media->LastBlock + 1);
        double size_gb = total_bytes / (1024.0 * 1024.0 * 1024.0);
        if(size_gb < 28.00f || size_gb > 29.00f) {
            continue;
        }

        st = gBS->HandleProtocol(handles[i], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&fs);
        if(EFI_ERROR(st)) continue;
            
        drive->Handle = blk;
        drive->fs = fs;

        st = drive->fs->OpenVolume(drive->fs, &drive->root);
        if(EFI_ERROR(st)) {
            fsl_panic(L"Failed to open root folder...");
            continue;
        }

        drive->Blocks = blk->Media->LastBlock + 1;
        drive->BlockSize = blk->Media->BlockSize;
        drive->DriveSize = drive->Blocks * drive->BlockSize;

        print(L"[ + ] Main HDD: "), PrintU32(size_gb), println(L" GB");
        break;
    }

    if(!drive->Handle)
    {
        pfree(drive, 1);
        return NULL;
    }

    return drive;
}

public string list_dir(drive_t drive)
{
    UINTN buf_size = 1024;
    EFI_FILE_INFO *info = allocate(0, buf_size);

    while (TRUE)
    {
        buf_size = 1024;
        EFI_STATUS st = drive->root->Read(drive->root, &buf_size, info);
        if(EFI_ERROR(st) || buf_size == 0)
            break;
        
        print(info->FileName), print(L": "), PrintU64(info->FileSize), println(NULL);
    }

    pfree(info, 1);
}

public fn write_to_file(drive_t drive, CHAR16 *filename, u8 *data, UINTN size)
{
    EFI_STATUS st;

    st = drive->fs->OpenVolume(drive->fs, &drive->root);
    if (EFI_ERROR(st)) {
        fsl_panic(L"Cannot open volume");
        return;
    }

    EFI_FILE_PROTOCOL *file;
    st = drive->root->Open(
        drive->root,
        &file,
        filename,
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
        0
    );
    
    if(EFI_ERROR(st)) {
        fsl_panic(L"Failed to create file on root");
        return;
    }

    st = file->Write(file, &size, data);
    if(EFI_ERROR(st)) {
        fsl_panic(L"Failed to write file");
        file->Close(file);
        return;
    }

    file->Flush(file);
    file->Close(file);
    println(L"File written successfully!\n");
}