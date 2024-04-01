#include "HudManager.hpp"

#include "ftoa.h"

#include "BoardPacket.hpp"

#include "Math.hpp"
#include "math.h"

const float HudManager::kGravity = 9.7925f;
const float HudManager::kInvGravity = 0.1021189686f;
const float HudManager::kGimbalHeight = 0.45f;
const float HudManager::kArmorLength = 0.13f;
const float HudManager::kPixelRatio = 576.0f;
const float HudManager::kRefLen = 0.15f;

void HudManager::SetSCapVoltage(float voltage)
{
    m_sCapVoltage = voltage;
    float normalizedVoltage = (voltage * voltage - 144.0f) * 0.0023148148148f;
    m_sCapPercentage = Math::FloatConstrain(normalizedVoltage, 0.0f, 1.0f);
    m_sCapDirty = true;
}

void HudManager::Init()
{
    SetDefaultTicksToUpdate(350);

    m_lastProjectileVelocitySet = 0;

    // Toy RPC setup, this RPC is used to change HudManager's behavior.
    m_opIndex = HMOT_NullOp;
    BoardPacketManager::Instance()->GetTestPacket().AddObserver(this);

    m_pCtrlMsg = &BoardPacketManager::Instance()->GetChassisCtrlMsg();
    m_pCtrlMsg2 = &BoardPacketManager::Instance()->GetChassisCtrlMsg2();
    m_pJudge = JudgeSystem::Instance();

    m_sCapDirty = true;

    m_enableDynamicCrosshair = true;

    m_initTick = 10000;
    m_hasInited = false;
}

void HudManager::Update()
{   
    if(!m_hasInited)
    {
        if(Time::GetTick() < 1000)
        {
            m_opIndex = HMOT_Setup;
        }
        else
        {
            m_opIndex = HMOT_Update;
        }

        if(Time::GetTick() > m_initTick)
        {
            m_hasInited = true;
        }
    }

    if(m_opIndex == HMOT_Setup)
    {
        // Setup all HUD elements, One drawcall per setup
        SetupCrosshair();
        SetupFireMode();
        SetupFireRate();
        SetupFricOn();
        SetupSpin();
        SetupAimbotStatus();
        SetupAmmoLid();
        SetupPitchAngle();
        SetupSCap();

        // Start to update dirtied HUD elements.
        m_opIndex = HMOT_Update;
    }
    else if(m_opIndex == HMOT_Update)
    {
        // Only if I can use keyword auto...
        // Update dirtied HUD elements on the client.
        const uint8_t latestVelocity = m_pCtrlMsg->GetProjectileVelocity();
        if(latestVelocity != m_lastProjectileVelocitySet || m_enableDynamicCrosshair)
        {
            m_lastProjectileVelocitySet = latestVelocity;
            UpdateCrosshair();
        }

        const GimbalFireModeType firemode = m_pCtrlMsg->GetGimbalFireMode();
        if(firemode != m_lastGimbalFireMode)
        {
            m_lastGimbalFireMode = firemode;
            UpdateFireMode();
        }

        const AimbotStatusType aimbotStatus = m_pCtrlMsg->GetAimbotStatus();
        if(aimbotStatus != m_lastAimbotStatus)
        {
            m_lastAimbotStatus = aimbotStatus;
            UpdateAimbotStatus();
        }

        const bool isAmmoLidOpen = m_pCtrlMsg->IsAmmoLidOpen();
        if(isAmmoLidOpen != m_lastAmmoLidStatus)
        {
            m_lastAmmoLidStatus = isAmmoLidOpen;
            UpdateAmmoLid();
        }

        const bool isSpinning = m_pCtrlMsg->IsSpinning();
        if(isSpinning != m_lastSpinningStatus)
        {
            m_lastSpinningStatus = isSpinning;
            UpdateSpin();
        }

        const bool isLowFireRate = m_pCtrlMsg->IsLowFireRate();
        if(isLowFireRate != m_lastLowFireRate)
        {
            m_lastLowFireRate = isLowFireRate;
            UpdateFireRate();
        }

        const bool isFricOn = m_pCtrlMsg->IsFricOn();
        if(isFricOn != m_lastFricOn)
        {
            m_lastFricOn = isFricOn;
            UpdateFricOn();
        }

        m_useSCap = m_pCtrlMsg2->UseSCap();
        if(m_sCapDirty || m_useSCap)
        {
            UpdateSCap();
        }

        m_pitchAngle = m_pCtrlMsg2->GetPitchAngle();
        UpdatePitchAngle();
    }
    else if(m_opIndex == HMOT_Refresh)
    {
        // Force refresh all HUD
        //UpdateCrosshair();
        UpdateFireMode();
        UpdateAimbotStatus();
        UpdateAmmoLid();
        UpdateSpin();
        UpdateFireRate();
        UpdateFricOn();
        UpdatePitchAngle();
        UpdateSCap();

        m_opIndex = HMOT_Update;
    }
}

