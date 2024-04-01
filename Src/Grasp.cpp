#include "Grasp.hpp"

Grasp::Grasp()
{
    m_grasp_state = 0;
}

void Grasp::Disable()
{
    bsp_pwm_off();
    bsp_gpio_low();
}

void Grasp::Enable()
{
    bsp_pwm_on();
    bsp_gpio_high();
}

void Grasp::Init()
{
    bsp_pwm_init();
    bsp_gpio_init();
}

void Grasp::stretch()
{
    bsp_pwm_on();
    bsp_gpio_high();
    bsp_pwm_set_dutyratio(62);
}

void Grasp::contract()
{
    bsp_pwm_on();
    bsp_gpio_low();
    bsp_pwm_set_dutyratio(62);
}

void Grasp::GraspStop()
{
    bsp_pwm_off();
}

void Grasp::Update()
{
    if (m_grasp_state == 1)
    {
        Grasp::stretch();
    }
    else if (m_grasp_state == 2)
    {
        Grasp::contract();
    }
    else if (m_grasp_state == 0)
    {
        Grasp::GraspStop();
    }
}
