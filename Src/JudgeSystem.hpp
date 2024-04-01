#ifndef JUDGESYSTEM_HPP
#define JUDGESYSTEM_HPP

#include "stm32f4xx.h"
#include "string.h"
#include "Time.hpp"
#include "Stream.hpp"
#include "Crc.hpp"
#include "HashTable.hpp"
#include "JudgeMsgHandler.hpp"

#include "bsp_judge.h"

#define JS_RX_QUEUE_LEN 128
#define JS_FRAME_BUFFER_LEN 150
#define JS_SEND_BUFFER_LEN 512


class JudgeSystem
{
public:
    enum JudgePacketType
    {
        JPT_GameStatus = 0x0001,
        JPT_GameResult = 0x0002,
        JPT_GameRobotHp = 0x0003,
        JPT_DartStatus = 0x0004,
        JPT_IcraBuffDebuffZoneStatus = 0x0005,
        JPT_EventData = 0x0101,
        JPT_SupplyProjectileAction = 0x0102,
        JPT_RefereeWarning = 0x0104,
        JPT_DartRemainingTime = 0x0105,
        JPT_GameRobotStatus = 0x0201,
        JPT_PowerHeatData = 0x0202,
        JPT_GameRobotPos = 0x0203,
        JPT_Buff = 0x0204,
        JPT_AerialRobotEnergy = 0x0205,
        JPT_RobotHurt = 0x0206,
        JPT_ShootData = 0x0207,
        JPT_BulletRemaining = 0x0208,
        JPT_RfidStatus = 0x0209,
        JPT_DartClientCmd = 0x020A,
        JPT_Custom = 0x0301,
    };

    enum RobotIdType
    {
        RIT_RedHero = 1,
        RIT_RedEngineer = 2,
        RIT_RedInfantry_3 = 3,
        RIT_RedInfantry_4 = 4,
        RIT_RedInfantry_5 = 5,
        RIT_RedDrone = 6,
        RIT_RedSentry = 7,
        RIT_RedDart = 8,
        RIT_RedRadar = 9,
        RIT_BluHero = 101,
        RIT_BluEngineer = 102,
        RIT_BluInfantry_3 = 103,
        RIT_BluInfantry_4 = 104,
        RIT_BluInfantry_5 = 105,
        RIT_BluDrone = 106,
        RIT_BluSentry = 107,
        RIT_BluDart = 108,
        RIT_BluRadar = 109,

        RIT_ClientRedHero = 0x0101,
        RIT_ClientRedEngineer = 0x0102,
        RIT_ClientRedInfantry_3 = 0x0103,
        RIT_ClientRedInfantry_4 = 0x0104,
        RIT_ClientRedInfantry_5 = 0x0105,
        RIT_ClientRedDrone = 0x0106,
        RIT_ClientBluHero = 0x0165,
        RIT_ClientBluEngineer = 0x0166,
        RIT_ClientBluInfantry_3 = 0x0167,
        RIT_ClientBluInfantry_4 = 0x0168,
        RIT_ClientBluInfantry_5 = 0x0169,
        RIT_ClientBluDrone = 0x016A,
    };

    enum DrawOperationType
    {
        DOP_Delete = 0x0100,
        DOP_DrawOne = 0x0101,
        DOP_DrawTwo = 0x0102,
        DOP_DrawFive = 0x0103,
        DOP_DrawSeven = 0x0104,
        DOP_DrawChar = 0x0110,
    };

    enum GraphicOpType
    {
        GOT_Null = 0,
        GOT_Add = 1,
        GOT_Modify = 2,
        GOT_Delete = 3,
    };

    enum GraphicType
    {
        GT_Line = 0,
        GT_Rectangle = 1,
        GT_Circle = 2,
        GT_Elipse = 3,
        GT_Arc = 4,
        GT_Float = 5,
        GT_Integer = 6,
        GT_Char = 7,
    };

    enum GraphicColorType
    {
        GCT_Team = 0,
        GCT_Yellow = 1,
        GCT_Green = 2,
        GCT_Orange = 3,
        GCT_Purple = 4,
        GCT_Pink = 5,
        GCT_Cyan = 6,
        GCT_Black = 7,
        GCT_Write = 8
    };

    enum DelectType
    {
        DT_Null = 0,
        DT_DeleteOne = 1,
        DT_DeleteAll = 2
    };

    __packed struct JS_GraphicData
    {
        uint8_t graphic_name[3];
        uint32_t operate_tpye:3;
        uint32_t graphic_tpye:3;
        uint32_t layer:4;
        uint32_t color:4;
        uint32_t start_angle:9;
        uint32_t end_angle:9;
        uint32_t width:10;
        uint32_t start_x:11;
        uint32_t start_y:11;
        uint32_t radius:10;
        uint32_t end_x:11;
        uint32_t end_y:11;
    };

