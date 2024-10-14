#ifndef HOSTPACKET_HPP
#define HOSTPACKET_HPP

#include "stm32f4xx.h"
#include "string.h"
#include "Time.hpp"
#include "Stream.hpp"
#include "JudgeMsgHandler.hpp"
#include "bsp_host_com.h"
#include "Crc.hpp"

#define BC_RX_QUEUE_LEN 512
#define BC_FRAME_BUFFER_LEN 150
#define BC_SEND_BUFFER_LEN 512

class BatterySystem
{
private:
    uint8_t m_RxQueue[BC_RX_QUEUE_LEN];
    uint8_t m_FrameBuffer[BC_FRAME_BUFFER_LEN];
    uint8_t m_CurrentFrame[BC_FRAME_BUFFER_LEN];
    uint8_t m_CurrentSendBuffer[BC_FRAME_BUFFER_LEN];
    uint8_t m_SendBuffer[BC_SEND_BUFFER_LEN];
    uint8_t m_RxQueueFront;
    uint8_t m_RxQueueRear;
    uint8_t m_FrameBufferLen;
    uint8_t m_SendSeq;
    uint16_t m_SendBufferLen;
    uint16_t m_CurrentSendBufferLen;
    uint32_t m_headerperS;
    uint32_t m_hs;
    uint8_t m_RSOC;
    uint16_t m_voltage_10mV;
    int16_t m_current_A;
    bool is_charge;

    const static uint32_t Flush_Tick;

    BatterySystem() : m_RxQueueFront(0),
                      m_RxQueueRear(0),
                      m_FrameBufferLen(0),
                      m_SendSeq(0),
                      m_SendBufferLen(0),
                      m_CurrentSendBufferLen(0),
                      m_headerperS(0),
                      m_hs(0),
                      m_RSOC(0),
                      m_voltage_10mV(0),
                      m_current_A(0),
                      is_charge(false)            
    {
    }

    void HandlePacket();
    void FlushBuffer();
    bool Add2SendBuffer(uint8_t *_pData, uint8_t _dataLen);
    bool VerifyFrame();

public:
    static BatterySystem *Instance()
    {
        static BatterySystem instance;
        return &instance;
    }

    void Enqueue(uint8_t _data)
    {
        m_RxQueue[m_RxQueueRear] = _data;
        m_RxQueueRear = (m_RxQueueRear + 1) % BC_RX_QUEUE_LEN;
    }

    uint8_t GetRSOC() {return m_RSOC;}
    bool GetCharge() {return is_charge;}

    void Init();
    void Update();
};

#endif
