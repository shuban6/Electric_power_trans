#include "M2006.hpp"
#include "DjiCanMotorCommander.hpp"

M2006::M2006() : Motor(ECT_M2006)
{
    pFeedback = &sensorFeedBack;
    controlMode = Motor::RELAX_MODE;
    positionSet = 0.0f;
    speedSet = 0.0f;
    accelerationSet = 0.0f;
    currentSet = 0.0f;
}

void M2006::Update()
{
    if (controlMode == POS_MODE)
    {
        pidPosition.ref = positionSet;
        pidPosition.fdb = pFeedback->positionFdb;

        pidPosition.UpdateResult();

        speedSet = pidPosition.result;
    }

    pidSpeed.ref = speedSet;
    pidSpeed.fdb = pFeedback->speedFdb;

    pidSpeed.UpdateResult();

    currentSet = pidSpeed.result;
    
    if(controlMode == RELAX_MODE)
    {
        currentSet = 0.0f;
    }

    DjiCanMotorCommander::Instance()->Add2Buffer(can, canId, (int16_t)currentSet);
}
