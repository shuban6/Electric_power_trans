#include "PushrodCtrlMsg.hpp"
#include "CanMsgDispatcher.hpp"
#include "Stream.hpp"

void PushrodCtrlMsg::HandleNewCanRxMsg(CanRxMsg *_msg)
{
    CanMsgHandler::HandleNewCanRxMsg(_msg);

    uint8_t *ptr = lastCanMsg.Data;

    m_pushrod = ArrayStreamHelper::ReadUint8(ptr);
}

void PushrodCtrlMsg::Init(CAN_TypeDef *can, uint32_t _stdId)
{
    m_pushrod = 2;

    m_canId = _stdId;
    m_pCan = can;
    CanMsgDispatcher::Instance()->RegisterHandler(m_pCan, m_canId, this);
}

void PushrodCtrlMsg::SendMsg()
{
    uint8_t buffer[8] = {0};

    uint8_t *ptr = buffer;

    // 0 bytes
    ArrayStreamHelper::Write(ptr, m_pushrod);
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
