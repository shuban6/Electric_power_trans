#include "bsp_rgb.h"

void bsp_rgb_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Green rgb
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

}

void bsp_rgb_on(bsp_rgb_list_t _rgb)
{
    switch (_rgb)
    {
    case rgb_Green:
        GPIO_ResetBits(GPIOD, GPIO_Pin_12);
        break;

    case rgb_Red:
        GPIO_ResetBits(GPIOD, GPIO_Pin_13);
        break;

    case rgb_Yellow:
        GPIO_ResetBits(GPIOD, GPIO_Pin_14);
        break;

    default:
        break;
    }
}

void bsp_rgb_off(bsp_rgb_list_t _rgb)
{
    switch (_rgb)
    {
    case rgb_Green:
        GPIO_SetBits(GPIOD, GPIO_Pin_12);
        break;

    case rgb_Red:
        GPIO_SetBits(GPIOD, GPIO_Pin_13);
        break;

    case rgb_Yellow:
        GPIO_SetBits(GPIOD, GPIO_Pin_14);
        break;

    default:
        break;
    }
}

void bsp_rgb_toggle(bsp_rgb_list_t _rgb)
{
    switch (_rgb)
    {
    case rgb_Green:
        GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
        break;

    case rgb_Red:
        GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
        break;

    case rgb_Yellow:
        GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
        break;

    default:
        break;
    }
}
