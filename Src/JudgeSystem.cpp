#include "JudgeSystem.hpp"
#include "Time.hpp"
#include "JudgeMsgHandler.hpp"

const uint8_t JudgeSystem::Magic_Number = 0xA5;
const uint8_t JudgeSystem::Header_Length = 5;
const uint32_t JudgeSystem::Flush_Tick = 100;

void JudgeSystem::Init()
{
    bsp_judge_init();
}

void JudgeSystem::Update()
{
    if(Time::GetTick() % 1000 == 0)
    {
        m_headerperS = m_hs;
        m_hs = 0;
    }
    uint8_t _currentRear = m_RxQueueRear;

    while(m_RxQueueFront != _currentRear)
    {
        uint8_t _data = m_RxQuene[m_RxQueueFront];

        if(m_FrameBufferLen < JS_FRAME_BUFFER_LEN)
        {
            m_FrameBuffer[m_FrameBufferLen] = _data;
            ++m_FrameBufferLen;
            if(m_FrameBufferLen == m_ExpectedLen)
            {
                if(Crc::VerifyCrc16CheckSum(m_FrameBuffer, m_FrameBufferLen))
                {
                    HandlePacket();
                }
            }
        }
            
        if(m_CheckingHeader)
        {
            m_HeaderBuffer[m_HeaderBufferLen] = _data;
            ++m_HeaderBufferLen;

            if(m_HeaderBufferLen == Header_Length)
            {
                m_HeaderBufferLen = 0;
                m_CheckingHeader = false;

                if(Crc::VerifyCrc8CheckSum(m_HeaderBuffer, Header_Length))
                {
                    uint8_t* _ptr = m_HeaderBuffer + 1;
                    m_ExpectedLen = ArrayStreamHelper::ReadUint16(_ptr, false) + 9;
                    //if(Crc::VerifyCrc16CheckSum(m_FrameBuffer, m_FrameBufferLen - Header_Length))
                    //HandlePacket();

                    memcpy(m_FrameBuffer, m_HeaderBuffer, Header_Length);
                    m_FrameBufferLen = Header_Length;
                }
            }
        }

        if(_data == Magic_Number)
        {
            m_CheckingHeader = true;
            m_HeaderBufferLen = 0;
            m_HeaderBuffer[m_HeaderBufferLen] = _data;
            ++m_HeaderBufferLen;
        }

        m_RxQueueFront = (m_RxQueueFront + 1) % JS_RX_QUEUE_LEN;
    }

    if(Time::GetTick() % Flush_Tick == 0)
    {
        FlushBuffer();
    }
}

void JudgeSystem::HandlePacket()
{
    memcpy(m_CurrentFrame, m_FrameBuffer, m_FrameBufferLen);

    uint8_t* _ptr = m_CurrentFrame + Header_Length;
    uint16_t _packetId = ArrayStreamHelper::ReadUint16(_ptr);

    switch (_packetId)
    {
    case JPT_GameStatus:
        memcpy(&GameStatus, _ptr, sizeof(GameStatus));
        break;
    
    case JPT_GameResult:
        memcpy(&GameResult, _ptr, sizeof(GameResult));
        break;

    case JPT_GameRobotHp:
        memcpy(&GameRobotHp, _ptr, sizeof(GameRobotHp));
        break;
            
    case JPT_DartStatus:
        memcpy(&DartStatus, _ptr, sizeof(DartStatus));
        break;
            
    case JPT_IcraBuffDebuffZoneStatus:
        memcpy(&IcraBuffDebuffZoneStatus, _ptr, sizeof(IcraBuffDebuffZoneStatus));
        break;
            
    case JPT_EventData:
        memcpy(&EventData, _ptr, sizeof(EventData));
        break;
            
    case JPT_SupplyProjectileAction:
        memcpy(&SupplyProjectileAction, _ptr, sizeof(SupplyProjectileAction));
        break;
            
    case JPT_RefereeWarning:
        memcpy(&RefereeWarning, _ptr, sizeof(RefereeWarning));
        break;
            
    case JPT_DartRemainingTime:
        memcpy(&DartRemainingTime, _ptr, sizeof(DartRemainingTime));
        break;
            
    case JPT_GameRobotStatus:
        memcpy(&GameRobotStatus, _ptr, sizeof(GameRobotStatus));
        break;
            
    case JPT_PowerHeatData:
        memcpy(&PowerHeatData, _ptr, sizeof(PowerHeatData));
        break;
    
    case JPT_GameRobotPos:
        memcpy(&GameRobotPos, _ptr, sizeof(GameRobotPos));
        break;
            
    case JPT_Buff:
        memcpy(&Buff, _ptr, sizeof(Buff));
        break;
            
    case JPT_AerialRobotEnergy:
        memcpy(&AerialRobotEnergy, _ptr, sizeof(AerialRobotEnergy));
        break;
            
    case JPT_RobotHurt:
        memcpy(&RobotHurt, _ptr, sizeof(RobotHurt));
        break;

    case JPT_ShootData:
        memcpy(&ShootData, _ptr, sizeof(ShootData));
        break;

    case JPT_BulletRemaining:
        memcpy(&BulletRemaining, _ptr, sizeof(BulletRemaining));
        break;

    case JPT_RfidStatus:
        memcpy(&RfidStatus, _ptr, sizeof(RfidStatus));
        break;

    case JPT_DartClientCmd:
        memcpy(&DartClientCmd, _ptr, sizeof(DartClientCmd));
        break;
    
    case JPT_Custom:
        memcpy(&CustomHeader, _ptr, sizeof(CustomHeader));
        HandleCustomPacket();
        break;

    default:
        break;
    }

    m_PacketLastUpdateTick.Modify((uint32_t)_packetId, Time::GetTick());

    ++m_hs;
}

