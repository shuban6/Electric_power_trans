#ifndef HOSTPACKET_HPP
#define HOSTPACKET_HPP

#include "bsp_host_com.h"
#include "Packet.hpp"

class HostPacket : public Packet
{
public:
    HostPacket(){}

    virtual void Init(uint8_t _id)
    {
        Packet::Init(_id);

        Registration();
    }

    virtual void Registration();
    virtual void SendPacket();
};

class TestHostPacket : public HostPacket
{
public:
	TestHostPacket(){}
    uint32_t m_testData;

    virtual void Init(uint8_t _id)
    {
        HostPacket::Init(_id);

        m_testData = 0x00;
    }

    virtual void OnPacketReceived();
    virtual void SendPacket();
    template<typename Stream> void SerializePacket(Stream &stream);
};

class HostPacketManager : public PacketManager
{
private:
    StreamChannel m_StreamChannel;

    TestHostPacket m_testPacket;

protected:
    virtual bool FlushSendBufferLow();

public:
    HostPacketManager();

    TestHostPacket& GetTestPacket()
    {
        return m_testPacket;
    }

    virtual void Init();

    static HostPacketManager* Instance()
    {
        static HostPacketManager instance;
        return &instance;
    }
};

#endif
