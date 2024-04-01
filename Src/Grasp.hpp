#ifndef GRASP_HPP
#define GRASP_HPP

#include "bsp_pwm.h"
#include "bsp_gpio.h"

#define MM_TO_PLUSE 3200 / 2
#define PLUSE_TO_MM (1 / MM_TO_PLUSE)
#define DIS_MM_TO_PLUSE(dis) (MM_TO_PLUSE * (dis))
#define SPEED_TO_PLUSE(speed) ((speed) * MM_TO_PLUSE)
#define TIMER_CLK (90000000 / 90)

#define CALC_ARR(speed) (TIMER_CLK / ((speed) * MM_TO_PLUSE))

class Grasp
{
private:
    uint8_t m_grasp_state;
public:
    Grasp();

    void Init();
    void Enable();
    void Disable();

    void stretch();
    void contract();
    void GraspStop();

    virtual void Update();

    void SetGrasp_State(uint8_t grasp_state){ m_grasp_state = grasp_state; }
    uint8_t GetGrasp() { return m_grasp_state; }

    static Grasp *Instance()
    {
        static Grasp instance;
        return &instance;
    }
};
#endif