void JudgeSystem::HandleCustomPacket()
{
    uint16_t _len = 0;
    memcpy(&_len, &m_CurrentFrame[1], sizeof(uint16_t));
    _len -= sizeof(JS_CustomHeader);

    JudgeMsgHandler** _handler = m_CustomMsgHandlerTable.Search(CustomHeader.data_cmd_id);

    if(_handler != nullptr)
    {
        (*_handler)->HandleNewMsg(&m_CurrentFrame[Header_Length + sizeof(uint16_t) + sizeof(JS_CustomHeader)], _len);
    }
}

void JudgeSystem::FlushBuffer()
{
    if(bsp_judge_send_busy())
    {
        return;
    }

    if(m_GraphicBufferLen > 0 || m_CharBufferLen > 0)
    {
        AddGraphicElement();
    }

    if(m_SendBufferLen != 0)
    {
        bsp_judge_send(m_SendBuffer, m_SendBufferLen);
        m_SendBufferLen = 0;
    }
}

bool JudgeSystem::Add2SendBuffer(RobotIdType _receiverId, uint16_t _frameId, uint8_t* _pData, uint16_t _dataLen)
{
    if(m_SendBufferLen + _dataLen + 15 > JS_SEND_BUFFER_LEN)
    {
        return false;
    }

    uint8_t* _pStart = &m_SendBuffer[m_SendBufferLen];

    uint8_t _header[7];

    _header[0] = Magic_Number;
    uint16_t _realLen = _dataLen + sizeof(JS_CustomHeader);
    memcpy(&_header[1], &_realLen, sizeof(uint16_t));
    _header[3] = m_SendSeq ++;
    _header[4] = Crc::Get_CRC8_Check_Sum(_header, 4, Crc::CRC8_INIT);

    uint16_t _jpt_customId = JPT_Custom;
    memcpy(&_header[5], &_jpt_customId, sizeof(uint16_t));

    memcpy(&m_SendBuffer[m_SendBufferLen], _header, sizeof(_header));
    m_SendBufferLen += sizeof(_header);

    JS_CustomHeader _customHeader;
    _customHeader.data_cmd_id = _frameId;
    _customHeader.sender_ID = GameRobotStatus.robot_id;
    _customHeader.receiver_ID = _receiverId;
    memcpy(&m_SendBuffer[m_SendBufferLen], &_customHeader, sizeof(_customHeader));
    m_SendBufferLen += sizeof(_customHeader);

    memcpy(&m_SendBuffer[m_SendBufferLen], _pData, _dataLen);
    m_SendBufferLen +=_dataLen;

    uint16_t _crc16 = Crc::Get_CRC16_Check_Sum(
        _pStart,
        sizeof(_header) + _dataLen + sizeof(_customHeader),
        Crc::CRC16_INIT
    );

    memcpy(&m_SendBuffer[m_SendBufferLen], &_crc16, sizeof(_crc16));
    m_SendBufferLen +=sizeof(_crc16);

    return true;
}

JudgeSystem::RobotIdType JudgeSystem::GetClientId()
{
    uint8_t _id = GameRobotStatus.robot_id;

    RobotIdType _clientId;
    switch (_id)
    {
    case RIT_RedHero:
        _clientId = RIT_ClientRedHero;
        break;

    case RIT_RedEngineer:
        _clientId = RIT_ClientRedEngineer;
        break;

    case RIT_RedInfantry_3:
        _clientId = RIT_ClientRedInfantry_3;
        break;

    case RIT_RedInfantry_4:
        _clientId = RIT_ClientRedInfantry_4;
        break;                
    
    case RIT_RedInfantry_5:
        _clientId = RIT_ClientRedInfantry_5;
        break;

    case RIT_RedDrone:
        _clientId = RIT_ClientRedDrone;
        break;

    case RIT_BluHero:
        _clientId = RIT_ClientBluHero;
        break;

    case RIT_BluEngineer:
        _clientId = RIT_ClientBluEngineer;
        break;

    case RIT_BluInfantry_3:
        _clientId = RIT_ClientBluInfantry_3;
        break;

    case RIT_BluInfantry_4:
        _clientId = RIT_ClientBluInfantry_4;
        break;                
    
    case RIT_BluInfantry_5:
        _clientId = RIT_ClientBluInfantry_5;
        break;

    case RIT_BluDrone:
        _clientId = RIT_ClientBluDrone;
        break;

    default:
        break;
    }
		
	return _clientId;
}

