#include "bsp_capacitor_relay.h"

void bsp_capacitor_relay_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOI, &GPIO_InitStructure);
}

void bsp_capacitor_relay_on(void)
{
    GPIO_SetBits(GPIOI, GPIO_Pin_2);
}

void bsp_capacitor_relay_off(void)
{
    GPIO_ResetBits(GPIOI, GPIO_Pin_2);
}
