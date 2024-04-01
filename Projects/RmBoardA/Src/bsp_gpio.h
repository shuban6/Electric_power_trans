#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "stm32f4xx.h"

    void bsp_gpio_init(void);
    void bsp_gpio_high(void);
    void bsp_gpio_low(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
