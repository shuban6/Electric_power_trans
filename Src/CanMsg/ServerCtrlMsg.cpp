#include "ServerCtrlMsg.hpp"
#include "CanMsgDispatcher.hpp"
#include "Stream.hpp"

void ServerCtrlMsg::HandleNewCanRxMsg(CanRxMsg *_msg)
{
    CanMsgHandler::HandleNewCanRxMsg(_msg);

    uint8_t *ptr = lastCanMsg.Data;

    uint16_t rawVx = ArrayStreamHelper::ReadUint16(ptr);
    uint16_t rawVy = ArrayStreamHelper::ReadUint16(ptr);
    uint16_t rawVw = ArrayStreamHelper::ReadUint16(ptr);

}

void ServerCtrlMsg::Init(CAN_TypeDef *can, uint32_t _stdId)
{
    m_id = 0;
    m_command = 3;

    m_canId = _stdId;
    m_pCan = can;
    CanMsgDispatcher::Instance()->RegisterHandler(m_pCan, m_canId, this);
}

void ServerCtrlMsg::SendMsg()
{
    uint8_t buffer[8] = {0};

    uint8_t *ptr = buffer;

    // 0 bytes
    ArrayStreamHelper::Write(ptr, m_id);
    // 2 bytes
    ArrayStreamHelper::Write(ptr, m_command);
    // 4 bytes
    //ArrayStreamHelper::Write(ptr, m_limitswitch_front);
    //ArrayStreamHelper::Write(ptr, m_limitswitch_rear);
    // 6 bytes
    // ArrayStreamHelper::Write(ptr, m_projectileVelocitySet);
    // 7 bytes

    uint8_t bitfieldBuffer = 0x00;

    buffer[7] = bitfieldBuffer;

    CanManager::Instance()->CanTransmit(m_pCan, m_canId, buffer, sizeof(buffer));
}
