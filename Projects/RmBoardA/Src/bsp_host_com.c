#include "bsp_host_com.h"

void bsp_host_com_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART8, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART8, DISABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_UART8); 
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_UART8);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	USART_DeInit(UART8);

	USART_InitStructure.USART_BaudRate = 921600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(UART8, &USART_InitStructure);
		
	USART_ITConfig(UART8, USART_IT_RXNE, ENABLE);
    USART_DMACmd(UART8, USART_DMAReq_Tx, ENABLE);

	//USART_ITConfig(UART8, USART_IT_TC, ENABLE);

	USART_Cmd(UART8, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = UART8_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream0);

	DMA_InitStructure.DMA_Channel = DMA_Channel_5;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (UART8->DR);
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = 512;
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
    DMA_Init(DMA1_Stream0, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Stream0, DMA_IT_TC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void bsp_host_com_send(uint8_t* _pData, uint16_t _len)
{
	if(DMA_GetFlagStatus(DMA1_Stream0, DMA_FLAG_TEIF0) == SET)
	{
		DMA1->LIFCR = (0x0001 << 3);
	}

    DMA_Cmd(DMA1_Stream0, DISABLE);
	DMA1_Stream0->M0AR = (uint32_t)_pData;

	DMA_SetCurrDataCounter(DMA1_Stream0, _len);
    DMA_Cmd(DMA1_Stream0, ENABLE);
    USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);
    //while( DMA_GetFlagStatus(DMA1_Stream0, DMA_FLAG_TCIF1) == RESET);
    //DMA_Cmd(DMA1_Stream0, DISABLE);
    //DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF1); 
}

uint8_t bsp_host_com_send_busy(void)
{
	if(DMA_GetFlagStatus(DMA1_Stream0, DMA_FLAG_TCIF0) == SET)
	{
		return 0x01;
	}

	return 0x00;
}

void DMA1_Stream0_IRQHandler(void)
{
	DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0);
}
