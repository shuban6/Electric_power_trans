#include "G6020SensorHandler.hpp"

const float G6020SensorHandler::RawPos2Rad = 0.0007670840321f;
const float G6020SensorHandler::RawRpm2Rps = 0.1047197551f;

G6020SensorHandler::G6020SensorHandler() : SensorEntity(ECT_G6020Sensor)
{
    hasAccelerationFdb = false;
    hasPositionFdb = true;
    hasSpeedFdb = true;
    hasTemperatureFdb = true;
    newDataReady = false;
}

void G6020SensorHandler::Init()
{
    SetCanTimeout(true);
}

void G6020SensorHandler::Update()
{
    ecd = (uint16_t)(lastCanMsg.Data[0] << 8 | lastCanMsg.Data[1]);           
    speed_rpm = (uint16_t)(lastCanMsg.Data[2] << 8 | lastCanMsg.Data[3]);     
    given_current = (uint16_t)(lastCanMsg.Data[4] << 8 | lastCanMsg.Data[5]); 
    temperate = lastCanMsg.Data[6];

    lastPositionFdb = positionFdb;

    lastSpeedFdb = speedFdb;
    positionFdb = Math::LoopFloatConstrain((float)(ecd - offset) * RawPos2Rad, - Math::Pi, Math::Pi);
    speedFdb = speed_rpm * RawRpm2Rps;
    
    temperatureFdb = (float)temperate;
}
