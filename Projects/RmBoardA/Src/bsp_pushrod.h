#ifndef BSP_PUSHROD_H
#define BSP_PUSHROD_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "stm32f4xx.h"
#include <stdbool.h>

    void bsp_pushrod_init(void);
    void bsp_place(void);
    void bsp_pick(void);
    void bsp_stop(void);
    void bsp_charge(void);
    void bsp_discharge(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
