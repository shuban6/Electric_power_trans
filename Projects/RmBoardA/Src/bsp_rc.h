/**
  ****************************************************************
  * @file    bsp_rc.h
  * @author  TofuLemon(5thzhengjiarandom@gmail.com)
  * @date    2020-Jan-11
  ****************************************************************
  */

#ifndef BSP_RC_H_
#define BSP_RC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f4xx.h"

#define RC_CH_VALUE_MIN ((uint16_t)364)
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)
#define RC_CH_OFFSET_MAX ((uint16_t)660)

typedef __packed struct
{
        __packed struct
        {
                int16_t ch[5];
                char s[2];
        } rc;
        __packed struct
        {
                int16_t x;
                int16_t y;
                int16_t z;
                uint8_t press_l;
                uint8_t press_r;
        } mouse;
        __packed struct
        {
                uint16_t v;
        } key;

} RC_raw_t;

RC_raw_t* get_remote_control_raw(void);
void bsp_remote_control_init(void);
void bsp_rc_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num);
void bsp_rc_disable(void);
void bsp_rc_enable(void);
void bsp_rc_restart(uint16_t dma_buf_num);
uint8_t bsp_read_rc_update_flag(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP_RC_H_ */
