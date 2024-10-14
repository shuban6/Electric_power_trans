#ifndef BSP_GRASP_H_
#define BSP_GRASP_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "stm32f4xx.h"

    void bsp_pwm_init(void);
    void bsp_pwm_on(void);
    void bsp_pwm_off(void);
    void bsp_pwm_toggle(void);
    void bsp_pwm_set_freq(uint16_t freq);

    void bsp_gpio_init(void);
    void bsp_gpio_high(void);
    void bsp_gpio_low(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
