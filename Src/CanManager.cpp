#include "CanManager.hpp"

bool CanManager::SendQueueEmpty()
{
    CanMsgStream &_stream = m_SendBuffer[m_SendFront];

    return m_SendRear == m_SendFront && _stream.m_LenSent == _stream.m_MsgLen;
}

void CanManager::SendOnePacket()
{
    CanMsgStream &_stream = m_SendBuffer[m_SendFront];

    CanTxMsg _txMessage;
    _txMessage.StdId = _stream.m_StreamId;
    _txMessage.IDE = CAN_ID_STD;
    _txMessage.RTR = CAN_RTR_DATA;

    if(_stream.m_MsgLen - _stream.m_LenSent > 8)
    {
        _txMessage.DLC = 8;
        memcpy(_txMessage.Data, &_stream.m_MsgBuffer[_stream.m_LenSent], 8);
        _stream.m_LenSent += 8;
        CAN_Transmit(_stream.m_pCan, &_txMessage);
    }
    else
    {
        uint8_t _len = _stream.m_MsgLen - _stream.m_LenSent;
        _txMessage.DLC = _len;
        memcpy(_txMessage.Data, &_stream.m_MsgBuffer[_stream.m_LenSent], _len);
        _stream.m_LenSent += _len;

        CAN_Transmit(_stream.m_pCan, &_txMessage);

        m_SendFront = (m_SendFront + 1) % 8;
    }
}

void CanManager::Init()
{
    bsp_can1_init();
    bsp_can2_init();
    m_SendFront = 0;
    m_SendRear = 0;
    m_PacketPerTick = 2;
}

void CanManager::Update()
{
    int8_t _packetToSend = m_PacketPerTick;

    while(_packetToSend > 0 && !SendQueueEmpty())
    {
        SendOnePacket();
        --_packetToSend;
    }
}

void CanManager::CanTransmit(CAN_TypeDef* _can, uint32_t _id, uint8_t* _pData, uint32_t _len)
{
    CanMsgStream& _stream = m_SendBuffer[m_SendRear];
    _stream.m_pCan = _can;
    _stream.m_LenSent = 0;
    _stream.m_MsgLen = _len;
    _stream.m_StreamId = _id;
    memcpy(_stream.m_MsgBuffer, _pData, _len);

    m_SendRear = (m_SendRear + 1) % 8;
}
