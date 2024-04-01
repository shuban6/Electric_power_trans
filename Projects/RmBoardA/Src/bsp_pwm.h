#ifndef BSP_PWM_H_
#define BSP_PWM_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "stm32f4xx.h"

    void bsp_pwm_init(void);
    void bsp_pwm_on(void);
    void bsp_pwm_off(void);
    void bsp_pwm_toggle(void);
    void bsp_pwm_set_period(uint16_t period);
    void bsp_pwm_set_freq(uint32_t freq);
    void bsp_pwm_set_dutyratio(uint16_t ratio);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
