#ifndef BSP_INFRARED_H
#define BSP_INFRARED_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "stm32f4xx.h"
#include <stdbool.h>

void bsp_infrared_init(void);
bool bsp_infrared_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
