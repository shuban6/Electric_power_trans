#include "ChargeCtrlMsg.hpp"
#include "CanMsgDispatcher.hpp"
#include "Stream.hpp"

void ChargeCtrlMsg::HandleNewCanRxMsg(CanRxMsg *_msg)
{
    CanMsgHandler::HandleNewCanRxMsg(_msg);

    uint8_t *ptr = lastCanMsg.Data;

    m_charge = ArrayStreamHelper::ReadUint8(ptr);
}

void ChargeCtrlMsg::Init(CAN_TypeDef *can, uint32_t _stdId)
{
    m_charge = 2;

    m_canId = _stdId;
    m_pCan = can;
    CanMsgDispatcher::Instance()->RegisterHandler(m_pCan, m_canId, this);
}

void ChargeCtrlMsg::SendMsg()
{
    uint8_t buffer[8] = {0};

    uint8_t *ptr = buffer;

    // 0 bytes
    ArrayStreamHelper::Write(ptr, m_charge);


    uint8_t bitfieldBuffer = 0x00;

    buffer[7] = bitfieldBuffer;

    CanManager::Instance()->CanTransmit(m_pCan, m_canId, buffer, sizeof(buffer));
}
