#include "bsp_buzzer.h"

void bsp_buzzer_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource6, GPIO_AF_TIM12);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOH, &GPIO_InitStruct);

    //Input freq = 90MHz
    TIM_InitStruct.TIM_Period = 299;
    TIM_InitStruct.TIM_Prescaler = 300000 / 440 - 1;
    TIM_InitStruct.TIM_ClockDivision = 0;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM12, &TIM_InitStruct);
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM12, &TIM_OCInitStruct);
    TIM12->CCR1 = 150;

    TIM_Cmd(TIM12, DISABLE);
}

void bsp_buzzer_on(void)
{
    TIM_Cmd(TIM12, ENABLE);
}

void bsp_buzzer_off(void)
{
    TIM_Cmd(TIM12, DISABLE);
}

void bsp_buzzer_toggle(void)
{
    TIM12->CR1 ^= TIM_CR1_CEN;
}

void bsp_buzzer_set_freq(uint32_t freq)
{
    TIM12->PSC = (uint16_t)(300000.0f / freq);
}
