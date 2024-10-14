#ifndef RGB_HPP
#define RGB_HPP

#include "bsp_rgb.h"

class rgb
{
public:
    enum rgb_List
    {
        Red = 0,
        Green,
        Yellow
    };

    static void Init()
    {
        bsp_rgb_init();
        for (int i = (int)rgb_Red; i <= (int)rgb_Yellow; ++i)
        {
            bsp_rgb_on((bsp_rgb_list_t)i);
        }
    }

    static void On(rgb_List _rgb) { bsp_rgb_on((bsp_rgb_list_t)_rgb); }

    static void Off(rgb_List _rgb) { bsp_rgb_off((bsp_rgb_list_t)_rgb); }

    static void On_red()
    {
        bsp_rgb_off(rgb_Green);
        bsp_rgb_on(rgb_Red);
        bsp_rgb_on(rgb_Yellow);
    }

    static void On_green()
    {
        bsp_rgb_off(rgb_Red);
        bsp_rgb_on(rgb_Green);
        bsp_rgb_on(rgb_Yellow);
    }

    static void On_yellow()
    {
        bsp_rgb_off(rgb_Yellow);
        bsp_rgb_on(rgb_Red);
        bsp_rgb_on(rgb_Green);
    }

    static void Toggle(rgb_List _rgb)
    {
        bsp_rgb_toggle((bsp_rgb_list_t)_rgb);
    }
};

#endif
