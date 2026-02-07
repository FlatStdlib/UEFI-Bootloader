#include "init.h"


EFI_BLOCK_IO_PROTOCOL *usb_find_raw_block(void)
{
    EFI_HANDLE *handles = NULL;
    UINTN count = 0;

    EFI_STATUS st = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiBlockIoProtocolGuid,
        NULL,
        &count,
        &handles
    );

    if (EFI_ERROR(st))
        return NULL;

    for (UINTN i = 0; i < count; i++) {

        EFI_BLOCK_IO_PROTOCOL *blk = NULL;

        st = gBS->HandleProtocol(
            handles[i],
            &gEfiBlockIoProtocolGuid,
            (VOID **)&blk
        );

        if (EFI_ERROR(st))
            continue;

        if (!blk->Media->MediaPresent)
            continue;

        if (!blk->Media->RemovableMedia)
            continue;

        if (blk->Media->LogicalPartition)
            continue;

        return blk;   // first match
    }

    return NULL;
}

EFI_STATUS usb_read_lba(EFI_BLOCK_IO_PROTOCOL *blk, UINT64 lba, UINTN blocks, VOID **out_buf)
{
    if (!blk || !out_buf || blocks == 0)
        return EFI_INVALID_PARAMETER;

    UINTN size = blocks * blk->Media->BlockSize;

    EFI_STATUS st = gBS->AllocatePool(
        EfiLoaderData,
        size,
        out_buf
    );

    if (EFI_ERROR(st))
        return st;

    st = blk->ReadBlocks(
        blk,
        blk->Media->MediaId,
        lba,
        size,
        *out_buf
    );

    if (EFI_ERROR(st)) {
        gBS->FreePool(*out_buf);
        *out_buf = NULL;
    }

    return st;
}

void hex_dump(const UINT8 *buf, UINTN size)
{
    for (UINTN i = 0; i < size; i++) {
        u16 BUFF[3];
        byte_to_hex(buf[i], BUFF);
        print(BUFF), print(L", ");
        if ((i + 1) % 16 == 0)
            print(L"\n");
    }

    if (size % 16)
        print(L"\n");
}