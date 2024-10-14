#include "JudgeSystem.hpp"

const uint32_t JudgeSystem::Flush_Tick = 100;
const uint16_t JudgeSystem::s_Tick = 1000;

void JudgeSystem::Init()
{
    bsp_judge_init();
    m_serverCtrlMsg = &BoardPacketManager::Instance()->GetServerCtrlMsg();
    m_actuatorRefMsg = &BoardPacketManager::Instance()->GetActuatorRefMsg();

    m_pBS = BatterySystem::Instance();
    m_pPushrod = Pushrod::Instance();
}

void JudgeSystem::Update()
{
    uint8_t _currentRear = m_RxQueueRear;

    while(m_RxQueueFront != _currentRear)
    {
        uint8_t _data = m_RxQueue[m_RxQueueFront];

        if(m_FrameBufferLen < JS_FRAME_BUFFER_LEN)
        {
            m_FrameBuffer[m_FrameBufferLen] = _data;
            ++m_FrameBufferLen;
            if (m_FrameBufferLen > 0 && m_FrameBuffer[m_FrameBufferLen - 1] == '}')
            {
                HandlePacket();
                m_FrameBufferLen = 0;
            }
        }
            
        m_RxQueueFront = (m_RxQueueFront + 1) % JS_RX_QUEUE_LEN;
    }

    Charge_Update();


    // send actuator feedback
    actuator_success = m_actuatorRefMsg->GetSucess();
    if (actuator_success)
    {
        if (!charge_command)
        {
            Make_actuator_json();
        }
        m_actuatorRefMsg->SetSuccess(0);
    }

    if(Time::GetTick() % Flush_Tick == 0)
    {
        FlushBuffer();
    }
}

void JudgeSystem::HandlePacket()
{
    memcpy(m_CurrentFrame, m_FrameBuffer, m_FrameBufferLen);

    uint8_t* _ptr = m_CurrentFrame;

    receive_success = false;
    receive_success = Parse_json(_ptr,m_FrameBufferLen);

    if (receive_success)
    {
        if(m_Command == 2)
        {
            charge_command = true;
            RSOC_flag = false;
        } else
        {
            send_success = Make_receive_json();
            m_pPushrod->SetChargeState(2);
            charge_command = false;
        }
        m_serverCtrlMsg->SetID(m_goal);
        m_serverCtrlMsg->SetCommand(m_Command);
        m_serverCtrlMsg->SendMsg();
    }
}

void JudgeSystem::Charge_Update(void)
{
    RSOC = m_pBS->GetRSOC();
    is_charge = m_pBS->GetCharge();

    if (!start_flag)
    {
        free_state();
        start_flag = true;
    }

    if(RSOC < 20)
    {
        RSOC_flag = false;
        free_state_flag = true;
    }
    
    // send start charge 
    if (is_charge && !charge_flag)
    {
        Start_Charge();
        charge_flag = true;
        free_state_flag = true;
    }
    else if (!is_charge)
    {
        charge_flag = false;
        RSOC_flag = false;
    }

    if(free_state_flag)
    {
        count++;
        if(count == 10000)
        {
            if (RSOC > 30 && !RSOC_flag)
            {
                free_state();
                RSOC_flag = true;
            }
            count = 0;
            free_state_flag = false;
        }
    }
}

void JudgeSystem::FlushBuffer()
{
    if(bsp_judge_send_busy())
    {
        return;
    }

    if(m_SendBufferLen != 0)
    {
        bsp_judge_send(m_SendBuffer, m_SendBufferLen);
        m_SendBufferLen = 0;
    }
}

bool JudgeSystem::Add2SendBuffer(uint8_t* _pData, uint16_t _dataLen)
{
    if(m_SendBufferLen + _dataLen > JS_SEND_BUFFER_LEN)
    {
        return false;
    }

    memcpy(&m_SendBuffer[m_SendBufferLen], _pData, _dataLen);
    m_SendBufferLen +=_dataLen;

    return true;
}

