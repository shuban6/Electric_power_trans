#ifndef BSP_CAPACITOR_RELAY_H
#define BSP_CAPACITOR_RELAY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f4xx.h"

void bsp_capacitor_relay_init(void);
void bsp_capacitor_relay_on(void);
void bsp_capacitor_relay_off(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
