#include "../fsl_efi.h"

/* THIS IS SUPPOSEDLY PROVIDED TO YOU BUT W.E */
EFI_GUID gEfiBlockIoProtocolGuid = {
    0x964e5b21, 0x6459, 0x11d2,
    {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}
};

public fn list_all_storage_drives()
{
    EFI_STATUS st;
    EFI_HANDLE *handles = NULL;
    UINTN count = 0;

    println(L"[+] Enumerating handles for Block IO...");
    st = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiBlockIoProtocolGuid,
        NULL,
        &count,
        &handles
    );

    if(EFI_ERROR(st) || count == 0) {
        println(L"[-] No handles found");
    }

    for(UINTN i = 0; i < count; i++) {
        EFI_BLOCK_IO_PROTOCOL *blk = NULL;

        st = gBS->HandleProtocol(
            handles[i],
            &gEfiBlockIoProtocolGuid,
            (VOID **)&blk
        );

        if(EFI_ERROR(st) || !blk || !blk->Media)
            continue;

        UINT64 total_bytes = blk->Media->BlockSize * ((UINT64)blk->Media->LastBlock + 1);
        UINT64 size_kb = total_bytes / 1024;
        UINT64 size_mb = total_bytes / (1024 * 1024);
        double size_gb = total_bytes / (1024.0 * 1024.0 * 1024.0);
        print(L"[ "), PrintU32(i), print(L" ]: BlockSize -> "), PrintDouble(size_gb);
        println(NULL);
    }
}

public EFI_BLOCK_IO_PROTOCOL *usb_find_raw_block(void)
{
    EFI_STATUS st;
    EFI_HANDLE *handles = NULL;
    UINTN count = 0;

    st = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiBlockIoProtocolGuid,
        NULL,
        &count,
        &handles
    );

    if(EFI_ERROR(st) || count == 0) {
        println(L"[-] No handles found");
        return NULL;
    }

    for(UINTN i = 0; i < count; i++) {
        EFI_BLOCK_IO_PROTOCOL *blk = NULL;

        st = gBS->HandleProtocol(
            handles[i],
            &gEfiBlockIoProtocolGuid,
            (VOID **)&blk
        );

        if(EFI_ERROR(st) || !blk || !blk->Media)
            continue;

        if(blk->Media->MediaPresent && blk->Media->RemovableMedia && !blk->Media->LogicalPartition)
            return blk;
    }
    
    return NULL;
}

public EFI_STATUS usb_read_lba(EFI_BLOCK_IO_PROTOCOL *blk, UINT64 lba, UINTN blocks, VOID **out_buf)
{
    if(!blk || !out_buf || blocks == 0)
        return EFI_INVALID_PARAMETER;

    UINTN size = blocks * blk->Media->BlockSize;

    EFI_STATUS st = gBS->AllocatePool(EfiLoaderData, size, out_buf);
    if(EFI_ERROR(st))
        return st;

    st = blk->ReadBlocks(blk, blk->Media->MediaId, lba, size, *out_buf);
    if(EFI_ERROR(st)) {
        gBS->FreePool(*out_buf);
        *out_buf = NULL;
    }

    return st;
}

EFI_STATUS usb_write_lba(EFI_BLOCK_IO_PROTOCOL *blk, UINT64 lba, UINTN blocks, VOID *in_buf)
{
    if(!blk || !in_buf || blocks == 0)
        return EFI_INVALID_PARAMETER;

    UINTN size = blocks * blk->Media->BlockSize;
    return blk->WriteBlocks(blk, blk->Media->MediaId, lba, size, in_buf);
}

public fn hex_dump(const UINT8 *buf, UINTN size)
{
    for(UINTN i = 0, nl = 0; i < size; i++) {
        u16 BUFF[3];
        byte_to_hex(buf[i], BUFF);
        print(BUFF);
        print(L", ");
        if (i == nl + 30) {
            i += 30;
            println(NULL);
        }
    }

    if(size % 16)
        println(NULL);
}
