#include "bsp_limitswitch.h"

void bsp_limitswitch_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOI, &GPIO_InitStructure);
}

bool bsp_limitswitch_front_get(void)
{
    if (GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_6) == Bit_SET)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool bsp_limitswitch_rear_get(void)
{
    if (GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_7) == Bit_SET)
    {
        return true;
    }
    else
    {
        return false;
    }
}
