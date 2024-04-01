/*
 * LED BSP for Rm Board C
 */

#ifndef BSP_LED_H_
#define BSP_LED_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f4xx.h"

typedef enum bsp_led_list
{
    Led_Red = 0,
    Led_Green,
    Led_DebugH7,
    Led_DebugG6,
    Led_DebugF5,
    Led_DebugE4,
    Led_DebugD3,
    Led_DebugC2,
    Led_DebugB1,
    Led_DebugA0
} bsp_led_list_t;

void bsp_led_init(void);
void bsp_led_on(bsp_led_list_t _led);
void bsp_led_off(bsp_led_list_t _led);
void bsp_led_toggle(bsp_led_list_t _led);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
