#include "fsl_efi.h"

EFI_BOOT_SERVICES *gBS  = NULL;
EFI_SYSTEM_TABLE *gST   = NULL;
EFI_HANDLE gImage       = {0};
fsl_efi *_FSLEFI_       = NULL;
u16 *SYSTEM_USER_NAME   = NULL;
EFI_GRAPHICS_OUTPUT_PROTOCOL *vGop = NULL;

CHAR16 BANNER[] = L"These commands are provided by the OS!\r\n"
                L"     Name          Description\r\n"
                L"__________________________________________\r\n"
                L"     help          List of help commands\r\n"
                L"     ls            List Files\r\n"
                L"     hdd           Hard Drive Info\r\n"
                L"     drives        List Drives\r\n"
                L"     set           Set a system variable\r\n"
                L"     echo          Echo a system variable\r\n"
                L"     list          List All System Variables\r\n";

public fn fsl_cli();
__declspec(dllexport) public fn EFIAPI Init_EFI(EFI_SYSTEM_TABLE *SystemTable, EFI_HANDLE ImageHandle);

public fn EFIAPI Init_EFI(EFI_SYSTEM_TABLE *SystemTable, EFI_HANDLE ImageHandle)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"[ + ] Initializing UEFI.....\r\n");
    gST = SystemTable;
    gImage = ImageHandle;
    gBS = SystemTable->BootServices;
    Init_FSL();
}

public fn Init_FSL()
{
    set_heap_sz(_HEAP_PAGE_ * 10);
    init_mem();
    _FSLEFI_ = allocate(0, sizeof(fsl_efi) + 1);
    _FSLEFI_->variables = init_map();
    _FSLEFI_->var_len = 0;
    _FSLEFI_->cursor = (_cordination){0};
    init_gfb(_FSLEFI_);
    
    println(L"[ + ] FSL EFI Initialized....");
    print(L"[ + ] Heap initialized with "), PrintU32(_HEAP_PAGE_ * 10), println(L" bytes...");
    println(L"[ + ] Initializing main drive....");
    _FSLEFI_->hdd_handle = init_fs();
    if(!_FSLEFI_->hdd_handle)
        fsl_panic(L"Unable to fetch main drive...!");

    /* Saving this for later */
    // write_to_file(_FSLEFI_->hdd_handle, L"testing.txt", "Hello write from UEFI!", 22);
    

    /* TODO; An actual PC user system */
    SYSTEM_USER_NAME = get_line(L"Username: ");
    if(!SYSTEM_USER_NAME)
        return;

    int sz = str_len(SYSTEM_USER_NAME);
    input_strip(SYSTEM_USER_NAME, &sz);
    print(L"[ + ] Screen Resolution: "), PrintU32(_FSLEFI_->resolution.x), print(L"/"), PrintU32(_FSLEFI_->resolution.y), println(NULL);

    init_fsl_theme();
    place_bold_text(80, 300, 8, 8, 0x00ff0000, 0x00535f46, SYSTEM_USER_NAME);
}

public fn init_gfb(fsl_efi *fsl)
{
    EFI_GUID gEfiGraphicsOutputProtocolGuid =
    { 0x9042a9de, 0x23dc, 0x4a38,
      { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } };
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;

    Status = gBS->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        (VOID **)&Gop
    );

    if(EFI_ERROR(Status)) {
        fsl_panic(L"GOP not found");
        return;
    }

    UINT32 BestMode = 0;
    UINTN MaxPixels = 0;

    for(UINT32 i = 0; i < Gop->Mode->MaxMode; i++) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
        UINTN Size;

        if(!EFI_ERROR(Gop->QueryMode(Gop, i, &Size, &Info))) {
            UINTN Pixels = Info->HorizontalResolution * Info->VerticalResolution;
            if (Pixels > MaxPixels) {
                MaxPixels = Pixels;
                BestMode = i;
            }
        }
    }

    Gop->SetMode(Gop, BestMode);

    UINT32 *fb = (UINT32 *)Gop->Mode->FrameBufferBase;
    fsl->framebuffer = fb;
    UINTN pixels = Gop->Mode->FrameBufferSize / 4;

    /* Might not be needed */
    for(UINTN i = 0; i < pixels; i++)
        fb[i] = 0x00000000;

    if(__FSL_DEBUG__)
    {
        print(L"GOP Enabled, Resolution Size: "),
        PrintU32(Gop->Mode->Info->HorizontalResolution), print(L":"),
        PrintU32(Gop->Mode->Info->VerticalResolution), println(NULL);
        print(L"RGB Format: "), PrintU32(Gop->Mode->Info->PixelFormat), println(NULL);
    }

    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    gST->ConOut->ClearScreen(gST->ConOut);
    fsl->resolution = (screen_size){
        .x = Gop->Mode->Info->HorizontalResolution,
        .y = Gop->Mode->Info->VerticalResolution
    };

    /* Might not be needed */
    UINTN stride = Gop->Mode->Info->PixelsPerScanLine;
    vGop = Gop;
    int base_x = 50;
    int base_y = 50;

    // Clear
    for(int row=0; row<8; row++)
        for(int col=0; col<8; col++)
            _FSLEFI_->framebuffer[(base_y + row) * stride + (base_x + col)] = 0x00000000;
}

public fn input_strip(const string buff, int *size)
{
    if(!buff)
        return;

    if(buff[*size] == L'\n' || buff[*size] == L'\r')
        buff[*size] = '\0', (*size)--;

    if(buff[*size] == L'\n' || buff[*size] == L'\r')
        buff[*size] = '\0', (*size)--;
}

public string get_line(const string buffer)
{
    print(buffer);
    EFI_INPUT_KEY Key;
    u16 *buff = allocate(0, 1024);
    int len = 0;

    while(1)
    {
        u16 ret = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        if(len >= 1023)
            break;

        if(ret == EFI_SUCCESS)
        {
            if(Key.UnicodeChar == L'\r')
                break;

            if(Key.UnicodeChar != 0 && is_ascii(Key.UnicodeChar))
            {
	            buff[len++] = Key.UnicodeChar;
                printc(Key.UnicodeChar);
			}
        }
    }

    if(len == 0)
        return NULL;

    return buff;
}