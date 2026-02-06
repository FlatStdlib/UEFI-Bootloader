#include "init.h"

EFI_BOOT_SERVICES *gBS;
EFI_SYSTEM_TABLE *gST;
fsl_efi _FSLEFI_ = {0};

__declspec(dllexport) void EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable);
public fn fsl_cli();

u16 NAME[100];
public fn EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"[ + ] Initializing UEFI\r\n");
    gST = SystemTable;
    _FSLEFI_ = (fsl_efi){
        .variables = allocate(0, sizeof(char)),
        .var_len = 0,
        .cursor = (_cordination){0}
    };

    println(L"[ + ] FSL EFI Initialized");
    println(L"[ + ] Initializing heap");
    init_mem();
    println(L"[ + ] Heap Initialized");
    println(L"Loading FSL CLI");
    fsl_cli();
}

void input_strip(const string buff, int *size)
{
    if(!buff)
        return;

    if(buff[*size - 1] == '\n' || buff[*size - 1] == '\r')
        buff[*size - 1] = '\0', size--;

    if(buff[*size - 1] == '\n' || buff[*size - 1] == '\n')
        buff[*size - 1] = '\0', size--;
}

static inline UINT64 rdtsc(void) {
    UINT32 lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((UINT64)hi << 32) | lo;
}

#define CPU_HZ 3000000000ULL
#define BLINK_INTERVAL (CPU_HZ)
public fn fsl_cli()
{
    gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(EFI_LIGHTGREEN, EFI_BLACK)
    );
    println(L"Welcome to FSL OS's CLI   ");
    println(L"Type ? for a list of commands");
    EFI_INPUT_KEY Key;
    u16 CMD[1024];
    memzero(CMD, 1024);
    int len = 0;
    gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(EFI_WHITE, EFI_BLACK)
    );
    print_color_text(EFI_GREEN, EFI_BLUE, L"TEST");

	int current_color = EFI_BLACK;
	int visible = 0, last_toggle = 0;
    while(1)
    {
    	UINTN CursorX = gST->ConOut->Mode->CursorColumn;
		UINTN CursorY = gST->ConOut->Mode->CursorRow;
        u16 ret = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        /* Cursor */
        UINT64 now = rdtsc();

	    if ((now - last_toggle) >= BLINK_INTERVAL) {
	        last_toggle = now;
	        visible = !visible;

	        if (visible) {
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

        if(ret == EFI_SUCCESS)
        {
            if(Key.UnicodeChar != 0 && is_ascii(Key.UnicodeChar))
            {
	            CMD[len++] = Key.UnicodeChar;
			}


			if(Key.UnicodeChar == L'\r' || Key.UnicodeChar == L'\n')
            {
                print_color_text(EFI_GREEN, EFI_BLACK, L"Current Buffer: "), print(CMD), print(L"\r\n> ");
                memzero(CMD, 1024);
                len = 0;
            }

			if(Key.UnicodeChar == 0x1B)
                    print_color_text(EFI_RED, EFI_BLACK, L"Escape key pressed!");

            if(len > 0)
            {
                input_strip(CMD, &len);

                int argc = 0;
                sArr args = NULL;

                if(mem_cmp(CMD, L"help", 4))
                {
                    print_color_text(EFI_RED, EFI_BLACK, L"This help command is working dawg");
                } else if(find_string(CMD, L"set") > -1)
                {
                    args = split_string(CMD, ' ', &argc);
                    if(!args || argc < 3)
                        continue;

                    set_new_variable(args[1], args[2]);
                    print_color_text(EFI_WHITE, EFI_RED, L"Variable set!");
                } else if(find_string(CMD, L"echo") > -1)
                {
                    args = split_string(CMD, ' ', &argc);
                    if(!args || argc < 2)
                        continue;

                    string value = find_key(_FSLEFI_.variables, args[1]);
                    if(!value) {
						println(L"Unable to find variable!");
                        pfree_array((array)args);
                        continue;
                    }

                    // _printf(L"Var Value: %s\r\n", value);

                }
            }
        }
    }
}
