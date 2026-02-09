#include "efi_libc.h"

int __FSL_DEBUG__ = 0;
u16 _OUTPUT_[1024] = {0};

public fn toggle_debug_mode()
{ __FSL_DEBUG__ = __FSL_DEBUG__ ? 0 : 1; }

public fn printc(const u16 ch)
{
	u16 BUFF[2] = {0};
	BUFF[0] = ch;
	BUFF[1] = '\0';
	print(BUFF);
}

public fn printi(i32 num)
{
	if(num == 0)
	{
		print((string)"0");
		return;
	}
	char BUFF[5] = {0};
	BUFF[0] = '0' + num;
	print((string)BUFF);
}


public fn _printi(int num)
{
    int temp = num, c = 0;
    char buff[150] = {0};
    while(temp)
    {
        buff[c++] = '0' + (temp % 10);
        temp /= 10;
    }

	int i;
    for(i = 0; i < c; i++)
    {
        char t = buff[i], n = buff[--c];
        buff[i] = n;
        buff[c] = t;
    }

	print((string)buff);
}

public fn printc_color_text(int fg, int bg, const char ch)
{
	gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(fg, bg)
    );

    u16 buff[2];
    buff[0] = ch;
    buff[1] = '\0';
    print(buff);

	gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(EFI_WHITE, EFI_BLACK)
    );
}

public fn print_color_text(int fg, int bg, const string buffer)
{
	gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(fg, bg)
    );

    print(buffer);

	gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(EFI_WHITE, EFI_BLACK)
    );
}

public fn println_color_text(int fg, int bg, const string buffer)
{
	gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(fg, bg)
    );

    println(buffer);

	gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(EFI_WHITE, EFI_BLACK)
    );
}

public fn print(const string buffer)
{
    gST->ConOut->OutputString(gST->ConOut, buffer);
}

public fn println(const string buffer)
{
    if(buffer)
        gST->ConOut->OutputString(gST->ConOut, buffer);

    gST->ConOut->OutputString(gST->ConOut, L"\r\n");
}

public fn PrintU32(UINT32 val)
{
    CHAR16 buf[12];
    int i = 0;
    if (val == 0) { buf[i++] = L'0'; }
    while (val > 0) {
        buf[i++] = L'0' + (val % 10);
        val /= 10;
    }
    buf[i] = 0;

    for (int j = 0; j < i/2; j++) {
        CHAR16 tmp = buf[j];
        buf[j] = buf[i-1-j];
        buf[i-1-j] = tmp;
    }

    gST->ConOut->OutputString(gST->ConOut, buf);
}

public fn PrintU64(UINT64 val)
{
    CHAR16 buf[21];
    int i = 0;

    if (val == 0) {
        buf[i++] = L'0';
    }

    while (val > 0) {
        buf[i++] = L'0' + (val % 10);
        val /= 10;
    }

    buf[i] = 0;

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        CHAR16 tmp = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = tmp;
    }

    gST->ConOut->OutputString(gST->ConOut, buf);
}


public fn PrintDouble(double val)
{
    CHAR16 buf[32];
    int i = 0;

    if (val < 0) {
        buf[i++] = L'-';
        val = -val;
    }

    UINT64 int_part = (UINT64)val;
    double frac_part = val - (double)int_part;

    CHAR16 int_buf[20];
    int int_i = 0;
    if (int_part == 0) int_buf[int_i++] = L'0';
    while (int_part > 0) {
        int_buf[int_i++] = L'0' + (int_part % 10);
        int_part /= 10;
    }
    int_buf[int_i] = 0;

    for (int j = 0; j < int_i; j++) {
        buf[i++] = int_buf[int_i - 1 - j];
    }

    buf[i++] = L'.';

    for (int k = 0; k < 6; k++) {
        frac_part *= 10.0;
        int digit = (int)frac_part;
        buf[i++] = L'0' + digit;
        frac_part -= digit;
    }

    buf[i] = 0;

    gST->ConOut->OutputString(gST->ConOut, buf);
}


public fn print_color_text_args(int fg, int bg, string *arr)
{
	gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(fg, bg)
    );
    
    for(int i = 0; arr[i] != NULL; i++)
        print((string)arr[i]);

	gST->ConOut->SetAttribute(
        gST->ConOut,
        EFI_TEXT_ATTR(EFI_WHITE, EFI_BLACK)
    );
}

public fn print_args(string *arr)
{
    for(int i = 0; arr[i] != NULL; i++)
        print((string)arr[i]);
}

public ptr to_heap(ptr p, i32 sz)
{
	ptr pointer = allocate(0, sz + 1);
	if(!pointer)
		return NULL;

	mem_cpy(pointer, p, sz);
	((string)pointer)[sz] = '\0';
	return pointer;
}

public ptr copy_heap(ptr p)
{
    int new_size = __get_size__(p);
	ptr pointer = allocate(0, new_size + 1);
	if(!pointer)
		return NULL;

	mem_cpy(pointer, p, new_size);
	((string)pointer)[new_size] = '\0';
	return pointer;
}

public fn __fsl_panic(string msg, string file, int line)
{
	if(__FSL_DEBUG__)
		print(file), print(L":"), _printi(line), print(L" -> ");

	print_color_text(EFI_RED, EFI_BLACK, L"error: "), println(msg);
}
