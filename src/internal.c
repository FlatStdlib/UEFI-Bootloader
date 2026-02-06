#include "init.h"

int __FSL_DEBUG__ = 0;
u16 _OUTPUT_[1024] = {0};

public fn toggle_debug_mode()
{ __FSL_DEBUG__ = __FSL_DEBUG__ ? 0 : 1; }

public fn printc(const char ch)
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

public fn print_color_text(int fg, int bg, const string buffer)
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