void HudManager::OnNotify(void* param)
{
    HudMangerOperationType newOp = (HudMangerOperationType)(*(uint8_t*)param);

    m_opIndex = newOp;
}

void HudManager::SetupCrosshair()
{
    // A sloppy hack to dirty the projectile velocity manually.
    m_lastProjectileVelocitySet = 0;

    for (int i = 0; i < 7; ++i)
    {
        m_CrosshairBuffer[i].operate_tpye = JudgeSystem::GOT_Add;
        m_CrosshairBuffer[i].graphic_name[2] = i + 1;
        m_CrosshairBuffer[i].color = JudgeSystem::GCT_Green;
        m_CrosshairBuffer[i].width = 2;
        m_CrosshairBuffer[i].graphic_tpye = JudgeSystem::GT_Line;

        m_pJudge->ModfiyShapeOnClient(&m_CrosshairBuffer[i]);

        // Reduce the need to do this for every update step.
        m_CrosshairBuffer[i].operate_tpye = JudgeSystem::GOT_Modify;
    }
}
void HudManager::UpdateCrosshair()
{
    float _v = m_lastProjectileVelocitySet;
    float _v2 = _v * _v;
    float _gimbalPitchAngle = m_enableDynamicCrosshair ? m_pitchAngle * 0.0174533f : 0.0f;
    float _cos = cos(_gimbalPitchAngle);
    float _sin = sin(_gimbalPitchAngle);

    float _time = 0.0f;
    float _atan = 0.0f;
    float _halfArmorLanding = 0.0;
    float _l = 0.0f;
    int16_t _dropPixel;
    float _invV = 1 / _v;

    m_CrosshairBuffer[0].color = JudgeSystem::GCT_Orange;

    _time = (_v * _sin + sqrt(_v2 * _sin * _sin + 2.0f * kGravity * kGimbalHeight)) * kInvGravity; 

    _atan = atan2(_sin - 0.5f * kGravity * _time * _invV, _cos);
    _dropPixel = (int16_t)(kPixelRatio * tan(_gimbalPitchAngle - _atan));
    if(_dropPixel > 539)
    {
        _dropPixel = 539;
    }

    m_CrosshairBuffer[0].start_x = 960;
    m_CrosshairBuffer[0].start_y = 540;
    m_CrosshairBuffer[0].end_x = 960;
    m_CrosshairBuffer[0].end_y = 540 - _dropPixel;

    m_pJudge->ModfiyShapeOnClient(&m_CrosshairBuffer[0]);

    m_CrosshairBuffer[1].color = JudgeSystem::GCT_Orange;
    m_CrosshairBuffer[1].start_y = 540 - _dropPixel;
    m_CrosshairBuffer[1].end_y = 540 - _dropPixel;

    _l = _v * _time * _cos / cos(_atan);

    _halfArmorLanding = kPixelRatio * kRefLen / _l;

    m_CrosshairBuffer[1].start_x = 960 - _halfArmorLanding;
    m_CrosshairBuffer[1].end_x = 960 + _halfArmorLanding;

    m_pJudge->ModfiyShapeOnClient(&m_CrosshairBuffer[1]);

    m_CrosshairBuffer[2].color = JudgeSystem::GCT_Green;
    _time = 0.1f;
    _atan = atan2(_sin - 0.5f * kGravity * _time * _invV, _cos);
    _dropPixel = (int16_t)(kPixelRatio * tan(_gimbalPitchAngle - _atan));
    if(_dropPixel > 539)
    {
        _dropPixel = 539;
    }

    m_CrosshairBuffer[2].start_y = 540 - _dropPixel;
    m_CrosshairBuffer[2].end_y = 540 - _dropPixel;

    _l = _v * _time * _cos / cos(_atan);

    _halfArmorLanding = kPixelRatio * kRefLen / _l;

    m_CrosshairBuffer[2].start_x = 960 - _halfArmorLanding;
    m_CrosshairBuffer[2].end_x = 960 + _halfArmorLanding;

    m_pJudge->ModfiyShapeOnClient(&m_CrosshairBuffer[2]);

    _time = 0.2f;
    _atan = atan2(_sin - 0.5f * kGravity * _time * _invV, _cos);
    _dropPixel = (int16_t)(kPixelRatio * tan(_gimbalPitchAngle - _atan));
    if(_dropPixel > 539)
    {
        _dropPixel = 539;
    }

    m_CrosshairBuffer[3].start_y = 540 - _dropPixel;
    m_CrosshairBuffer[3].end_y = 540 - _dropPixel;

    _l = _v * _time * _cos / cos(_atan);

    _halfArmorLanding = kPixelRatio * kRefLen / _l;

    m_CrosshairBuffer[3].start_x = 960 - _halfArmorLanding;
    m_CrosshairBuffer[3].end_x = 960 + _halfArmorLanding;

    m_pJudge->ModfiyShapeOnClient(&m_CrosshairBuffer[3]);

    _time = 0.4f;
    _atan = atan2(_sin - 0.5f * kGravity * _time * _invV, _cos);
    _dropPixel = (int16_t)(kPixelRatio * tan(_gimbalPitchAngle - _atan));
    if(_dropPixel > 539)
    {
        _dropPixel = 539;
    }

    m_CrosshairBuffer[4].start_y = 540 - _dropPixel;
    m_CrosshairBuffer[4].end_y = 540 - _dropPixel;

    _l = _v * _time * _cos / cos(_atan);

    _halfArmorLanding = kPixelRatio * kRefLen / _l;

    m_CrosshairBuffer[4].start_x = 960 - _halfArmorLanding;
    m_CrosshairBuffer[4].end_x = 960 + _halfArmorLanding;

    m_pJudge->ModfiyShapeOnClient(&m_CrosshairBuffer[4]);

    _time = 0.6f;
    _atan = atan2(_sin - 0.5f * kGravity * _time * _invV, _cos);
    _dropPixel = (int16_t)(kPixelRatio * tan(_gimbalPitchAngle - _atan));
    if(_dropPixel > 539)
    {
        _dropPixel = 539;
    }

    m_CrosshairBuffer[5].start_y = 540 - _dropPixel;
    m_CrosshairBuffer[5].end_y = 540 - _dropPixel;

    _l = _v * _time * _cos / cos(_atan);

    _halfArmorLanding = kPixelRatio * kRefLen / _l;

    m_CrosshairBuffer[5].start_x = 960 - _halfArmorLanding;
    m_CrosshairBuffer[5].end_x = 960 + _halfArmorLanding;

    m_pJudge->ModfiyShapeOnClient(&m_CrosshairBuffer[5]);

    _time = 0.8f;
    _atan = atan2(_sin - 0.5f * kGravity * _time * _invV, _cos);
    _dropPixel = (int16_t)(kPixelRatio * tan(_gimbalPitchAngle - _atan));
    if(_dropPixel > 539)
    {
        _dropPixel = 539;
    }

    m_CrosshairBuffer[6].start_y = 540 - _dropPixel;
    m_CrosshairBuffer[6].end_y = 540 - _dropPixel;

    _l = _v * _time * _cos / cos(_atan);

    _halfArmorLanding = kPixelRatio * kRefLen / _l;

    m_CrosshairBuffer[6].start_x = 960 - _halfArmorLanding;
    m_CrosshairBuffer[6].end_x = 960 + _halfArmorLanding;

    m_pJudge->ModfiyShapeOnClient(&m_CrosshairBuffer[6]);
}
void HudManager::DeleteCrosshair()
{
    for (int i = 0; i < 7; ++i)
    {
        m_CrosshairBuffer[i].operate_tpye = JudgeSystem::GOT_Delete;
        m_pJudge->ModfiyShapeOnClient(&m_CrosshairBuffer[i]);
        m_CrosshairBuffer[i].operate_tpye = JudgeSystem::GOT_Modify;
    }
}

