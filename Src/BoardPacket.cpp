#include "BoardPacket.hpp"
#include "CanMsgDispatcher.hpp"

void BoardPacket::Registration()
{
    uint32_t _channelId = 0;
    switch (m_ChannelCanId)
    {
    case CIT_Channel0:
        _channelId = 0;
        break;
    case CIT_Channel1:
        _channelId = 1;
        break;

    default:
        break;
    }
    BoardPacketManager::Instance()->PacketHandlerRegistration(_channelId, m_ProtocolId, this);
}

void BoardPacket::SendPacket()
{
    BoardPacketManager::Instance()->SendPacket(m_Buffer, m_DataLen + 8);
}

void TestBoardPacket::OnPacketReceived()
{
    SerializePacket(m_InputStream);
    NotifyAll((void*)&m_testData);
}

void TestBoardPacket::SendPacket()
{
    SerializePacket(m_OutputStream);
    BoardPacket::SendPacket();
}

template<typename Stream>
void TestBoardPacket::SerializePacket(Stream &stream)
{
    m_DataLen = 1;

    SerializeHeader(stream);

    stream.SerializeU8(m_testData);

    SerializeCrc16(stream);
}

void RobotStatePacket::OnPacketReceived()
{
    SerializePacket(m_InputStream);
}

void RobotStatePacket::SendPacket()
{
    SerializePacket(m_OutputStream);
    BoardPacket::SendPacket();
}

template<typename Stream>
void RobotStatePacket::SerializePacket(Stream &stream)
{
    m_DataLen = 4;

    SerializeHeader(stream);

    stream.SerializeU8(chassis_level);
    stream.SerializeU8(shooter_level);
    stream.SerializeU16(shooter_heat);

    SerializeCrc16(stream);
}

void ShooterStateSetPacket::OnPacketReceived()
{
    SerializePacket(m_InputStream);
}

void ShooterStateSetPacket::SendPacket()
{
    SerializePacket(m_OutputStream);
    BoardPacket::SendPacket();
}

template<typename Stream>
void ShooterStateSetPacket::SerializePacket(Stream &stream)
{
    m_DataLen = 1;

    SerializeHeader(stream);

    stream.SerializeU8(shooter_state);
	
    SerializeCrc16(stream);
}

BoardPacketManager::BoardPacketManager() : PacketManager(BOARD_CHANNEL_NUM), m_pCan(CAN2)
{
    m_pChannel = m_StreamChannel;
}

void BoardPacketManager::Init()
{
    CanMsgDispatcher::Instance()->RegisterHandler(m_pCan, CIT_Channel0, this);
    CanMsgDispatcher::Instance()->RegisterHandler(m_pCan, CIT_Channel1, this);
    PacketManager::Init();

    m_testPacket.Init(CIT_Channel0, 0x99);
    m_ctrlMsg.Init(CAN2, 0x18);
    m_gimbalRefMsg.Init(CAN2, 0x10);
    m_graspctrlMsg.Init(CAN2, 0x20);
    m_pushrodCtrlMsg.Init(CAN2, 0x30);
    m_serverCtrlMsg.Init(CAN2, 0x41);
    m_actuatorRefMsg.Init(CAN2, 0x40);
    m_chargeCtrlMsg.Init(CAN2, 0x42);
    m_robot_state_packet.Init(CIT_Channel0,0x1F);
}

void BoardPacketManager::Update()
{
    PacketManager::Update();
}

bool BoardPacketManager::FlushSendBufferLow()
{
    CanManager::Instance()->CanTransmit(m_pCan, CIT_Channel0, m_SendBuffer, m_SendBufferLength);
    return true;
}

void BoardPacketManager::HandleNewCanRxMsg(CanRxMsg *_msg)
{
    if (_msg->StdId == CIT_Channel0)
    {
        Enqueue(0, _msg->Data, _msg->DLC);
        return;
    }
    Enqueue(1, _msg->Data, _msg->DLC);
}
