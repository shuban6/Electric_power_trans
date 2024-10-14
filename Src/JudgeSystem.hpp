#ifndef JUDGESYSTEM_HPP
#define JUDGESYSTEM_HPP

#include "stm32f4xx.h"
#include "string.h"
#include "Time.hpp"
#include "Stream.hpp"
#include "HashTable.hpp"
#include "JudgeMsgHandler.hpp"
#include "bsp_judge.h"
#include "cJSON.h"
#include "BoardPacket.hpp"
#include "ServerCtrlMsg.hpp"
#include "ActuatorRefMsg.hpp"
#include "BatterySystem.hpp"
#include "Pushrod.hpp"

#define JS_RX_QUEUE_LEN 512
#define JS_FRAME_BUFFER_LEN 150
#define JS_SEND_BUFFER_LEN 512

enum class ChargeState
{
    Idle,      // ³õÊ¼×´Ì¬
    Charging,  // ³äµç×´Ì¬
    FreeState, // ×ÔÓÉ×´Ì¬
    Grasping,  // È¡×®×´Ì¬
    Releasing, // ·Å×®×´Ì¬
    CheckRSOC  // ¼ì²éÊ£ÓàµçÁ¿×´Ì¬
};

class JudgeSystem
{
private:
    uint8_t m_RxQueue[JS_RX_QUEUE_LEN];
    uint8_t m_FrameBuffer[JS_FRAME_BUFFER_LEN];
    uint8_t m_CurrentFrame[JS_FRAME_BUFFER_LEN];
    uint8_t m_CurrentSendBuffer[JS_FRAME_BUFFER_LEN];
    uint8_t m_SendBuffer[JS_SEND_BUFFER_LEN];
    char m_take_id[8];
    char m_give_id[8];
    char m_chg_id[15];
    char m_task_id[10];
    uint8_t m_RxQueueFront;
    uint8_t m_RxQueueRear;
    uint8_t m_FrameBufferLen;
    uint8_t m_SendSeq;
    uint16_t m_SendBufferLen;
    uint16_t m_CurrentSendBufferLen;
    uint32_t m_headerperS;
    uint32_t m_s;
    uint8_t m_Command;
    uint8_t m_goal;
    bool receive_success;
    bool send_success;
    bool actuator_success;
    uint8_t takeorgive;
    bool charge_command;
    bool is_charge;
    bool charge_flag;
    bool RSOC_flag;
    bool free_state_flag;
    bool start_flag;
    bool restar_charge_flag;
    uint16_t count;
    uint8_t RSOC;
    ChargeState lastState;
    ChargeState currentState;

    BatterySystem *m_pBS;
    Pushrod *m_pPushrod;

    const static uint32_t Flush_Tick;
    const static uint16_t s_Tick;

    JudgeSystem() : m_RxQueueFront(0),
                    m_RxQueueRear(0),
                    m_FrameBufferLen(0),
                    m_SendSeq(0),
                    m_SendBufferLen(0),
                    m_CurrentSendBufferLen(0),
                    m_headerperS(0),
                    m_s(0),
                    m_Command(2),
                    m_goal(0),
                    receive_success(0),
                    send_success(false),
                    actuator_success(0),
                    takeorgive(0),
                    charge_command(false),
                    is_charge(false),
                    charge_flag(true),
                    RSOC_flag(false),
                    free_state_flag(false),
                    start_flag(false),
                    restar_charge_flag(false),
                    count(0),
                    RSOC(0),
                    lastState(Idle),
                    currentState(Idle)
    {}

    void HandlePacket();
    void FlushBuffer();
    
public:
    ServerCtrlMsg *m_serverCtrlMsg;
    ActuatorRefMsg *m_actuatorRefMsg;

    static JudgeSystem* Instance()
    {
        static JudgeSystem instance;
        return &instance;
    }

    void Enqueue(uint8_t _data)
    {
        m_RxQueue[m_RxQueueRear] = _data;
        m_RxQueueRear = (m_RxQueueRear + 1) % JS_RX_QUEUE_LEN;
    }

    void Init();
    void Update();
    bool Add2SendBuffer(uint8_t* _pData, uint16_t _dataLen);
    bool Parse_json(uint8_t* _pData, uint16_t _dataLen);
    bool Make_receive_json(void);
    bool Make_actuator_json(void);
    bool free_state(void);
    bool Ready_Charge(void);
    uint8_t parseStringToUint8(char *str);
    bool Error_feedback(void);
    bool Start_Charge(void);
    bool append_string(const char *str);
    void Charge_Update(void);
    void InitializeSystem(void);
    void UpdateSensorStatus(void);
    void HandleChargingState(void);
    void HandleFreeState(void);
    void HandleGraspState(void);
    void HandleReleaseState(void);
    void HandleCheckRSOCState(void);

};

#endif
