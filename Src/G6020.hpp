#ifndef G6020_HPP
#define G6020_HPP

#include "Motor.hpp"
#include "G6020SensorHandler.hpp"
#include "Pid.hpp"
#include "CanManager.hpp"
#include "DjiCanMotorCommander.hpp"

class G6020 : public Motor
{
public:
    G6020SensorHandler sensorFeedBack;
    Pid pidSpeed;
    Pid pidPosition;
    float currentSet;
    CAN_TypeDef* can;
    uint32_t canId;
    float ImuW;
    bool UseImuW;

    G6020();

    void RegistMotor(CAN_TypeDef* _can, int _canId)
    {
        can = _can;
        canId = _canId;
        sensorFeedBack.RegistSensor(_can, _canId);
    }

    virtual void Update();
};

#endif
