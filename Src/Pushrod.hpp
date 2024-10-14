#ifndef PUSHROD_HPP
#define PUSHROD_HPP

#include "bsp_pushrod.h"

class Pushrod
{
private:
    uint8_t m_pushrod_state;
    uint8_t m_charge_state;

public:
    Pushrod(): m_pushrod_state(2), m_charge_state(1){}

    void Init()
    {
        bsp_pushrod_init();
    }

    void Update()
    {
        switch (m_pushrod_state)
        {
        case 1:
            In();
            break;
        case 0:
            Out();
            break;
        default:
            bsp_stop();
            break;
        }

        switch (m_charge_state)
        {
        case 1:
            bsp_discharge();
            break;
        case 2:
            bsp_charge();
            break;
        default:
            bsp_discharge();
            break;
        }
    }

    void Out()
    {
        bsp_place();
    }

    void In()
    {
        bsp_pick();
    }



    void SetPushRodState(uint8_t state)
    {
        m_pushrod_state = state;
    }

    void SetChargeState(uint8_t state)
    {
        m_charge_state = state;
    }

    uint8_t GetPushRodState()
    {
        return m_pushrod_state;
    }

    uint8_t GetChargeState()
    {
        return m_charge_state;
    }

    static Pushrod *Instance()
    {
        static Pushrod instance;
        return &instance;
    }
};
#endif // PUSHROD_HPP