bool JudgeSystem::Parse_json(uint8_t* _pData, uint16_t _dataLen)
{
    cJSON* json = cJSON_ParseWithLength((const char*)_pData, _dataLen);
    if(json != NULL)
    {
        cJSON* ptr = cJSON_GetObjectItem(json, "take_id"); 
        if(ptr != NULL)
        {
            strcpy(m_take_id, ptr->valuestring);
            m_goal = parseStringToUint8((char *)m_take_id);
        }

        ptr = cJSON_GetObjectItem(json, "give_id");
        if (ptr != NULL)
        {
            strcpy(m_give_id, ptr->valuestring);
            m_goal = parseStringToUint8((char *)m_give_id);
        }
        

        ptr = cJSON_GetObjectItem(json, "command");
        if (ptr != NULL)
        {
            if (strcmp(ptr->valuestring, "0") == 0)
            {
                m_Command = 0;
                takeorgive = 1;
            }
            else if (strcmp(ptr->valuestring, "1") == 0)
            {
                m_Command = 1;
                takeorgive = 2;
            }
            else if (strcmp(ptr->valuestring, "2") == 0)
            {
                m_Command = 2;
            }

        }

        ptr = cJSON_GetObjectItem(json, "chg_id");
        if(ptr != NULL)
        {
            strcpy(m_chg_id, ptr->valuestring);
        }

        ptr = cJSON_GetObjectItem(json, "task_id");
        if (ptr != NULL)
        {
            strcpy(m_task_id, ptr->valuestring);
        }

        cJSON_Delete(json);
    }

    return true;
}

bool JudgeSystem::Make_receive_json(void)
{
    const char *code = "20000";
    const char *robot_id = "002";
    const char *msg = "receive";
    const char *success = receive_success ? "\"true" : "\"false";

    m_CurrentSendBufferLen = 0;

    if (!append_string("{\"code\":\"") ||
        !append_string(code) ||
        !append_string("\",\"robot_id\":\"") ||
        !append_string(robot_id) ||
        !append_string("\",\"msg\":\"") ||
        !append_string(msg) ||
        !append_string("\",\"success\":") ||
        !append_string(success) ||
        !append_string("\"}"))
    {
        return false;
    }

    if (m_CurrentSendBufferLen < JS_FRAME_BUFFER_LEN)
    {
        m_CurrentSendBuffer[m_CurrentSendBufferLen] = '\0';
    }
    else
    {
        return false;
    }

    Add2SendBuffer(m_CurrentSendBuffer, m_CurrentSendBufferLen);
    return true;
}

bool JudgeSystem::Make_actuator_json(void)
{
    const char *code_take = "20001";
    const char *code_give = "20002";
    const char *robot_id = "002";
    const char *msg = "actuator";
    const char *success = actuator_success ? "\"true" : "\"false";

    m_CurrentSendBufferLen = 0;

    if (takeorgive == 1)
    {
        if (!append_string("{\"code\":\"") ||
            !append_string(code_take))
        {
            return false;
        }
    }
    else if (takeorgive == 2)
    {
        if (!append_string("{\"code\":\"") ||
            !append_string(code_give))
        {
            return false;
        }
    }

    if (!append_string("\",\"robot_id\":\"") ||
        !append_string(robot_id) ||
        !append_string("\",\"msg\":\"") ||
        !append_string(msg) ||
        !append_string("\",\"chg_id\":\"") ||
        !append_string((char *)m_chg_id) ||
        !append_string("\",\"task_id\":\"") ||
        !append_string((char *)m_task_id))
    {
        return false;
    }

    if (takeorgive == 1)
    {
        if (!append_string("\",\"take_id\":\"") ||
            !append_string((char *)m_take_id))
        {
            return false;
        }
    }
    else if (takeorgive == 2)
    {
        uint8_t RSOC = m_pBS->GetRSOC();
        if(RSOC < 20)
        {
            if (!append_string("\",\"give_id\":\"") ||
                !append_string((char *)m_give_id) ||
                !append_string("\",\"is_charge\":\"") ||
                !append_string("1"))
                {
                    return false;
                }
        }else
        {
            if (!append_string("\",\"give_id\":\"") ||
                !append_string((char *)m_give_id) ||
                !append_string("\",\"is_charge\":\"") ||
                !append_string("0"))
                {
                    return false;
                }
        }
    }

    if (!append_string("\",\"success\":") ||
        !append_string(success) ||
        !append_string("\"}"))
    {
        return false;
    }

    if (m_CurrentSendBufferLen < JS_FRAME_BUFFER_LEN)
    {
        m_CurrentSendBuffer[m_CurrentSendBufferLen] = '\0';
    }
    else
    {
        return false;
    }

    Add2SendBuffer(m_CurrentSendBuffer, m_CurrentSendBufferLen);
    send_success = true;
    return true;
}

uint8_t JudgeSystem::parseStringToUint8(char *str)
{
    char *dashm_CurrentSendBufferLen = str;
    while (*dashm_CurrentSendBufferLen && *dashm_CurrentSendBufferLen != '-')
    {
        dashm_CurrentSendBufferLen++;
    }

    if (*dashm_CurrentSendBufferLen == '-' && *(dashm_CurrentSendBufferLen + 1) != '\0')
    {
        int result = 0;
        char *numberStr = dashm_CurrentSendBufferLen + 1;

        while (*numberStr)
        {
            if (*numberStr >= '0' && *numberStr <= '9')
            {
                result = result * 10 + (*numberStr - '0');
            }
            numberStr++;
        }

        return static_cast<uint8_t>(result);
    }
    else
    {
        return 0;
    }
}

