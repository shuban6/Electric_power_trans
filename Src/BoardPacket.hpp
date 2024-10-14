#ifndef BOARDPACKET_HPP
#define BOARDPACKET_HPP

#include "Packet.hpp"
#include "CanMsgHandler.hpp"
#include "ChassisCtrlMsg.hpp"
#include "GimbalRefMsg.hpp"
#include "GraspCtrlMsg.hpp"
#include "PushrodCtrlMsg.hpp"
#include "ServerCtrlMsg.hpp"
#include "ActuatorRefMsg.hpp"
#include "ChargeCtrlMsg.hpp"

#define BOARD_CHANNEL_NUM 2

enum BoardPacketChannelIdType
{
    CIT_Channel0 = 0x120,
    CIT_Channel1 = 0x130,
};

class BoardPacket : public Packet
{
private:
    BoardPacketChannelIdType m_ChannelCanId;

public:
    BoardPacket(){}

    virtual void Init(BoardPacketChannelIdType _channelCanId, uint8_t _id)
    {
        Packet::Init(_id);
        m_ChannelCanId = _channelCanId;

        Registration();
    }

    virtual void Registration();
    virtual void SendPacket();
};

class ShooterStateSetPacket : public BoardPacket
{
public:
    ShooterStateSetPacket():BoardPacket(){}
    
	uint8_t shooter_state;
		
	virtual void Init(BoardPacketChannelIdType _channelCanId, uint8_t _id)
    {
        BoardPacket::Init(_channelCanId, _id);

        shooter_state = 0;
    }
		
    virtual void OnPacketReceived();
    virtual void SendPacket();
    template<typename Stream> void SerializePacket(Stream &stream);
};

class TestBoardPacket : public BoardPacket
{
public:
	TestBoardPacket(){}
    uint8_t m_testData;

    virtual void Init(BoardPacketChannelIdType _channelCanId, uint8_t _id)
    {
        BoardPacket::Init(_channelCanId, _id);

        m_testData = 0x00;
    }

    virtual void OnPacketReceived();
    virtual void SendPacket();
    template<typename Stream> void SerializePacket(Stream &stream);
};

class RobotStatePacket : public BoardPacket
{
public:
    RobotStatePacket(){}

    uint8_t chassis_level;
	uint8_t shooter_level;
	uint16_t shooter_heat;

	bool receive_flag;

	virtual void Init(BoardPacketChannelIdType _channelCanId, uint8_t _id)
    {
        BoardPacket::Init(_channelCanId, _id);

        chassis_level = 0;
		shooter_level = 0;
		shooter_heat = 100;
    }
	
    virtual void OnPacketReceived();
    virtual void SendPacket();
    template<typename Stream> void SerializePacket(Stream &stream);
};

class BoardPacketManager : public PacketManager, public CanMsgHandler
{
private:
    StreamChannel m_StreamChannel[BOARD_CHANNEL_NUM];
    CAN_TypeDef *m_pCan;

    TestBoardPacket m_testPacket;

	RobotStatePacket m_robot_state_packet;

    ChassisCtrlMsg m_ctrlMsg;
    GimbalRefMsg m_gimbalRefMsg;
    GraspCtrlMsg m_graspctrlMsg;
    PushrodCtrlMsg m_pushrodCtrlMsg;
    ChargeCtrlMsg m_chargeCtrlMsg;
    ServerCtrlMsg m_serverCtrlMsg;
    ActuatorRefMsg m_actuatorRefMsg;

    

protected:
    virtual bool FlushSendBufferLow();

public:
    BoardPacketManager();

    TestBoardPacket& GetTestPacket() { return m_testPacket; }

    ChassisCtrlMsg& GetChassisCtrlMsg() { return m_ctrlMsg; }
    GimbalRefMsg& GetGimbalRefMsg() { return m_gimbalRefMsg; }
    GraspCtrlMsg& GetGraspCtrlMsg() { return m_graspctrlMsg; }
    PushrodCtrlMsg& GetPushrodCtrlMsg() { return m_pushrodCtrlMsg; }
    ServerCtrlMsg& GetServerCtrlMsg() { return m_serverCtrlMsg; }
    ActuatorRefMsg& GetActuatorRefMsg() { return m_actuatorRefMsg; }
    ChargeCtrlMsg& GetChargeCtrlMsg() {return m_chargeCtrlMsg; }

	RobotStatePacket& GetRobotStatePacket () {return m_robot_state_packet;}
	
    virtual void Update();
    virtual void Init();
    virtual void HandleNewCanRxMsg(CanRxMsg *_msg);

    static BoardPacketManager *Instance()
    {
        static BoardPacketManager instance;
        return &instance;
    }
};

#endif
