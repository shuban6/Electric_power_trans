#ifndef INFRARED_HPP
#define INFRARED_HPP

#include "bsp_infrared.h"

bool bsp_infrared_get(void);

class Infrared
{

private:
    bool m_infrared_state;

public:
    Infrared() : m_infrared_state(false) {}

    void Init()
    {
        bsp_infrared_init();
    }

    void Update()
    {
        m_infrared_state = bsp_infrared_get();
    }

    bool GetInfraredState()
    {
        return bsp_infrared_get();
    }

    static Infrared *Instance()
    {
        static Infrared instance;
        return &instance;
    }
};
#endif