void JudgeSystem::DeleteLayer(DelectType _op, uint8_t _layer)
{
    if(0xFF == _layer && DT_DeleteOne == _op)
    {
        _op = DT_Null;
    }
    JS_DeleteGraphic _deleteG;
    _deleteG.operate_tpye = _op;
    _deleteG.layer = _layer;

    uint8_t _buffer[2];

    memcpy(_buffer, &_deleteG, 2);

    Add2SendBuffer(GetClientId(), DOP_Delete, _buffer, 2);

    m_HasGraphicOp = true;
}

void JudgeSystem::AddGraphicElement()
{
    if(m_HasGraphicOp)
    {
        m_HasGraphicOp = false;
        m_GraphicBufferLen = 0;
        return;
    }

    if(m_SendBufferLen != 0)
    {
        m_GraphicBufferLen = 0;
        return;
    }
    
    if(AddSevenGraphicElement())
    {
        return;
    }
    if(AddFiveGraphicElement())
    {
        return;
    }
    if(AddTwoGraphicElement())
    {
        return;
    }
    if(AddOneGraphicElement())
    {
        return;
    }
    if(AddCharGraphicElement())
    {
        return;
    }
}

bool JudgeSystem::AddOneGraphicElement()
{
    if(m_GraphicBufferLen < 1)
    {
        return false;
    }

    uint8_t _size = 15;

    if(_size + 15 > (JS_SEND_BUFFER_LEN - m_SendBufferLen))
    {
        return false;
    }

    uint8_t _buffer[15];

    memcpy(_buffer, &m_GraphicBuffer[m_GraphicBufferLen - 1], _size);
    Add2SendBuffer(GetClientId(), DOP_DrawOne, _buffer, _size);

    m_GraphicBufferLen -= 1;

    return true;
}

bool JudgeSystem::AddTwoGraphicElement()
{
    if(m_GraphicBufferLen < 2)
    {
        return false;
    }

    uint8_t _size = 30;

    if(_size + 15 > (JS_SEND_BUFFER_LEN - m_SendBufferLen))
    {
        return false;
    }

    uint8_t _buffer[30];

    memcpy(_buffer, &m_GraphicBuffer[m_GraphicBufferLen - 2], _size);
    Add2SendBuffer(GetClientId(), DOP_DrawTwo, _buffer, _size);

    m_GraphicBufferLen -= 2;

    return true;
}

bool JudgeSystem::AddFiveGraphicElement()
{
    if(m_GraphicBufferLen < 5)
    {
        return false;
    }

    uint8_t _size = 75;

    if(_size + 15 > (JS_SEND_BUFFER_LEN - m_SendBufferLen))
    {
        return false;
    }

    uint8_t _buffer[75];

    memcpy(_buffer, &m_GraphicBuffer[m_GraphicBufferLen - 5], _size);
    Add2SendBuffer(GetClientId(), DOP_DrawFive, _buffer, _size);

    m_GraphicBufferLen -= 5;

    return true;
}

bool JudgeSystem::AddSevenGraphicElement()
{
    if(m_GraphicBufferLen < 7)
    {
        return false;
    }

    uint8_t _size = 105;

    if(_size + 15 > (JS_SEND_BUFFER_LEN - m_SendBufferLen))
    {
        return false;
    }

    uint8_t _buffer[105];

    memcpy(_buffer, &m_GraphicBuffer[m_GraphicBufferLen - 7], _size);
    Add2SendBuffer(GetClientId(), DOP_DrawSeven, _buffer, _size);

    m_GraphicBufferLen -= 7;
    
    return true;
}

bool JudgeSystem::AddCharGraphicElement()
{
    uint8_t _size = 45;
    
    if(m_CharBuffer == 0)
    {
        return false;
    }

    if(_size + 15 > (JS_SEND_BUFFER_LEN - m_SendBufferLen))
    {
        return false;
    }

    uint8_t _buffer[45];

    memcpy(_buffer, &m_CharBuffer[m_CharBufferLen - 1], _size);
    Add2SendBuffer(GetClientId(), DOP_DrawChar, _buffer, _size);

    -- m_CharBufferLen;

    return true;
}

bool JudgeSystem::ModfiyShapeOnClient(JS_GraphicData* _pData)
{
    if(m_GraphicBufferLen >= 16)
    {
        return false;
    }

    memcpy(&m_GraphicBuffer[m_GraphicBufferLen], _pData, sizeof(JS_GraphicData));
    ++m_GraphicBufferLen;

    return true;
}

bool JudgeSystem::ModfiyCharOnClient(JS_ClientCustomCharacter* _pData)
{
    if(m_CharBufferLen >= 8)
    {
        return false;
    }

    memcpy(&m_CharBuffer[m_CharBufferLen], _pData, sizeof(JS_ClientCustomCharacter));
    ++m_CharBufferLen;

    return true;
}
