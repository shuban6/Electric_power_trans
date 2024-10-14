#ifndef LIMITSWITCH_HPP
#define LIMITSWITCH_HPP

#include "bsp_limitswitch.h"

class LimitSwitch
{
private:
    bool m_limitswitch_front_state;
    bool m_limitswitch_rear_state;

public:
    LimitSwitch() : m_limitswitch_front_state(false), m_limitswitch_rear_state(false) {}

    void Init()
    {
        bsp_limitswitch_init();
    }

    void Update()
    {
        m_limitswitch_front_state = bsp_limitswitch_front_get();
        m_limitswitch_rear_state = bsp_limitswitch_rear_get();
    }

    bool GetLimitSwitchFrontState()
    {
        return bsp_limitswitch_front_get();
    }

    bool GetLimitSwitchRearState()
    {
        return bsp_limitswitch_rear_get();
    }

    static LimitSwitch *Instance()
    {
        static LimitSwitch instance;
        return &instance;
    }
};

#endif

