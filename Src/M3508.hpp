#ifndef M3508_HPP
#define M3508_HPP

#include "Motor.hpp"
#include "M3508SensorHandler.hpp"
#include "Pid.hpp"
#include "CanManager.hpp"
#include "DjiCanMotorCommander.hpp"

class M3508 : public Motor
{
public:
    M3508SensorHandler sensorFeedBack;
    Pid pidSpeed;
    Pid pidPosition;
    float currentSet;
    CAN_TypeDef* can;
    uint32_t canId;

    M3508();

    void RegistMotor(CAN_TypeDef* _can, int _canId)
    {
        can = _can;
        canId = _canId;
        sensorFeedBack.RegistSensor(_can, _canId);
    }

    virtual void Update();
};

#endif
