#ifndef BSP_RGB_H_
#define BSP_RGB_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "stm32f4xx.h"

    typedef enum bsp_rgb_list
    {
        rgb_Red = 0,
        rgb_Green,
        rgb_Yellow
    } bsp_rgb_list_t;

    void bsp_rgb_init(void);
    void bsp_rgb_on(bsp_rgb_list_t _rgb);
    void bsp_rgb_off(bsp_rgb_list_t _rgb);
    void bsp_rgb_toggle(bsp_rgb_list_t _rgb);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
