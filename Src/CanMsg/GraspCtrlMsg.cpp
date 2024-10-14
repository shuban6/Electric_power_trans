#include "GraspCtrlMsg.hpp"
#include "CanMsgDispatcher.hpp"
#include "Stream.hpp"

void GraspCtrlMsg::HandleNewCanRxMsg(CanRxMsg *_msg)
{
    CanMsgHandler::HandleNewCanRxMsg(_msg);

    uint8_t *ptr = lastCanMsg.Data;

    uint16_t raw_grasp_dir = ArrayStreamHelper::ReadUint16(ptr);
    uint16_t raw_speed = ArrayStreamHelper::ReadUint16(ptr);


    if ( raw_grasp_dir == 2 )
    {
        m_speed = (float)raw_speed;
    }
    else if (raw_grasp_dir == 1)
    {
        m_speed = (float)-raw_speed;
    }
}

float GraspCtrlMsg::UncompressUint16(uint16_t _in)
{
    return (float)(((float)_in / (float)UINT16_MAX * (2.0f * MAX_CHASSIS_CTRL_SPEED)) - MAX_CHASSIS_CTRL_SPEED);
}

uint16_t GraspCtrlMsg::CompressFloat(float _in)
{
    return (uint16_t)((_in + MAX_CHASSIS_CTRL_SPEED) / (2.0f * MAX_CHASSIS_CTRL_SPEED) * (float)UINT16_MAX);
}

void GraspCtrlMsg::Init(CAN_TypeDef *can, uint32_t _stdId)
{
    m_speed = 0;

    m_canId = _stdId;
    m_pCan = can;
    CanMsgDispatcher::Instance()->RegisterHandler(m_pCan, m_canId, this);
}

void GraspCtrlMsg::SendMsg()
{
    uint8_t buffer[8] = {0};

    uint8_t *ptr = buffer;


    // 0 bytes
    ArrayStreamHelper::Write(ptr, CompressFloat(m_speed));
    // 2 bytes
    // ArrayStreamHelper::Write(ptr, CompressFloat(m_speed));
    // // 4 bytes
    // ArrayStreamHelper::Write(ptr, CompressFloat(m_Vw));

    // 6 bytes
    // ArrayStreamHelper::Write(ptr, m_projectileVelocitySet);
    // 7 bytes

    uint8_t bitfieldBuffer = 0x00;

    buffer[7] = bitfieldBuffer;

    CanManager::Instance()->CanTransmit(m_pCan, m_canId, buffer, sizeof(buffer));
}
