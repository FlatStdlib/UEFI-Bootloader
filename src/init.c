#include "init.h"

EFI_BOOT_SERVICES *gBS;
EFI_SYSTEM_TABLE *gST;

__declspec(dllexport) void EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable); 

public fn test_this(EFI_SYSTEM_TABLE *SystemTable)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Test Function!\n");
}

public fn EFIAPI Init_FSL(EFI_SYSTEM_TABLE *SystemTable)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"FSL Initialized....!\n");
    test_this(SystemTable);
}

// public fn my_c_function(EFI_SYSTEM_TABLE *SystemTable)
// {
//     EFI_INPUT_KEY Key;
//     SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Press any key to continue...\n");
//     for(int i = 0; i < 10; i++)
//         SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Called C function!\r\n");

//     while(1)
//     {
//         CHAR16 key = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
//         if(key == EFI_SUCCESS)
//         {
//             CHAR16 buf[2];
//             if (Key.UnicodeChar != 0) {
//                 buf[0] = Key.UnicodeChar;
//                 buf[1] = L'\0'; 
//                 SystemTable->ConOut->OutputString(SystemTable->ConOut, L"You pressed: ");
//                 SystemTable->ConOut->OutputString(SystemTable->ConOut, buf);
//                 SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
//             } else {
//                 buf[0] = Key.ScanCode;
//                 buf[1] = L'\0'; 
//                 SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Special key pressed: scan code =");
//                 SystemTable->ConOut->OutputString(SystemTable->ConOut, buf);
//                 SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
//             }
//         }
//     }
// }