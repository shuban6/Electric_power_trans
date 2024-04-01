#include "bsp_imu.h"

void bsp_imu_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART6, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART6, DISABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

    GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6); 
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	USART_DeInit(USART6);

	USART_InitStructure.USART_BaudRate = 921600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART6, &USART_InitStructure);
		
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
    USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);

	//USART_ITConfig(USART6, USART_IT_TC, ENABLE);

	USART_Cmd(USART6, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Stream6);

	DMA_InitStructure.DMA_Channel = DMA_Channel_5;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART6->DR);
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
    DMA_Init(DMA2_Stream6, &DMA_InitStructure);

	DMA_ITConfig(DMA2_Stream6, DMA_IT_TC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void bsp_imu_send(uint8_t* _pData, uint16_t _len)
{
    DMA_Cmd(DMA2_Stream6, DISABLE);
	DMA2_Stream6->M0AR = (uint32_t)_pData;

	DMA_SetCurrDataCounter(DMA2_Stream6, _len);
    DMA_Cmd(DMA2_Stream6, ENABLE);
    USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);
    //while( DMA_GetFlagStatus(DMA2_Stream6, DMA_FLAG_TCIF1) == RESET);
    //DMA_Cmd(DMA2_Stream6, DISABLE);
    //DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF1); 
}

uint8_t bsp_imu_send_busy(void)
{
	if(DMA_GetFlagStatus(DMA2_Stream6, DMA_FLAG_TCIF6) == SET)
	{
		return 0x01;
	}

	return 0x00;
}

void DMA2_Stream6_IRQHandler(void)
{
	DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6);
}
