#include "init.h"

EFI_BOOT_SERVICES *gBS;
EFI_SYSTEM_TABLE *gST;
EFI_HANDLE gImage;
fsl_efi *_FSLEFI_ = NULL;
u16 *SYSTEM_USER_NAME = NULL;
#define CPU_HZ 3000000000ULL
#define BLINK_INTERVAL (CPU_HZ)

public fn fsl_cli();
__declspec(dllexport) public fn EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable, EFI_HANDLE ImageHandle);

public fn EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable, EFI_HANDLE ImageHandle)
{
    gST = SystemTable;
    gImage = ImageHandle;
    gBS = SystemTable->BootServices;
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"[ + ] Initializing UEFI\r\n");
    println(L"[ + ] FSL EFI Initialized");
    println(L"[ + ] Initializing heap");
    set_heap_sz(_HEAP_PAGE_ * 10);
    init_mem();
    _FSLEFI_ = allocate(0, sizeof(fsl_efi) + 1);
    _FSLEFI_->variables = init_map();
    _FSLEFI_->var_len = 0;
    _FSLEFI_->cursor = (_cordination){0};
    
    println(L"[ + ] Heap Initialized");
    SYSTEM_USER_NAME = get_line(L"Username: ");
    if(!SYSTEM_USER_NAME)
        return;

    print_args((string []){L"Welcome to FSL OS, ", (string)SYSTEM_USER_NAME, L"\r\n", NULL});

    println(L"Loading FSL CLI");

    /* A Cursor Blinker thats way blinks way to fast rn, fuck that */
    // EFI_EVENT TimerEvent;
    // gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL,
    //                 TPL_CALLBACK,
    //                 blink_cursor,
    //                 NULL,
    //                 &TimerEvent);

    // gBS->SetTimer(TimerEvent, TimerPeriodic, 5000000);
    println(L"[+] RAW USB lib-style PoC");

    EFI_BLOCK_IO_PROTOCOL *blk = usb_find_raw_block();
    if(!blk) {
        fsl_panic(L"[-] No raw USB block device found\n");
        return;
    }

    print(L"[+] BlockSize: "), _printi(blk->Media->BlockSize), print(L"\r\n");
    print(L"[+] LastBlock: "), _printi(blk->Media->LastBlock), print(L"\r\n");

    VOID *buf = NULL;
    EFI_STATUS st = usb_read_lba(blk, 0, 1, &buf);
    if(EFI_ERROR(st)) {
        fsl_panic(L"[-] Read failed\r\n");
        return;
    }

    println(L"[+] LBA 0 dump (first 64 bytes):\n");
    hex_dump((UINT8 *)buf, 64);

    gBS->FreePool(buf);
    
    fsl_cli();
}

public fn read_usb_drive()
{
    println(L"[+] RAW USB lib-style PoC");

    EFI_BLOCK_IO_PROTOCOL *blk = usb_find_raw_block();
    if(!blk) {
        print(L"[-] No raw USB block device found\n");
        return;
    }

    // println(L"[+] LastBlock: %lu\n", blk->Media->LastBlock);

    VOID *buf = NULL;
    EFI_STATUS st = usb_read_lba(blk, 0, 1, &buf);
    if(EFI_ERROR(st)) {
        print(L"[-] Read failed: \n");
        return;
    }

    println(L"[+] LBA 0 dump (first 64 bytes):");
    hex_dump((UINT8 *)buf, 64);

    gBS->FreePool(buf);
}

void input_strip(const string buff, int *size)
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
    println(buffer);
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

			if(len > 0 && Key.UnicodeChar == L'\r')
            {
                println(L"\r\n");
                // input_strip(CMD, &len);

                int argc = 0;
                sArr args = NULL;

                if(mem_cmp(CMD, L"help", 4))
                {
                    println_color_text(EFI_WHITE, EFI_BLACK, L"This help command is working dawg");
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
