#include "ChassisCtrlMsg.hpp"
#include "CanMsgDispatcher.hpp"
#include "Stream.hpp"

void ChassisCtrlMsg::HandleNewCanRxMsg(CanRxMsg* _msg)
{
    CanMsgHandler::HandleNewCanRxMsg(_msg);

    uint8_t* ptr = lastCanMsg.Data;

    uint16_t rawVx = ArrayStreamHelper::ReadUint16(ptr);
    uint16_t rawVy = ArrayStreamHelper::ReadUint16(ptr);
    uint16_t rawVw = ArrayStreamHelper::ReadUint16(ptr);

    m_Vx = UncompressUint16(rawVx);

}

float ChassisCtrlMsg::UncompressUint16(uint16_t _in)
{
    return (float)(((float)_in / (float)UINT16_MAX * (2.0f * MAX_CHASSIS_CTRL_SPEED)) - MAX_CHASSIS_CTRL_SPEED);
}

uint16_t ChassisCtrlMsg::CompressFloat(float _in)
{
    return (uint16_t)((_in + MAX_CHASSIS_CTRL_SPEED) / (2.0f * MAX_CHASSIS_CTRL_SPEED) * (float)UINT16_MAX);
}

void ChassisCtrlMsg::Init(CAN_TypeDef* can, uint32_t _stdId)
{
    m_Vx = 0.0f;
    m_pVx = 0;
    m_infrared = 0;
    m_limitswitch_front = 0;
    m_limitswitch_rear = 0;

    m_canId = _stdId;
    m_pCan = can;
    CanMsgDispatcher::Instance()->RegisterHandler(m_pCan, m_canId, this);
}

void ChassisCtrlMsg::SendMsg()
{
    uint8_t buffer[8] = {0};

    uint8_t* ptr = buffer;

    m_pVx = CompressFloat(m_Vx);

    // 0 bytes
    ArrayStreamHelper::Write(ptr, CompressFloat(m_Vx));
    // 2 bytes
    ArrayStreamHelper::Write(ptr, m_infrared);
    // 4 bytes
    ArrayStreamHelper::Write(ptr, m_limitswitch_front);
    ArrayStreamHelper::Write(ptr, m_limitswitch_rear);
    // 6 bytes
    // ArrayStreamHelper::Write(ptr, m_projectileVelocitySet);
    // 7 bytes

    uint8_t bitfieldBuffer = 0x00;
    
    buffer[7] = bitfieldBuffer;

    CanManager::Instance()->CanTransmit(m_pCan, m_canId, buffer, sizeof(buffer));
}