    __packed struct JS_ClientCustomCharacter
    {
        JS_GraphicData grapic_data_struct;
        uint8_t data[30];
    };
private:
    __packed struct JS_GameStatus
    {
        uint8_t game_type : 4;
        uint8_t game_progress : 4;
        uint16_t stage_remain_time;
        uint64_t sync_remain_stamp;
    };

    __packed struct JS_GameResult
    {
        uint8_t winner;
    };

    __packed struct JS_GameRobotHp
    {
        uint16_t red_1_robot_HP;
        uint16_t red_2_robot_HP;
        uint16_t red_3_robot_HP;
        uint16_t red_4_robot_HP;
        uint16_t red_5_robot_HP;
        uint16_t red_7_robot_HP;
        uint16_t red_outpost_HP;
        uint16_t red_base_HP;
        uint16_t blue_1_robot_HP;
        uint16_t blue_2_robot_HP;
        uint16_t blue_3_robot_HP;
        uint16_t blue_4_robot_HP;
        uint16_t blue_5_robot_HP;
        uint16_t blue_7_robot_HP;
        uint16_t blue_outpost_HP;
        uint16_t blue_base_HP;
    };

    __packed struct JS_DartStatus
    {
        uint8_t dart_belong;
        uint16_t stage_remaining_time;
    };

    __packed struct JS_IcraBuffDebuffZoneStatus
    {
        uint8_t F1_zone_status:1;
        uint8_t F1_zone_buff_debuff_status:3;
        uint8_t F2_zone_status:1;
        uint8_t F2_zone_buff_debuff_status:3;
        uint8_t F3_zone_status:1;
        uint8_t F3_zone_buff_debuff_status:3;
        uint8_t F4_zone_status:1;
        uint8_t F4_zone_buff_debuff_status:3;
        uint8_t F5_zone_status:1;
        uint8_t F5_zone_buff_debuff_status:3;
        uint8_t F6_zone_status:1;
        uint8_t F6_zone_buff_debuff_status:3;

        uint16_t red1_bullet_left;
        uint16_t red2_bullet_left;
        uint16_t blue1_bullet_left;
        uint16_t blue2_bullet_left;
    };

    __packed struct JS_EventData
    {
        uint32_t event_type;
    };

    __packed struct JS_SupplyProjectileAction
    {
        uint8_t supply_projectile_id;
        uint8_t supply_robot_id;
        uint8_t supply_projectile_step;
        uint8_t supply_projectile_num;
    };

    __packed struct JS_RefereeWarning
    {
        uint8_t level;
        uint8_t foul_robot_id;
    };

    __packed struct JS_DartRemainingTime
    {
        uint8_t dart_remaining_time;
    };

    __packed struct JS_GameRobotStatus
    {
        uint8_t robot_id;
        uint8_t robot_level;
        uint16_t remain_HP;
        uint16_t max_HP;

        uint16_t shooter_id1_17mm_cooling_rate;
        uint16_t shooter_id1_17mm_cooling_limit;
        uint16_t shooter_id1_17mm_speed_limit;

        uint16_t shooter_id2_17mm_cooling_rate;
        uint16_t shooter_id2_17mm_cooling_limit;
        uint16_t shooter_id2_17mm_speed_limit;

        uint16_t shooter_id1_42mm_cooling_rate;
        uint16_t shooter_id1_42mm_cooling_limit;
        uint16_t shooter_id1_42mm_speed_limit;

        uint16_t chassis_power_limit;
        uint8_t mains_power_gimbal_output : 1;
        uint8_t mains_power_chassis_output : 1;
        uint8_t mains_power_shooter_output : 1;
    };
    
    __packed struct JS_PowerHeatData
    {
        uint16_t chassis_volt;
        uint16_t chassis_current;
        float chassis_power;
        uint16_t chassis_power_buffer;
        uint16_t shoot_id1_17mm_cooling_heat;
        uint16_t shoot_id2_17mm_cooling_heat;
        uint16_t shoot_id1_42mm_cooling_heat;
    };
    
    __packed struct JS_GameRobotPos
    {
        float x;
        float y;
        float z;
        float yaw;
    };

    __packed struct JS_Buff
    {
        uint8_t power_rune_buff;
    };

    __packed struct JS_AerialRobotEnergy
    {
        uint8_t attack_time;
    };

    __packed struct JS_RobotHurt
    {
        uint8_t armor_id : 4;
        uint8_t hurt_type : 4;
    };

    __packed struct JS_ShootData
    {
        uint8_t bullet_type;
        uint8_t shooter_id;
        uint8_t bullet_freq;
        float bullet_speed;
    };

    __packed struct JS_BulletRemaining
    {
        uint16_t bullet_remaining_num_17mm;
        uint16_t bullet_remaining_num_42mm;
        uint16_t coin_remaining_num;
    };

