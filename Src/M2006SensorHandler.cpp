#include "M2006SensorHandler.hpp"

const float M2006SensorHandler::RawPos2Rad = 0.000006782511971f;  /* 2Pi / 8191 / 36 */
const float M2006SensorHandler::RawRpm2Rps = 0.0009259259259f;    /* 2Pi / 60 / 36 */
const float M2006SensorHandler::PiDiv36 = 0.0872664826f;      /* PI / 36 */

M2006SensorHandler::M2006SensorHandler() : SensorEntity(ECT_M2006Sensor)
{
    hasAccelerationFdb = false;
    hasPositionFdb = true;
    hasSpeedFdb = true;
    hasTemperatureFdb = true;
    rotorPosition = 0.0f;
    rotorLastPosition = 0.0f;
    newDataReady = false;
}

void M2006SensorHandler::Init()
{
    SetCanTimeout(true);
}

void M2006SensorHandler::Update()
{
    ecd = (uint16_t)(lastCanMsg.Data[0] << 8 | lastCanMsg.Data[1]);           
    speed_rpm = (uint16_t)(lastCanMsg.Data[2] << 8 | lastCanMsg.Data[3]);     
    given_current = (uint16_t)(lastCanMsg.Data[4] << 8 | lastCanMsg.Data[5]); 
    temperate = lastCanMsg.Data[6];  

    lastSpeedFdb = speedFdb;

    rotorLastPosition = rotorPosition;
    lastPositionFdb = positionFdb;
    rotorPosition = ecd * RawPos2Rad - PiDiv36;

    positionFdb += Math::LoopFloatConstrain((rotorPosition - rotorLastPosition), - PiDiv36, PiDiv36);
    speedFdb = speed_rpm * RawRpm2Rps;

    temperatureFdb = (float)temperate;
}
