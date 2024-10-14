#include "BatterySystem.hpp"

const uint8_t START_BYTE = 0xDD;
const uint8_t READ_BYTE = 0xA5;
const uint8_t CMD_CODE = 0x03;
const uint8_t STOP_BYTE = 0x77;
const uint32_t BatterySystem::Flush_Tick = 100;

void BatterySystem::Init()
{
    bsp_host_com_init();
}

void BatterySystem::Update()
{
    if (Time::GetTick() % 1000 == 0)
    {
        m_headerperS = m_hs;
        m_hs = 0;
    }
    uint8_t _currentRear = m_RxQueueRear;

    while (m_RxQueueFront != _currentRear)
    {
        uint8_t _data = m_RxQueue[m_RxQueueFront];

        if (m_FrameBufferLen < BC_FRAME_BUFFER_LEN)
        {
            m_FrameBuffer[m_FrameBufferLen] = _data;
            ++m_FrameBufferLen;
            if (m_FrameBufferLen > 2 && m_FrameBuffer[m_FrameBufferLen - 1] == STOP_BYTE)
            {
                if (VerifyFrame())
                {
                    HandlePacket();
                }
                m_FrameBufferLen = 0;
            }
        }

        m_RxQueueFront = (m_RxQueueFront + 1) % BC_RX_QUEUE_LEN;
    }

    if (Time::GetTick() % Flush_Tick == 0)
    {   
        FlushBuffer();
    }

    if (Time::GetTick() % 1000 == 0)
    {
        uint8_t *emptyData = nullptr;
        Add2SendBuffer(emptyData, 0);
    }
}

bool BatterySystem::VerifyFrame()
{
    if (m_FrameBufferLen < 7) // ��С֡��
    {
        return false;
    }

    if (m_FrameBuffer[0] != 0xDD)
    {
        return false;
    }

    if (m_FrameBuffer[m_FrameBufferLen - 1] != 0x77)
    {
        return false;
    }
    return true;
}

void BatterySystem::HandlePacket()
{
    memcpy(m_CurrentFrame, m_FrameBuffer, m_FrameBufferLen);

    uint8_t *_ptr = m_CurrentFrame + 4;

    uint8_t *ptr = _ptr + 9 * sizeof(uint16_t) + sizeof(uint8_t);
    m_RSOC = ArrayStreamHelper::ReadUint8(ptr, false);

    m_voltage_10mV = ArrayStreamHelper::ReadUint16(_ptr);
    m_current_A = ArrayStreamHelper::ReadInt16(_ptr);

    if (m_current_A & 0x8000) // ������λΪ 1����λΪ 100 mA
    {
        m_current_A = m_current_A*10;
    }

    // judege charge or discharge
    if(m_current_A > 0)
    {
        is_charge = true;
    }else
    {
        is_charge = false;
    }
}

void BatterySystem::FlushBuffer()
{
    if (bsp_host_com_send_busy())
    {
        return;
    }

    if (m_SendBufferLen != 0)
    {
        bsp_host_com_send(m_SendBuffer, m_SendBufferLen);
        m_SendBufferLen = 0;
    }
}

bool BatterySystem::Add2SendBuffer(uint8_t *_pData, uint8_t _dataLen)
{
    uint8_t totalLength = 1 + 1 + 1 + 1 + _dataLen + 2 + 1; // ��ʼλ����ȡ/д��λ�������롢���ȡ��������ݡ�У���ֹͣλ

    // ��鷢�ͻ������Ƿ����㹻�Ŀռ�
    if (m_SendBufferLen + totalLength > BC_SEND_BUFFER_LEN)
    {
        return false;
    }

    uint8_t *_pStart = &m_SendBuffer[m_SendBufferLen];
    uint8_t offset = 0;

    _pStart[offset++] = START_BYTE;
    _pStart[offset++] = READ_BYTE;
    _pStart[offset++] = CMD_CODE;
    _pStart[offset++] = _dataLen;

    if (_dataLen > 0)
    {
        memcpy(&_pStart[offset], _pData, _dataLen);
        offset += _dataLen;
    }

    _pStart[offset++] = 0xFF;
    _pStart[offset++] = 0xFD;

    // ֹͣλ
    _pStart[offset++] = STOP_BYTE;

    // ���»���������
    m_SendBufferLen += offset;

    return true;
}
