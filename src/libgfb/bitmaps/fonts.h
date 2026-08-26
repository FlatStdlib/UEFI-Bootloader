#include "bold_font.h"
#include "default_font.h"
#include "large_font.h"

#define TOTAL_BOLD_FONTS 30

private struct {
    char ch;
    u64 *normal;
    u64 *fat;
} bold_fonts[] = {
    {'a', a_font_bitmap,            a_fat_font_bitmap},
    {'b', b_font_bitmap,            b_fat_font_bitmap},
    {'c', c_font_bitmap,            c_fat_font_bitmap},
    {'d', d_font_bitmap,            d_fat_font_bitmap},
    {'e', e_font_bitmap,            e_fat_font_bitmap},
    {'f', f_font_bitmap,            f_fat_font_bitmap},
    {'g', g_font_bitmap,            g_fat_font_bitmap},
    {'h', h_font_bitmap,            h_fat_font_bitmap},
    {'i', i_font_bitmap,            i_fat_font_bitmap},
    {'j', j_font_bitmap,            j_fat_font_bitmap},
    {'k', k_font_bitmap,            k_fat_font_bitmap},
    {'l', l_font_bitmap,            l_fat_font_bitmap},
    {'m', m_font_bitmap,            m_fat_font_bitmap},
    {'n', n_font_bitmap,            n_fat_font_bitmap},
    {'o', o_font_bitmap,            o_fat_font_bitmap},
    {'p', p_font_bitmap,            p_fat_font_bitmap},
    {'q', q_font_bitmap,            q_fat_font_bitmap},
    {'r', r_font_bitmap,            r_fat_font_bitmap},
    {'s', s_font_bitmap,            s_fat_font_bitmap},
    {'t', t_font_bitmap,            t_fat_font_bitmap},
    {'u', u_font_bitmap,            u_fat_font_bitmap},
    {'v', v_font_bitmap,            v_fat_font_bitmap},
    {'w', w_font_bitmap,            w_fat_font_bitmap},
    {'x', x_font_bitmap,            x_fat_font_bitmap},
    {'y', y_font_bitmap,            y_fat_font_bitmap},
    {'z', z_font_bitmap,            z_fat_font_bitmap},
    {' ', space_font_bitmap,        space_font_bitmap},
    {'[', open_brace_font_bitmap,   open_brace_font_bitmap},
    {']', close_brace_font_bitmap,  close_brace_font_bitmap},
    0
};

private ptr get_char_bitmap(char ch)
{
    for(int i = 0; i < TOTAL_BOLD_FONTS; i++)
        if(bold_fonts[i].ch == ch)
            return bold_fonts[i].normal;

    return 0;
}

private ptr get_bold_char_bitmap(char ch)
{
    for(int i = 0; i < TOTAL_BOLD_FONTS; i++)
        if(bold_fonts[i].ch == ch)
            return bold_fonts[i].fat;

    return 0;
}