    __packed struct JS_RfidStatus
    {
        uint32_t rfid_status;
    };

    __packed struct JS_DartClientCmd
    {
        uint8_t dart_launch_opening_status;
        uint8_t dart_attack_target;
        uint16_t target_change_time;
        uint16_t operate_launch_cmd_time;
    };

    __packed struct JS_CustomHeader
    {
        uint16_t data_cmd_id;
        uint16_t sender_ID;
        uint16_t receiver_ID;
    };

    __packed struct JS_DeleteGraphic
    {
        uint8_t operate_tpye;
        uint8_t layer;
    };

    uint8_t m_RxQuene[JS_RX_QUEUE_LEN];
    uint8_t m_FrameBuffer[JS_FRAME_BUFFER_LEN];
    uint8_t m_CurrentFrame[JS_FRAME_BUFFER_LEN];
    uint8_t m_HeaderBuffer[5];
    uint8_t m_SendBuffer[JS_SEND_BUFFER_LEN];
    JS_GraphicData m_GraphicBuffer[16];
    JS_ClientCustomCharacter m_CharBuffer[8];
    uint8_t m_RxQueueFront;
    uint8_t m_RxQueueRear;
    uint8_t m_FrameBufferLen;
    uint8_t m_HeaderBufferLen;
    uint16_t m_ExpectedLen;
    uint8_t m_SendSeq;
    uint8_t m_GraphicBufferLen;
    uint8_t m_CharBufferLen;
    uint16_t m_SendBufferLen;
    uint32_t m_headerperS;
    uint32_t m_hs;
    HashTable<uint32_t> m_PacketLastUpdateTick;
    HashTable<JudgeMsgHandler*> m_CustomMsgHandlerTable;
    bool m_CheckingHeader;
    bool m_HasGraphicOp;

    const static uint8_t Magic_Number;
    const static uint8_t Header_Length;
    const static uint32_t Flush_Tick;

    JudgeSystem() : m_RxQueueFront(0),
                    m_RxQueueRear(0),
                    m_FrameBufferLen(0),
                    m_HeaderBufferLen(0),
                    m_ExpectedLen(0),
                    m_SendSeq(0),
                    m_GraphicBufferLen(0),
                    m_CharBufferLen(0),
                    m_SendBufferLen(0),
                    m_headerperS(0),
                    m_hs(0),
                    m_CheckingHeader(false),
                    m_HasGraphicOp(false)
    {}

    void HandlePacket();
    void HandleCustomPacket();
    void FlushBuffer();
    RobotIdType GetClientId();
    void AddGraphicElement();
    bool AddOneGraphicElement();
    bool AddTwoGraphicElement();
    bool AddFiveGraphicElement();
    bool AddSevenGraphicElement();
    bool AddCharGraphicElement();
public:
    static JudgeSystem* Instance()
    {
        static JudgeSystem instance;
        return &instance;
    }

    void Enqueue(uint8_t _data)
    {
        m_RxQuene[m_RxQueueRear] = _data;
        m_RxQueueRear = (m_RxQueueRear + 1) % JS_RX_QUEUE_LEN;
    }

    uint32_t GetLastUpdateTick(JudgePacketType _packetType)
    {
        return *m_PacketLastUpdateTick.Search((uint32_t)_packetType);
    }

    void RegisterHandler(uint32_t _cmdId, JudgeMsgHandler* _handler){ m_CustomMsgHandlerTable.Insert(_cmdId, _handler); }

    void Init();
    void Update();
    bool Add2SendBuffer(RobotIdType _receiverId, uint16_t _frameId, uint8_t* _pData, uint16_t _dataLen);
    void DeleteLayer(DelectType _op, uint8_t _layer = 0xFF);
    bool ModfiyShapeOnClient(JS_GraphicData* _pData);
    bool ModfiyCharOnClient(JS_ClientCustomCharacter* _pData);

    JS_GameStatus GameStatus;
    JS_GameResult GameResult;
    JS_GameRobotHp GameRobotHp;
    JS_DartStatus DartStatus;
    JS_IcraBuffDebuffZoneStatus IcraBuffDebuffZoneStatus;
    JS_EventData EventData;
    JS_SupplyProjectileAction SupplyProjectileAction;
    JS_RefereeWarning RefereeWarning;
    JS_DartRemainingTime DartRemainingTime;
    JS_GameRobotStatus GameRobotStatus;
    JS_PowerHeatData PowerHeatData;
    JS_GameRobotPos GameRobotPos;
    JS_Buff Buff;
    JS_AerialRobotEnergy AerialRobotEnergy;
    JS_RobotHurt RobotHurt;
    JS_ShootData ShootData;
    JS_BulletRemaining BulletRemaining;
    JS_RfidStatus RfidStatus;
    JS_DartClientCmd DartClientCmd;
    JS_CustomHeader CustomHeader;
};

#endif
