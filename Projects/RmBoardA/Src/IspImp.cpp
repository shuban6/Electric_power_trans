#include "IspImp.hpp"

#include "CanManager.hpp"
#include "CanMsgDispatcher.hpp"
#include "JudgeSystem.hpp"

#include "BoardPacket.hpp"
#include "BatterySystem.hpp"

void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg _rxMsg;

	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		CAN_Receive(CAN1, CAN_FIFO0, &_rxMsg);
	}

	CanManager::Instance()->MsgQueue(0)->Enqueue(&_rxMsg);
	
}

void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg _rxMsg;

	if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET)
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		CAN_Receive(CAN2, CAN_FIFO0, &_rxMsg);
	}

	CanManager::Instance()->MsgQueue(1)->Enqueue(&_rxMsg);
}


void UART7_IRQHandler(void)
{
	if(USART_GetFlagStatus(UART7, USART_IT_RXNE))
	{
    	uint8_t _data = UART7->DR;
		JudgeSystem::Instance()->Enqueue(_data);
	}
    if(USART_GetFlagStatus(UART7, USART_IT_NE))
	{
		USART_ReceiveData(UART7);
	}

		uint16_t _data = UART7->SR;
		_data = UART7->DR;

}

// void UART8_IRQHandler(void)
// {
// 	if(USART_GetFlagStatus(UART8, USART_IT_RXNE))
// 	{
//     	uint8_t _data = UART8->DR;
// 		HostPacketManager::Instance()->Enqueue(0, &_data, 1);
// 	}

//     if(USART_GetFlagStatus(UART8, USART_IT_NE))
// 	{
// 		USART_ReceiveData(UART8);
// 	}

// 		uint16_t _data = UART8->SR;
// 		_data = UART8->DR;

// }

void USART6_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART6, USART_IT_RXNE))
	{
    	uint8_t _data = USART6->DR;
		_data = USART6->DR;
		BatterySystem::Instance()->Enqueue(_data);
	}
    if(USART_GetFlagStatus(USART6, USART_IT_NE))
	{
		USART_ReceiveData(USART6);
	}

		uint16_t _data = USART6->SR;
		_data = USART6->DR;

}
