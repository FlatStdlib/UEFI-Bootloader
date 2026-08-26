
#include "gfb.h"            // Public
#include "../fsl_efi.h"     // Private

public fn create_box(int x, int y, int w, int h, u64 bg)
{
    int end_x = x + w;
    int end_y = y + h;
    for(int y = y; y < end_y; y++)
    {
        for(int x = x; x < end_x; x++)
        {
            draw_pixel(0, 0, x, y, bg);
        }
    }
}

public fn place_bold_text(int x, int y, int BM_height, int BM_width, i64 fg, i64 bg, string text)
{
    int len = str_len(text);
    int start_pos = x;
    for(int i = 0, font_spacing = 0; i < len; i++, font_spacing += 8) {
        u64 *bm = get_bold_char_bitmap(text[i]);
        if(bm == space_font_bitmap)
            output_char(start_pos + font_spacing, y, BM_height, BM_width, bg, bm);
        else
            output_char(start_pos + font_spacing, y, BM_height, BM_width, fg, bm);
    }
}

public fn place_text(int x, int y, int BM_height, int BM_width, i64 fg, i64 bg, string text)
{
    int len = str_len(text);
    int start_pos = x;
    for(int i = 0, font_spacing = 0; i < len; i++, font_spacing += 8) {
        u64 *bm = get_char_bitmap(text[i]);
        if(bm == space_font_bitmap)
            output_char(start_pos + font_spacing, y, BM_height, BM_width, bg, bm);
        else
            output_char(start_pos + font_spacing, y, BM_height, BM_width, fg, bm);
    }
}

// BM_ = Bitmap
public fn place_bitmap_text(int x, int y, int BM_height, int BM_width, i64 fg, i64 bg, u64 **text, i32 sz)
{
    int start_pos = x;
    for(int i = 0, font_spacing = 0; i < sz; i++, font_spacing += 8) {
        if(text[i] == space_font_bitmap) {
            output_char(start_pos + font_spacing, y, BM_height, BM_width, bg, text[i]);
            continue;
        } else {
            output_char(start_pos + font_spacing, y, BM_height, BM_width, fg, text[i]);
        }
        
        gBS->Stall(500000);
    }
}

// designed for specifically 8x8 fronts
public fn output_char(int at_x, int at_y, int width, int height, u32 color, u64 *bitmap)
{
    int bitcount = 0;
    for(int y = at_y; y < at_y + height; y++, bitcount++)
	{
		for(int x = at_x, bit = width; x < at_x + width; x++, bit--)
			if((bitmap[bitcount] >> bit) & 0xFF != 0)
				draw_pixel(0, 0, x, y, color);
	}
}

public fn output_large_char(int at_x, int at_y, int width, int height, u32 color, u64 *bitmap)
{
    int bitcount = 0;
    for (int y = at_y; y < at_y + height; y++)
    {
        for (int n = 0; n < 3; n++)
        {
            u64 row = bitmap[bitcount++];
            for (int x = 0; x < 8; x++)
            {
                int bit = 7 - x;
                if ((row >> bit) & 1)
                    draw_pixel(0, 0, at_x + (n * 8) + x, y, color);
            }
        }
    }
}

public fn clear_screen(uint32_t color)
{
    UINTN pixels = _FSLEFI_->resolution.x * _FSLEFI_->resolution.y;
    for(UINTN i = 0; i < pixels; i++)
        _FSLEFI_->framebuffer[i] = color;
}

public fn draw_pixel(int at_x, int at_y, int x, int y, uint32_t color) {
    UINTN stride = vGop->Mode->Info->PixelsPerScanLine;
    _FSLEFI_->framebuffer[(at_y + y) * stride + (at_x + x)] = color;
}