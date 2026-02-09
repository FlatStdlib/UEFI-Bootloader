#include "fsl_efi.h"

EFI_BOOT_SERVICES *gBS;
EFI_SYSTEM_TABLE *gST;
EFI_HANDLE gImage;
fsl_efi *_FSLEFI_ = NULL;
u16 *SYSTEM_USER_NAME = NULL;
#define CPU_HZ 3000000000ULL
#define BLINK_INTERVAL (CPU_HZ)

/* Some basic font */
uint8_t font_A[8] = {
    0b00111100,
    0b01000010,
    0b01000010,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00000000
};

CHAR16 BANNER[] = L"These commands are provided by the OS!\r\n"
                L"     Name          Description\r\n"
                L"__________________________________________\r\n"
                L"     help          List of help commands\r\n"
                L"     set           Set a system variable\r\n"
                L"     echo          Echo a system variable\r\n"
                L"     listdr        List all I/O Drives\r\n";

public fn fsl_cli();
__declspec(dllexport) public fn EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable, EFI_HANDLE ImageHandle);

public fn EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable, EFI_HANDLE ImageHandle)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"[ + ] Initializing UEFI.....\r\n");
    gST = SystemTable;
    gImage = ImageHandle;
    gBS = SystemTable->BootServices;
    set_heap_sz(_HEAP_PAGE_ * 10);
    init_mem();
    _FSLEFI_ = allocate(0, sizeof(fsl_efi) + 1);
    _FSLEFI_->variables = init_map();
    _FSLEFI_->var_len = 0;
    _FSLEFI_->cursor = (_cordination){0};
    switch_to_gui_mode(_FSLEFI_);
    
    println(L"[ + ] FSL EFI Initialized....");
    print(L"[ + ] Heap initialized with "), PrintU32(_HEAP_PAGE_ * 10), println(L" bytes...");
    println(L"[ + ] Initializing main drive....");
    _FSLEFI_->hdd_handle = init_fs();
    if(!_FSLEFI_->hdd_handle)
        fsl_panic(L"Unable to fetch main drive...!");

    // write_to_file(_FSLEFI_->hdd_handle, L"testing.txt", "Hello write from UEFI!", 22);
    
    SYSTEM_USER_NAME = get_line(L"Username: ");
    if(!SYSTEM_USER_NAME)
        return;

    print_args((string []){L"\r\nWelcome to FSL OS, ", (string)SYSTEM_USER_NAME, L"\r\n", NULL});

    println(L"[ + ] Loading FSL CLI.....");

    /* A Cursor Blinker thats way blinks way to fast rn, fuck that */
    // EFI_EVENT TimerEvent;
    // gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL,
    //                 TPL_CALLBACK,
    //                 blink_cursor,
    //                 NULL,
    //                 &TimerEvent);

    // gBS->SetTimer(TimerEvent, TimerPeriodic, 5000000);
    clear_screen(_FSLEFI_, 0x00000000);
    
    /* Just some top bar shit */
    for(int y = 0; y < 30; y++)
        for(int x = 0; x < _FSLEFI_->resolution.x; x++)
            draw_pixel(_FSLEFI_, x, y, 0x00211832);
            
    draw_char(_FSLEFI_, 50, 50, font_A, 0x00FF0000);
    // fsl_cli();
}

public fn clear_screen(fsl_efi *fsl, uint32_t color)
{
    UINTN pixels = fsl->resolution.x * fsl->resolution.y;
    for(UINTN i = 0; i < pixels; i++)
        fsl->framebuffer[i] = color;
}

public fn switch_to_gui_mode(fsl_efi *fsl)
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

    for(UINTN i = 0; i < pixels; i++)
        fb[i] = 0x00000000;

    print(L"GOP Enabled, Resolution Size: "),
    PrintU32(Gop->Mode->Info->HorizontalResolution), print(L":"),
    PrintU32(Gop->Mode->Info->VerticalResolution), println(NULL);
    print(L"RGB Format: "), PrintU32(Gop->Mode->Info->PixelFormat), println(NULL);
    fsl->resolution = (screen_size){ 
        .x = Gop->Mode->Info->HorizontalResolution,
        .y = Gop->Mode->Info->VerticalResolution
    };

    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    gST->ConOut->ClearScreen(gST->ConOut);   
}

void draw_char(fsl_efi *fsl, int x, int y, uint8_t *bitmap, uint32_t color) {
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 8; col++) {
            if(bitmap[row] & (1 << (7 - col)))
                draw_pixel(fsl, x + col, y + row, color);
        }
    }
}

void draw_big_pixel(fsl_efi *fsl, int x, int y, uint32_t color, int scale) {
    for(int dy = 0; dy < scale; dy++)
        for(int dx = 0; dx < scale; dx++)
            draw_pixel(fsl, x*scale + dx, y*scale + dy, color);
}

void draw_pixel(fsl_efi *fsl, int x, int y, uint32_t color) {
    fsl->framebuffer[y * fsl->resolution.x + x] = color;
}

// public fn read_usb_drive()
// {
//     println(L"[ + ] USB Reading");

//     EFI_BLOCK_IO_PROTOCOL *blk = usb_find_raw_block();
//     if(!blk) {
//         fsl_panic(L"[ - ] No raw USB block device found\n");
//         return;
//     }
    
//     UINT64 total_bytes = blk->Media->BlockSize * ((UINT64)blk->Media->LastBlock + 1);
//     UINT64 size_kb = total_bytes / 1024;
//     UINT64 size_mb = total_bytes / (1024 * 1024);
//     double size_gb = total_bytes / (1024.0 * 1024.0 * 1024.0);
//     print(L"[ + ] Storage Size: "), PrintDouble(size_gb), print(L"\r\n");

