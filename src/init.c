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
    int len = 0;
    gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(EFI_WHITE, EFI_BLACK)
    );

    while(1)
    {
        u16 ret = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        if(ret == EFI_SUCCESS)
        {
            CMD[len++] = Key.UnicodeChar;
            if (Key.UnicodeChar != 0) {
                if(Key.UnicodeChar == 0x0A) {
                    memzero(CMD, 1024);
                    len = 0;
                }
                print(L"You pressed: "), printc(CMD[len - 1]), println(NULL);
            } else {
                print(L"You pressed: "), printc(CMD[len - 1]), println(NULL);
            }

            if(len > 0)
            {
                input_strip(CMD, &len);
                print(L"Current Buffer: "), print(CMD), println(NULL);

                if(str_cmp(CMD, L"help"))
                {
                    println(L"Working");
                }
            }
            print(L"> ");
        }
    }
}