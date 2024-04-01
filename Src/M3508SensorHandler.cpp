#include "M3508SensorHandler.hpp"

const float M3508SensorHandler::RawPos2Rad = 0.00003994561794f;  /* 2Pi / 8191 / 3591 X 187*/
const float M3508SensorHandler::RawRpm2Rps = 0.005453242609f;    /* 2Pi / 60 X 187 / 3591 */
const float M3508SensorHandler::PiDiv19 = 0.1635972783f;      /* PI / 3591 X 187 */

M3508SensorHandler::M3508SensorHandler() : SensorEntity(ECT_M3508Sensor)
{
    hasAccelerationFdb = false;
    hasPositionFdb = true;
    hasSpeedFdb = true;
    hasTemperatureFdb = true;
    rotorPosition = 0.0f;
    rotorLastPosition = 0.0f;
    newDataReady = false;
}

void M3508SensorHandler::Init()
{
    SetCanTimeout(true);
}

void M3508SensorHandler::Update()
{
    ecd = (uint16_t)(lastCanMsg.Data[0] << 8 | lastCanMsg.Data[1]);           
    speed_rpm = (uint16_t)(lastCanMsg.Data[2] << 8 | lastCanMsg.Data[3]);     
    given_current = (uint16_t)(lastCanMsg.Data[4] << 8 | lastCanMsg.Data[5]); 
    temperate = lastCanMsg.Data[6];  

    lastSpeedFdb = speedFdb;

    rotorLastPosition = rotorPosition;
    lastPositionFdb = positionFdb;
    rotorPosition = ecd * RawPos2Rad - PiDiv19;

    positionFdb += Math::LoopFloatConstrain((rotorPosition - rotorLastPosition), - PiDiv19, PiDiv19);
    speedFdb = speed_rpm * RawRpm2Rps;

    temperatureFdb = (float)temperate;
}
