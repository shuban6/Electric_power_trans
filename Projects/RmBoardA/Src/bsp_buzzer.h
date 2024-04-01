/**
  ****************************************************************
  * @file    bsp_buzzer.h
  * @author  TofuLemon(5thzhengjiarandom@gmail.com)
  * @date    2020-Jul-14
  ****************************************************************
  */

#ifndef BSP_BUZZER_H_
#define BSP_BUZZER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f4xx.h"

void bsp_buzzer_init(void);
void bsp_buzzer_on(void);
void bsp_buzzer_off(void);
void bsp_buzzer_toggle(void);
void bsp_buzzer_set_freq(uint32_t freq);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP_BUZZER_H_ */