//     /* Debug */
//     // VOID *buf = NULL;
//     // EFI_STATUS st = usb_read_lba(blk, 0, 1, &buf);
//     // if(EFI_ERROR(st)) {
//     //     fsl_panic(L"[-] Read failed\r\n");
//     //     return;
//     // }

//     // println(L"[+] LBA 0 dump (first 64 bytes):\n");
//     // hex_dump((UINT8 *)buf, 64);

//     // gBS->FreePool(buf);
// }

public fn input_strip(const string buff, int *size)
{
    if(!buff)
        return;

    if(buff[*size] == '\n' || buff[*size] == '\r')
        buff[*size] = '\0', (*size)--;

    if(buff[*size] == '\n' || buff[*size] == '\r')
        buff[*size] = '\0', (*size)--;
}

static inline UINT64 rdtsc(void) {
    UINT32 lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((UINT64)hi << 32) | lo;
}

public fn blink_cursor()
{
    UINTN CursorX = gST->ConOut->Mode->CursorColumn;
	UINTN CursorY = gST->ConOut->Mode->CursorRow;
	int visible = 0, last_toggle = 0;
    /* Cursor */
    UINT64 now = rdtsc();

	if((now - last_toggle) >= BLINK_INTERVAL) {
	    last_toggle = now;
	    visible = !visible;

	    if(visible) {
	        gST->ConOut->SetCursorPosition(
		        gST->ConOut,
		        CursorX,
		        CursorY - 1
		    );
	        print_color_text(EFI_WHITE, EFI_GREEN, L"_");
	    } else {
	        gST->ConOut->SetCursorPosition(
		        gST->ConOut,
		        CursorX,
		        CursorY - 1
		    );
	        print_color_text(EFI_WHITE, EFI_BLACK, L" ");
	    }
    }
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

public fn fsl_cli()
{
    println_color_text(EFI_LIGHTGREEN, EFI_BLACK, L"FSL OS's CLI ");
    println_color_text(EFI_LIGHTGREEN, EFI_BLACK, L"Type ? for a list of commands");
    EFI_INPUT_KEY Key;
    u16 CMD[1024];
    memzero(CMD, 1024);
    int len = 0;

    print_color_text_args(EFI_WHITE, EFI_RED, (string []){L"[FSL@", SYSTEM_USER_NAME, L"] ~ #", NULL}), print(L" ");
    while(1)
    {
        u16 ret = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);

        if(ret == EFI_SUCCESS)
        {
            if(Key.UnicodeChar != 0 && is_ascii(Key.UnicodeChar))
            {
	            CMD[len++] = Key.UnicodeChar;
                printc_color_text(EFI_GREEN, EFI_BLACK, CMD[len - 1]);
			}


			if((Key.UnicodeChar == 0x1B || Key.UnicodeChar == L'a') && Key.ScanCode == SCAN_ESC)
                println(L"ESC key detected");

			if(len > 0 && (Key.UnicodeChar == L'\r' || Key.UnicodeChar == L'\n'))
            {
                println(L"\r\n");
                input_strip(CMD, &len);

                int argc = 0;
                sArr args = NULL;

                if(mem_cmp(CMD, L"help", 4))
                {
                    println_color_text(EFI_WHITE, EFI_BLACK, BANNER);
                } else if(mem_cmp(CMD, L"ls", 2)) {
                    println(L"Files:");
                    list_dir(_FSLEFI_->hdd_handle);
                } else if(mem_cmp(CMD, L"hdd", 3)) {
                    print(L"Main Drive Size: "), PrintU32(_FSLEFI_->hdd_handle->DriveSize), println(NULL);
                } else if(mem_cmp(CMD, L"drives", 6)) {
                    println(L"Drives:");
                    list_all_storage_drives();
                } else if(find_string(CMD, L"set") > -1)
                {
                    args = split_string(CMD, ' ', &argc);
                    if(args) {
                        if(map_append(_FSLEFI_->variables, args[1], args[2]))
                            print_color_text_args(EFI_WHITE, EFI_BLACK, (string []){L" Variable set: ", _FSLEFI_->variables->fields[0]->key, L" -> ", _FSLEFI_->variables->fields[0]->value, L"!\r\n", NULL});
                        else
                            println_color_text(EFI_WHITE, EFI_RED, L"Failed to set variable!");

                        pfree_array((array)args);
                    }
                } else if(find_string(CMD, L"list") > -1) {
                    for(int i = 0; _FSLEFI_->variables->fields[i] != NULL; i++)
                        print_args((string []){_FSLEFI_->variables->fields[i]->value, L"\r\n", NULL});
                } else if(find_string(CMD, L"echo") > -1)
                {
                    args = split_string(CMD, ' ', &argc);
                    if(args) {
                        string value = find_key(_FSLEFI_->variables, args[1]);
                        
                        if(!value) {
                            println(L"Unable to find variable!");
                        } else {
                            print_color_text_args(EFI_WHITE, EFI_BLACK, (string []){L"Variable set: ", value, L"!\r\n", NULL});
                        }
                        
                        pfree_array((array)args);
                    }
                }
                
                memzero(CMD, 1024);
                len = 0;
                print_color_text_args(EFI_WHITE, EFI_RED, (string []){L"[FSL@", SYSTEM_USER_NAME, L"] ~ #", NULL}), print(L" ");
            }
        }
    }
}
