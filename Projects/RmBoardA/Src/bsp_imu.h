#ifndef BSP_IMU_H
#define BSP_IMU_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f4xx.h"

void bsp_imu_init(void);
void bsp_imu_send(uint8_t* _pData, uint16_t _len);
uint8_t bsp_imu_send_busy(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
