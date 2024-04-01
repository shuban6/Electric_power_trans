#include "ChassisCtrlMsg.hpp"
#include "CanMsgDispatcher.hpp"
#include "Stream.hpp"

// const uint8_t ChassisCtrlMsg::kFricMask     = (0x01 << 0);
// const uint8_t ChassisCtrlMsg::kSpinningMask = (0x01 << 1);
// const uint8_t ChassisCtrlMsg::kAmmoLidMask  = (0x01 << 2);
// const uint8_t ChassisCtrlMsg::kFireRateMask = (0x01 << 3);

// const uint8_t ChassisCtrlMsg::kAstOffset = 4;
// const uint8_t ChassisCtrlMsg::kGftOffset = 6;

// // 0000 0011
// const uint8_t ChassisCtrlMsg::kEnumMask = 0x03;

void ChassisCtrlMsg::HandleNewCanRxMsg(CanRxMsg* _msg)
{
    CanMsgHandler::HandleNewCanRxMsg(_msg);

    uint8_t* ptr = lastCanMsg.Data;

    uint16_t rawVx = ArrayStreamHelper::ReadUint16(ptr);
    uint16_t rawVy = ArrayStreamHelper::ReadUint16(ptr);
    uint16_t rawVw = ArrayStreamHelper::ReadUint16(ptr);

    m_Vx = UncompressUint16(rawVx);
    // m_Vy = UncompressUint16(rawVy);
    // m_Vw = UncompressUint16(rawVw);

    // m_projectileVelocitySet = ArrayStreamHelper::ReadUint8(ptr);
    uint8_t bitfieldBuffer = ArrayStreamHelper::ReadUint8(ptr);

    // m_isAmmoLidOpen = (kAmmoLidMask == (kAmmoLidMask & bitfieldBuffer));
    // m_isSpinning = (kSpinningMask == (kSpinningMask & bitfieldBuffer));
    // m_isLowFireRate = (kFireRateMask == (kFireRateMask & bitfieldBuffer));
    // m_isFricOn = (kFricMask == (kFricMask & bitfieldBuffer));

    // m_aimbotStatus = (AimbotStatusType)(kEnumMask & (bitfieldBuffer >> kAstOffset));
    // m_gimbalFireMode = (GimbalFireModeType)(kEnumMask & (bitfieldBuffer >> kGftOffset));
}

float ChassisCtrlMsg::UncompressUint16(uint16_t _in)
{
    return (float)(((float)_in / (float)UINT16_MAX * (2.0f * MAX_CHASSIS_CTRL_SPEED)) - MAX_CHASSIS_CTRL_SPEED);
}

uint16_t ChassisCtrlMsg::CompressFloat(float _in)
{
    return (uint16_t)((_in + MAX_CHASSIS_CTRL_SPEED) / (2.0f * MAX_CHASSIS_CTRL_SPEED) * (float)UINT16_MAX);
}

void ChassisCtrlMsg::Init(CAN_TypeDef* can, uint32_t _stdId)
{
    m_Vx = 0.0f;
    m_pVx = 0;
    // m_Vy = 0.0f;
    // m_Vw = 0.0f;

    // m_projectileVelocitySet = 0;
    // m_gimbalFireMode = GFT_Auto;
    // m_aimbotStatus = AST_Offline;

    // m_isAmmoLidOpen = false;
    // m_isSpinning = false;
    // m_isLowFireRate = true;
    // m_isFricOn = false;

    m_canId = _stdId;
    m_pCan = can;
    CanMsgDispatcher::Instance()->RegisterHandler(m_pCan, m_canId, this);
}

void ChassisCtrlMsg::SendMsg()
{
    uint8_t buffer[8] = {0};

    uint8_t* ptr = buffer;

    m_pVx = CompressFloat(m_Vx);

    // 0 bytes
    ArrayStreamHelper::Write(ptr, CompressFloat(m_Vx));
    // 2 bytes
    // ArrayStreamHelper::Write(ptr, CompressFloat(m_Vy));
    // // 4 bytes
    // ArrayStreamHelper::Write(ptr, CompressFloat(m_Vw));

    // 6 bytes
    // ArrayStreamHelper::Write(ptr, m_projectileVelocitySet);
    // 7 bytes

    uint8_t bitfieldBuffer = 0x00;
    
    buffer[7] = bitfieldBuffer;

    CanManager::Instance()->CanTransmit(m_pCan, m_canId, buffer, sizeof(buffer));
}