bool JudgeSystem::free_state(void)
{
    const char *code = "20010";
    const char *robot_id = "002";
    const char *robot_sts = "0";
    const char *msg = "free state";
    const char *success = "true";

    m_CurrentSendBufferLen = 0;

    if (!append_string("{\"code\":\"") ||
        !append_string(code) ||
        !append_string("\",\"robot_id\":\"") ||
        !append_string(robot_id) ||
        !append_string("\",\"robot_sts\":\"") ||
        !append_string(robot_sts) ||
        !append_string("\",\"msg\":\"") ||
        !append_string(msg) ||
        !append_string("\",\"success\":\"") ||
        !append_string(success) ||
        !append_string("\"}"))
    {
        return false;
    }

    if (m_CurrentSendBufferLen < JS_FRAME_BUFFER_LEN)
    {
        m_CurrentSendBuffer[m_CurrentSendBufferLen] = '\0';
    }
    else
    {
        return false;
    }

    Add2SendBuffer(m_CurrentSendBuffer, m_CurrentSendBufferLen);
    return true;
}

bool JudgeSystem::Ready_Charge(void)
{
    const char *code = "20012";
    const char *robot_id = "002";
    const char *is_charge = "0";
    const char *success = "true";

    m_CurrentSendBufferLen = 0;

    if (!append_string("{\"code\":\"") ||
        !append_string(code) ||
        !append_string("\",\"robot_id\":\"") ||
        !append_string(robot_id) ||
        !append_string("\",\"is_charge\":\"") ||
        !append_string(is_charge) ||
        !append_string("\",\"success\":\"") ||
        !append_string(success) ||
        !append_string("\"}"))
    {
        return false;
    }

    if (m_CurrentSendBufferLen < JS_FRAME_BUFFER_LEN)
    {
        m_CurrentSendBuffer[m_CurrentSendBufferLen] = '\0';
    }
    else
    {
        return false;
    }

    Add2SendBuffer(m_CurrentSendBuffer, m_CurrentSendBufferLen);
    return true;
}

bool JudgeSystem::Error_feedback(void)
{
    const char *code = "20013";
    const char *robot_id = "002";
    const char *msg = "Actuator Error";
    const char *success = "false";

    m_CurrentSendBufferLen = 0;

    if (!append_string("{\"code\":\"") ||
        !append_string(code) ||
        !append_string("\",\"robot_id\":\"") ||
        !append_string(robot_id) ||
        !append_string("\",\"msg\":\"") ||
        !append_string(msg) ||
        !append_string("\",\"success\":\"") ||
        !append_string(success) ||
        !append_string("\"}"))
    {
        return false;
    }

    if (m_CurrentSendBufferLen < JS_FRAME_BUFFER_LEN)
    {
        m_CurrentSendBuffer[m_CurrentSendBufferLen] = '\0';
    }
    else
    {
        return false;
    }

    Add2SendBuffer(m_CurrentSendBuffer, m_CurrentSendBufferLen);
    return true;
}

bool JudgeSystem::Start_Charge(void)
{
    const char *code = "20012";
    const char *robot_id = "002";
    const char *is_charge = "1";
    const char *success = "true";

    m_CurrentSendBufferLen = 0;

    if (!append_string("{\"code\":\"") ||
        !append_string(code) ||
        !append_string("\",\"robot_id\":\"") ||
        !append_string(robot_id) ||
        !append_string("\",\"is_charge\":\"") ||
        !append_string(is_charge) ||
        !append_string("\",\"success\":\"") ||
        !append_string(success) ||
        !append_string("\"}"))
    {
        return false;
    }

    if (m_CurrentSendBufferLen < JS_FRAME_BUFFER_LEN)
    {
        m_CurrentSendBuffer[m_CurrentSendBufferLen] = '\0';
    }
    else
    {
        return false;
    }

    Add2SendBuffer(m_CurrentSendBuffer, m_CurrentSendBufferLen);
    return true;
}

bool JudgeSystem::append_string(const char *str)
{
    size_t len = strlen(str);
    if (m_CurrentSendBufferLen + len < JS_FRAME_BUFFER_LEN)
    {
        memcpy(&m_CurrentSendBuffer[m_CurrentSendBufferLen], str, len);
        m_CurrentSendBufferLen += len;
        return true;
    }
    else
    {
        return false;
    }
}
