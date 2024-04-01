#include "bsp_judge.h"

void bsp_judge_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART7, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART7, DISABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_UART7); 
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_UART7);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	USART_DeInit(UART7);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(UART7, &USART_InitStructure);
		
	USART_ITConfig(UART7, USART_IT_RXNE, ENABLE);
    USART_DMACmd(UART7, USART_DMAReq_Tx, ENABLE);

	//USART_ITConfig(UART7, USART_IT_TC, ENABLE);

	USART_Cmd(UART7, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = UART7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream1);

	DMA_InitStructure.DMA_Channel = DMA_Channel_5;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (UART7->DR);
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = 200;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream1, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void bsp_judge_send(uint8_t* _pData, uint16_t _len)
{
    DMA_Cmd(DMA1_Stream1, DISABLE);
	DMA1_Stream1->M0AR = (uint32_t)_pData;

	DMA_SetCurrDataCounter(DMA1_Stream1, _len);
    DMA_Cmd(DMA1_Stream1, ENABLE);
    USART_DMACmd(UART7,USART_DMAReq_Tx,ENABLE);
    //while( DMA_GetFlagStatus(DMA1_Stream1, DMA_FLAG_TCIF1) == RESET);
    //DMA_Cmd(DMA1_Stream1, DISABLE);
    //DMA_ClearFlag(DMA1_Stream1, DMA_FLAG_TCIF1); 
}

uint8_t bsp_judge_send_busy(void)
{
	if(DMA_GetFlagStatus(DMA1_Stream1, DMA_FLAG_TCIF1) == SET)
	{
		return 0x01;
	}

	return 0x00;
}

void DMA1_Stream1_IRQHandler(void)
{
	DMA_ClearFlag(DMA1_Stream1, DMA_FLAG_TCIF1);
}
