#ifndef BSP_JUDGE_H
#define BSP_JUDGE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f4xx.h"

void bsp_judge_init(void);
void bsp_judge_send(uint8_t* _pData, uint16_t _len);
uint8_t bsp_judge_send_busy(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
