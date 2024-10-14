#include "bsp_infrared.h"

void bsp_infrared_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOI, &GPIO_InitStructure);
}

bool bsp_infrared_get(void)
{
    if (GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_2) == Bit_RESET)
    {
        return true;
    }
    else
    {
        return false;
    }
}
