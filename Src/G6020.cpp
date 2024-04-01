#include "G6020.hpp"

G6020::G6020() : Motor(ECT_G6020), UseImuW(false)
{
    pFeedback = &sensorFeedBack;
    controlMode = Motor::RELAX_MODE;
    positionSet = 0.0f;
    speedSet = 0.0f;
    accelerationSet = 0.0f;
    currentSet = 0.0f;
}

void G6020::Update()
{
    float _spdFdb = pFeedback->speedFdb;
    if(UseImuW)
    {
        _spdFdb = ImuW;
    }

    if (controlMode == POS_MODE)
    {
        float err;
        err = positionSet - pFeedback->positionFdb;
        err = Math::LoopFloatConstrain(err, -Math::Pi, Math::Pi);

        pidPosition.UpdateIResult(err);

        // err[n] - err[n-1] = (ref[n] - fdb[n])-(ref[n-1] - fdb[n-1]) = -spd
        speedSet = pidPosition.kp * err + pidPosition.iResult - pidPosition.kd * pFeedback->speedFdb;

        speedSet = Math::LimitMax(speedSet, pidPosition.maxOut);
    }

    pidSpeed.ref = speedSet;
    pidSpeed.fdb = _spdFdb;

    pidSpeed.UpdateResult();

    currentSet = pidSpeed.result;

    if(controlMode == RELAX_MODE)
    {
        currentSet = 0.0f;
    }

    DjiCanMotorCommander::Instance()->Add2Buffer(can, canId, (int16_t)currentSet);
}
