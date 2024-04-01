#include "bsp_pwm.h"

void bsp_pwm_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    // Input freq = 90MHz 9375 = 125*75 f = 9600Hz
    TIM_InitStruct.TIM_Period = 124;
    TIM_InitStruct.TIM_Prescaler = 74;
    TIM_InitStruct.TIM_ClockDivision = 0;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_InitStruct);
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC4Init(TIM4, &TIM_OCInitStruct);
    TIM4->CCR1 = 124;

    TIM_Cmd(TIM4, DISABLE);
}

void bsp_pwm_on(void)
{
    TIM_Cmd(TIM4, ENABLE);
}

void bsp_pwm_off(void)
{
    TIM_Cmd(TIM4, DISABLE);
}

void bsp_pwm_toggle(void)
{
    TIM4->CR1 ^= TIM_CR1_CEN;
}

void bsp_pwm_set_period(uint16_t period)
{
    TIM4->ARR = (uint16_t)(period - 1);
}

void bsp_pwm_set_freq(uint32_t freq)
{
    TIM4->PSC = (uint16_t)(300000.0f / freq);
}

void bsp_pwm_set_dutyratio(uint16_t ratio)
{
    TIM4->CCR1 = ratio;
}
