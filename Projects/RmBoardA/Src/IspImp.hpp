#ifndef ISP_IMP_HPP
#define ISP_IMP_HPP

#include "stm32f4xx.h"

extern "C" void CAN1_RX0_IRQHandler(void);
extern "C" void CAN2_RX0_IRQHandler(void);
extern "C" void UART7_IRQHandler(void);
extern "C" void UART8_IRQHandler(void);
extern "C" void USART6_IRQHandler(void);

#endif