void HudManager::SetupFireMode()
{
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Add;
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.graphic_name[1] = 1;
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.graphic_tpye = JudgeSystem::GT_Char;
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.width = 2;
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.color = JudgeSystem::GCT_Pink;
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.start_x = 1000;
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.start_y = 360;
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.start_angle = 20;
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.end_angle = 15;
    uint8_t strBuffer[30] = "Mode     : BURST";
    memcpy(m_GheDrawBuffer[GHET_FireMode].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_FireMode]);

    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}
void HudManager::UpdateFireMode()
{
    uint8_t strBuffer[30] = {0};
    switch (m_lastGimbalFireMode)
    {
    case GFT_Auto:
        uint8_t wordBuffer1[30] = "Mode     : AUTO";
        m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.color = JudgeSystem::GCT_Pink;
        memcpy(strBuffer, wordBuffer1, 30);
        break;
    
    case GFT_Burst:
        uint8_t wordBuffer2[30] = "Mode     : BURST";
        m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.color = JudgeSystem::GCT_Pink;
        memcpy(strBuffer, wordBuffer2, 30);
        break;

    case GFT_SafeAuto:
        uint8_t wordBuffer3[30] = "Mode     : AUTO";
        m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.color = JudgeSystem::GCT_Green;
        memcpy(strBuffer, wordBuffer3, 30);
        break;

    case GFT_SafeBurst:
        uint8_t wordBuffer4[30] = "Mode     : BURST";
        m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.color = JudgeSystem::GCT_Green;
        memcpy(strBuffer, wordBuffer4, 30);
        break;

    default:
        break;
    }

    memcpy(m_GheDrawBuffer[GHET_FireMode].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_FireMode]);
}
void HudManager::DeleteFireMode()
{
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Delete;
    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_FireMode]);
    m_GheDrawBuffer[GHET_FireMode].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}

