#ifndef BSP_LIMITSWITCH_H
#define BSP_LIMITSWITCH_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "stm32f4xx.h"
#include <stdbool.h>

void bsp_limitswitch_init(void);
bool bsp_limitswitch_front_get(void);
bool bsp_limitswitch_rear_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

