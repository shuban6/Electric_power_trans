#include "bsp_led.h"

void bsp_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Green Led
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // Red Led
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void bsp_led_on(bsp_led_list_t _led)
{
    switch (_led)
    {
        case Led_Green:
            GPIO_ResetBits(GPIOF, GPIO_Pin_14);
            break;

        case Led_Red:
            GPIO_ResetBits(GPIOE, GPIO_Pin_11);
            break;

        case Led_DebugH7:
            GPIO_ResetBits(GPIOG, GPIO_Pin_1);
            break;

        case Led_DebugG6:
            GPIO_ResetBits(GPIOG, GPIO_Pin_2);
            break;

        case Led_DebugF5:
            GPIO_ResetBits(GPIOG, GPIO_Pin_3);
            break;

        case Led_DebugE4:
            GPIO_ResetBits(GPIOG, GPIO_Pin_4);
            break;

        case Led_DebugD3:
            GPIO_ResetBits(GPIOG, GPIO_Pin_5);
            break;

        case Led_DebugC2:
            GPIO_ResetBits(GPIOG, GPIO_Pin_6);
            break;

        case Led_DebugB1:
            GPIO_ResetBits(GPIOG, GPIO_Pin_7);
            break;

        case Led_DebugA0:
            GPIO_ResetBits(GPIOG, GPIO_Pin_8);
            break;

        default:
            break;
    }
}

void bsp_led_off(bsp_led_list_t _led)
{
    switch (_led)
    {
        case Led_Green:
            GPIO_SetBits(GPIOF, GPIO_Pin_14);
            break;

        case Led_Red:
            GPIO_SetBits(GPIOE, GPIO_Pin_11);
            break;

        case Led_DebugH7:
            GPIO_SetBits(GPIOG, GPIO_Pin_1);
            break;

        case Led_DebugG6:
            GPIO_SetBits(GPIOG, GPIO_Pin_2);
            break;

        case Led_DebugF5:
            GPIO_SetBits(GPIOG, GPIO_Pin_3);
            break;

        case Led_DebugE4:
            GPIO_SetBits(GPIOG, GPIO_Pin_4);
            break;

        case Led_DebugD3:
            GPIO_SetBits(GPIOG, GPIO_Pin_5);
            break;

        case Led_DebugC2:
            GPIO_SetBits(GPIOG, GPIO_Pin_6);
            break;

        case Led_DebugB1:
            GPIO_SetBits(GPIOG, GPIO_Pin_7);
            break;

        case Led_DebugA0:
            GPIO_SetBits(GPIOG, GPIO_Pin_8);
            break;

        default:
            break;
    }
}

void bsp_led_toggle(bsp_led_list_t _led)
{
    switch (_led)
    {
        case Led_Green:
            GPIO_ToggleBits(GPIOF, GPIO_Pin_14);
            break;

        case Led_Red:
            GPIO_ToggleBits(GPIOE, GPIO_Pin_11);
            break;

        case Led_DebugH7:
            GPIO_ToggleBits(GPIOG, GPIO_Pin_1);
            break;

        case Led_DebugG6:
            GPIO_ToggleBits(GPIOG, GPIO_Pin_2);
            break;

        case Led_DebugF5:
            GPIO_ToggleBits(GPIOG, GPIO_Pin_3);
            break;

        case Led_DebugE4:
            GPIO_ToggleBits(GPIOG, GPIO_Pin_4);
            break;

        case Led_DebugD3:
            GPIO_ToggleBits(GPIOG, GPIO_Pin_5);
            break;

        case Led_DebugC2:
            GPIO_ToggleBits(GPIOG, GPIO_Pin_6);
            break;

        case Led_DebugB1:
            GPIO_ToggleBits(GPIOG, GPIO_Pin_7);
            break;

        case Led_DebugA0:
            GPIO_ToggleBits(GPIOG, GPIO_Pin_8);
            break;

        default:
            break;
    }
}