void HudManager::SetupAimbotStatus()
{
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Add;
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.graphic_name[1] = 2;
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.graphic_tpye = JudgeSystem::GT_Char;
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.width = 2;
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.color = JudgeSystem::GCT_Cyan;
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.start_x = 1000;
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.start_y = 300;
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.start_angle = 20;
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.end_angle = 15;
    uint8_t strBuffer[30] = "Aim      : Offline";
    memcpy(m_GheDrawBuffer[GHET_AimbotStatus].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_AimbotStatus]);

    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}
void HudManager::UpdateAimbotStatus()
{
    uint8_t strBuffer[30] = {0};
    switch (m_lastAimbotStatus)
    {
    case AST_Offline:
        uint8_t wordBuffer1[30] = "Aim      : Offline";
        memcpy(strBuffer, wordBuffer1, 30);
        break;
    
    case AST_Manual:
        uint8_t wordBuffer2[30] = "Aim      : Manual";
        memcpy(strBuffer, wordBuffer2, 30);
        break;

    case AST_Aimbot:
        uint8_t wordBuffer3[30] = "Aim      : Aimbot";
        memcpy(strBuffer, wordBuffer3, 30);
        break;

    default:
        break;
    }

    memcpy(m_GheDrawBuffer[GHET_AimbotStatus].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_AimbotStatus]);
}
void HudManager::DeleteAimbot()
{
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Delete;
    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_AimbotStatus]);
    m_GheDrawBuffer[GHET_AimbotStatus].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}

void HudManager::SetupAmmoLid()
{
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Add;
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.graphic_name[1] = 3;
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.graphic_tpye = JudgeSystem::GT_Char;
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.width = 2;
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.color = JudgeSystem::GCT_Cyan;
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.start_x = 1000;
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.start_y = 240;
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.start_angle = 20;
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.end_angle = 15;
    uint8_t strBuffer[30] = "Lid      : CLOSED";
    memcpy(m_GheDrawBuffer[GHET_AmmoLid].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_AmmoLid]);

    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}
