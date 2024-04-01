#include "HostPacket.hpp"

void HostPacket::Registration()
{
    HostPacketManager::Instance()->PacketHandlerRegistration(0, m_ProtocolId, this);
}

void HostPacket::SendPacket()
{
    HostPacketManager::Instance()->SendPacket(m_Buffer, m_DataLen + 8);
}

void TestHostPacket::OnPacketReceived()
{
    SerializePacket(m_InputStream);
    NotifyAll((void*)&m_testData);
}

void TestHostPacket::SendPacket()
{
    SerializePacket(m_OutputStream);
    ++m_testData;

    HostPacket::SendPacket();
}

template<typename Stream>
void TestHostPacket::SerializePacket(Stream &stream)
{
    m_DataLen = 4;

    SerializeHeader(stream);

    stream.SerializeU32(m_testData);

    SerializeCrc16(stream);
}

HostPacketManager::HostPacketManager() : PacketManager(1)
{
    m_pChannel = &m_StreamChannel;
}

void HostPacketManager::Init()
{
    bsp_host_com_init();
    PacketManager::Init();

    m_testPacket.Init(0x01);
}

bool HostPacketManager::FlushSendBufferLow()
{
    if (bsp_host_com_send_busy())
    {
        return false;
    }

    bsp_host_com_send(m_SendBuffer, m_SendBufferLength);
    return true;
}
