#ifndef M2006_HPP
#define M2006_HPP

#include "Motor.hpp"
#include "M2006SensorHandler.hpp"
#include "Pid.hpp"
#include "CanManager.hpp"

class M2006 : public Motor
{
public:
    M2006SensorHandler sensorFeedBack;
    Pid pidSpeed;
    Pid pidPosition;
    float currentSet;
    CAN_TypeDef* can;
    uint32_t canId;

    M2006();

    void RegistMotor(CAN_TypeDef* _can, int _canId)
    {
        can = _can;
        canId = _canId;
        sensorFeedBack.RegistSensor(_can, _canId);
    }

    virtual void Update();
};

#endif
