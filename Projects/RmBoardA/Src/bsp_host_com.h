#ifndef BSP_HOST_COM_H
#define BSP_HOST_COM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f4xx.h"

void bsp_host_com_init(void);
void bsp_host_com_send(uint8_t* _pData, uint16_t _len);
uint8_t bsp_host_com_send_busy(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
