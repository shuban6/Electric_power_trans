#include "ActuatorRefMsg.hpp"
#include "CanMsgDispatcher.hpp"
#include "Stream.hpp"

void ActuatorRefMsg::HandleNewCanRxMsg(CanRxMsg *_msg)
{
    CanMsgHandler::HandleNewCanRxMsg(_msg);

    uint8_t *ptr = lastCanMsg.Data;

    success = ArrayStreamHelper::ReadUint8(ptr);
    
}

void ActuatorRefMsg::Init(CAN_TypeDef *can, uint32_t _stdId)
{
    success = 0;

    m_canId = _stdId;
    m_pCan = can;
    CanMsgDispatcher::Instance()->RegisterHandler(m_pCan, m_canId, this);
}

void ActuatorRefMsg::SendMsg()
{
    uint8_t buffer[8] = {0};

    uint8_t *ptr = buffer;

    // 0 bytes
    ArrayStreamHelper::Write(ptr, success);
    // 2 bytes
    //ArrayStreamHelper::Write(ptr, m_command);
    // 4 bytes
    // ArrayStreamHelper::Write(ptr, m_limitswitch_front);
    // ArrayStreamHelper::Write(ptr, m_limitswitch_rear);
    // 6 bytes
    // ArrayStreamHelper::Write(ptr, m_projectileVelocitySet);
    // 7 bytes

    uint8_t bitfieldBuffer = 0x00;

    buffer[7] = bitfieldBuffer;

    CanManager::Instance()->CanTransmit(m_pCan, m_canId, buffer, sizeof(buffer));
}