void HudManager::UpdateAmmoLid()
{
    uint8_t strBuffer[30] = {0};

    if(!m_lastAmmoLidStatus)
    {
        uint8_t wordBuffer[30] = "Lid      : CLOSED";
        memcpy(strBuffer, wordBuffer, 30);
    }
    else
    {
        uint8_t wordBuffer[30] = "Lid      : OPEN";
        memcpy(strBuffer, wordBuffer, 30);
    }
    
    memcpy(m_GheDrawBuffer[GHET_AmmoLid].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_AmmoLid]);
}
void HudManager::DeleteAmmoLid()
{
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Delete;
    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_AmmoLid]);
    m_GheDrawBuffer[GHET_AmmoLid].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}

void HudManager::SetupSpin()
{
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Add;
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.graphic_name[1] = 4;
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.graphic_tpye = JudgeSystem::GT_Char;
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.width = 2;
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.color = JudgeSystem::GCT_Cyan;
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.start_x = 1000;
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.start_y = 210;
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.start_angle = 20;
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.end_angle = 15;
    uint8_t strBuffer[30] = "Spin     : OFF";
    memcpy(m_GheDrawBuffer[GHET_Spin].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_Spin]);

    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}
void HudManager::UpdateSpin()
{
    uint8_t strBuffer[30] = {0};

    if(!m_lastSpinningStatus)
    {
        uint8_t wordBuffer[30] = "Spin     : OFF";
        memcpy(strBuffer, wordBuffer, 30);
    }
    else
    {
        uint8_t wordBuffer[30] = "Spin     : On";
        memcpy(strBuffer, wordBuffer, 30);
    }
    
    memcpy(m_GheDrawBuffer[GHET_Spin].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_Spin]);
}
void HudManager::DeleteSpin()
{
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Delete;
    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_Spin]);
    m_GheDrawBuffer[GHET_Spin].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}

void HudManager::SetupFireRate()
{
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Add;
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.graphic_name[1] = 5;
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.graphic_tpye = JudgeSystem::GT_Char;
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.width = 2;
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.color = JudgeSystem::GCT_Cyan;
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.start_x = 1000;
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.start_y = 330;
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.start_angle = 20;
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.end_angle = 15;
    uint8_t strBuffer[30] = "Firerate : LOW";
    memcpy(m_GheDrawBuffer[GHET_FireRate].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_FireRate]);

    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}
void HudManager::UpdateFireRate()
{
    uint8_t strBuffer[30] = {0};

    if(m_lastLowFireRate)
    {
        uint8_t wordBuffer[30] = "Firerate : LOW";
        memcpy(strBuffer, wordBuffer, 30);
    }
    else
    {
        uint8_t wordBuffer[30] = "Firerate : HIGH";
        memcpy(strBuffer, wordBuffer, 30);
    }
    
    memcpy(m_GheDrawBuffer[GHET_FireRate].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_FireRate]);
}
void HudManager::DeleteFireRate()
{
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Delete;
    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_FireRate]);
    m_GheDrawBuffer[GHET_FireRate].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}

void HudManager::SetupFricOn()
{
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Add;
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.graphic_name[1] = 6;
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.graphic_tpye = JudgeSystem::GT_Char;
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.width = 2;
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.color = JudgeSystem::GCT_Cyan;
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.start_x = 1000;
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.start_y = 270;
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.start_angle = 20;
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.end_angle = 15;
    uint8_t strBuffer[30] = "Fric     : OFF";
    memcpy(m_GheDrawBuffer[GHET_FricOn].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_FricOn]);

    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}
void HudManager::UpdateFricOn()
{
    uint8_t strBuffer[30] = {0};

    if(!m_lastFricOn)
    {
        uint8_t wordBuffer[30] = "Fric     : OFF";
        memcpy(strBuffer, wordBuffer, 30);
    }
    else
    {
        uint8_t wordBuffer[30] = "Fric     : On";
        memcpy(strBuffer, wordBuffer, 30);
    }
    
    memcpy(m_GheDrawBuffer[GHET_FricOn].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_FricOn]);
}
void HudManager::DeleteFricOn()
{
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Delete;
    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_FricOn]);
    m_GheDrawBuffer[GHET_FricOn].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}

void HudManager::SetupPitchAngle()
{
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Add;
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.graphic_name[1] = 7;
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.graphic_tpye = JudgeSystem::GT_Char;
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.width = 2;
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.color = JudgeSystem::GCT_Cyan;
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.start_x = 1000;
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.start_y = 180;
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.start_angle = 20;
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.end_angle = 15;
    uint8_t strBuffer[30] = "Pitch    : ";
    char floatBuffer[10];
    ftoa(m_pitchAngle, floatBuffer, 3);
    memcpy(strBuffer + 11, floatBuffer, 10);
    memcpy(m_GheDrawBuffer[GHET_PitchAngle].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_PitchAngle]);

    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}

void HudManager::UpdatePitchAngle()
{
    uint8_t strBuffer[30] = "Pitch    : ";
    char floatBuffer[10];
    ftoa(m_pitchAngle, floatBuffer, 3);
    memcpy(strBuffer + 11, floatBuffer, 10);

    memcpy(m_GheDrawBuffer[GHET_PitchAngle].data, strBuffer, 30);

    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_PitchAngle]);
}

void HudManager::DeletePitchAngle()
{
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Delete;
    m_pJudge->ModfiyCharOnClient(&m_GheDrawBuffer[GHET_PitchAngle]);
    m_GheDrawBuffer[GHET_PitchAngle].grapic_data_struct.operate_tpye = JudgeSystem::GOT_Modify;
}


void HudManager::SetupSCap()
{
    m_SCapBuffer[0].operate_tpye = JudgeSystem::GOT_Add;
    m_SCapBuffer[0].graphic_name[1] = 8;
    m_SCapBuffer[0].graphic_tpye = JudgeSystem::GT_Line;
    m_SCapBuffer[0].width = 20;
    m_SCapBuffer[0].color = JudgeSystem::GCT_Cyan;
    m_SCapBuffer[0].start_x = 1000;
    m_SCapBuffer[0].start_y = 120;
    m_SCapBuffer[0].end_x = 1350;
    m_SCapBuffer[0].end_y = 120;

    m_pJudge->ModfiyShapeOnClient(&m_SCapBuffer[0]);
    m_SCapBuffer[0].operate_tpye = JudgeSystem::GOT_Modify;

    m_SCapBuffer[1].operate_tpye = JudgeSystem::GOT_Add;
    m_SCapBuffer[1].graphic_name[1] = 9;
    m_SCapBuffer[1].graphic_tpye = JudgeSystem::GT_Line;
    m_SCapBuffer[1].width = 18;
    m_SCapBuffer[1].color = JudgeSystem::GCT_Green;
    m_SCapBuffer[1].start_x = 1001;
    m_SCapBuffer[1].start_y = 120;
    m_SCapBuffer[1].end_x = 1349;
    m_SCapBuffer[1].end_y = 120;

    m_pJudge->ModfiyShapeOnClient(&m_SCapBuffer[1]);
    m_SCapBuffer[1].operate_tpye = JudgeSystem::GOT_Modify;
}

void HudManager::UpdateSCap()
{
    if(!m_sCapDirty)
    {
        m_pJudge->ModfiyShapeOnClient(&m_SCapBuffer[0]);
    }

    m_SCapBuffer[1].color = JudgeSystem::GCT_Green;
    if(m_pCtrlMsg2->UseSCap())
    {
        m_SCapBuffer[1].color = JudgeSystem::GCT_Orange;
    }

    m_SCapBuffer[1].end_x = 961 + (uint32_t)(m_sCapPercentage * (float)(1349 - 961));

    m_sCapDirty = false;
    m_pJudge->ModfiyShapeOnClient(&m_SCapBuffer[1]);
}

void HudManager::DeleteSCap()
{
    m_SCapBuffer[0].operate_tpye = JudgeSystem::GOT_Delete;
    m_pJudge->ModfiyShapeOnClient(&m_SCapBuffer[0]);
    m_SCapBuffer[0].operate_tpye = JudgeSystem::GOT_Modify;

    m_SCapBuffer[1].operate_tpye = JudgeSystem::GOT_Delete;
    m_pJudge->ModfiyShapeOnClient(&m_SCapBuffer[1]);
    m_SCapBuffer[1].operate_tpye = JudgeSystem::GOT_Modify;
}

