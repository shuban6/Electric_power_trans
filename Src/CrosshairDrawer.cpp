#include "CrosshairDrawer.hpp"
#include "Dr16.hpp"
#include "math.h"

#include "BoardPacket.hpp"

void CrosshairDrawner::Init()
{
    SetDefaultTicksToUpdate(550);

    m_pGimbalController = (GimbalController*)GetOwner()->GetEntity(ECT_GimbalController);

    m_rpcIndex = 255;
    BoardPacketManager::Instance()->GetTestPacket().AddObserver(this);

    m_lastMaxProjectileVelocity = JudgeSystem::Instance()->GameRobotStatus.shooter_id1_17mm_speed_limit;
}

void CrosshairDrawner::Update()
{   
    if(m_rpcIndex == 0)
    {
        m_DrawBuffer.operate_tpye = JudgeSystem::GOT_Add;
        m_DrawBuffer.graphic_name[2] = 1;
        m_DrawBuffer.color = JudgeSystem::GCT_Cyan;
        m_DrawBuffer.width = 2;
        m_DrawBuffer.graphic_tpye = JudgeSystem::GT_Line;
        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);
        m_DrawBuffer.color = JudgeSystem::GCT_Green;
        m_DrawBuffer.graphic_name[2] = 2;
        m_DrawBuffer.radius = 50;
        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);
        m_DrawBuffer.graphic_name[2] = 3;
        m_DrawBuffer.radius = 50;
        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);
        m_DrawBuffer.graphic_name[2] = 4;
        m_DrawBuffer.radius = 50;
        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);
        m_DrawBuffer.graphic_name[2] = 5;
        m_DrawBuffer.radius = 50;
        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);
        m_DrawBuffer.graphic_name[2] = 6;
        m_DrawBuffer.radius = 50;
        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);
        m_DrawBuffer.graphic_name[2] = 7;
        m_DrawBuffer.radius = 50;
        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);
    }

    uint16_t latestMaxVel = JudgeSystem::Instance()->GameRobotStatus.shooter_id1_17mm_speed_limit;
    if(latestMaxVel == m_lastMaxProjectileVelocity)
    {
        return;
    }

    m_lastMaxProjectileVelocity = latestMaxVel;

    if(m_rpcIndex == 1)
    {
        m_DrawBuffer.operate_tpye = JudgeSystem::GOT_Modify;
        m_DrawBuffer.graphic_tpye = JudgeSystem::GT_Line;
        float _v = m_lastMaxProjectileVelocity;
        float _v2 = _v * _v;
        float _gimbalPitchAngle = - m_pGimbalController->GetGimbalMotor(GimbalController::GMT_Pitch)->sensorFeedBack.positionFdb;
        float _cos = cos(_gimbalPitchAngle);
        float _sin = sin(_gimbalPitchAngle);

        float _time = 0.0f;
        float _atan = 0.0f;
        float _halfArmorLanding = 0.0;
        float _l = 0.0f;
        int16_t _dropPixel;
        float _invV = 1 / _v;

        m_DrawBuffer.graphic_name[2] = 1;
        m_DrawBuffer.color = JudgeSystem::GCT_Orange;
        m_DrawBuffer.width = 2;

        _time = (_v * _sin + sqrt(_v2 * _sin * _sin + 2.0f * GRAVITY * GIMBAL_HEIGHT)) * INV_GRAVITY; 

        _atan = atan2(_sin - 0.5f * GRAVITY * _time * _invV, _cos);
        _dropPixel = (int16_t)(PIXEL_RATIO * tan(_gimbalPitchAngle - _atan));
        if(_dropPixel > 539)
        {
            _dropPixel = 539;
        }

        m_DrawBuffer.start_x = 960;
        m_DrawBuffer.start_y = 540;
        m_DrawBuffer.end_x = 960;
        m_DrawBuffer.end_y = 540 - _dropPixel;

        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);

        m_DrawBuffer.graphic_name[2] = 2;
        

        m_DrawBuffer.start_y = 540 - _dropPixel;
        m_DrawBuffer.end_y = 540 - _dropPixel;

        _l = _v * _time * _cos / cos(_atan);

        _halfArmorLanding = PIXEL_RATIO * REF_LEN / _l;

        m_DrawBuffer.start_x = 960 - _halfArmorLanding;
        m_DrawBuffer.end_x = 960 + _halfArmorLanding;

        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);

        m_DrawBuffer.graphic_name[2] = 3;
        m_DrawBuffer.color = JudgeSystem::GCT_Green;
        _time = 0.1f;
        _atan = atan2(_sin - 0.5f * GRAVITY * _time * _invV, _cos);
        _dropPixel = (int16_t)(PIXEL_RATIO * tan(_gimbalPitchAngle - _atan));
        if(_dropPixel > 539)
        {
            _dropPixel = 539;
        }

        m_DrawBuffer.start_y = 540 - _dropPixel;
        m_DrawBuffer.end_y = 540 - _dropPixel;

        _l = _v * _time * _cos / cos(_atan);

        _halfArmorLanding = PIXEL_RATIO * REF_LEN / _l;

        m_DrawBuffer.start_x = 960 - _halfArmorLanding;
        m_DrawBuffer.end_x = 960 + _halfArmorLanding;

        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);

        m_DrawBuffer.graphic_name[2] = 4;

        _time = 0.2f;
        _atan = atan2(_sin - 0.5f * GRAVITY * _time * _invV, _cos);
        _dropPixel = (int16_t)(PIXEL_RATIO * tan(_gimbalPitchAngle - _atan));
        if(_dropPixel > 539)
        {
            _dropPixel = 539;
        }
        m_DrawBuffer.start_y = 540 - _dropPixel;
        m_DrawBuffer.end_y = 540 - _dropPixel;

        _l = _v * _time * _cos / cos(_atan);

        _halfArmorLanding = PIXEL_RATIO * REF_LEN / _l;

        m_DrawBuffer.start_x = 960 - _halfArmorLanding;
        m_DrawBuffer.end_x = 960 + _halfArmorLanding;

        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);

        m_DrawBuffer.graphic_name[2] = 5;

        _time = 0.4f;
        _atan = atan2(_sin - 0.5f * GRAVITY * _time * _invV, _cos);
        _dropPixel = (int16_t)(PIXEL_RATIO * tan(_gimbalPitchAngle - _atan));
        if(_dropPixel > 539)
        {
            _dropPixel = 539;
        }
        m_DrawBuffer.start_y = 540 - _dropPixel;
        m_DrawBuffer.end_y = 540 - _dropPixel;
        _l = _v * _time * _cos / cos(_atan);

        _halfArmorLanding = PIXEL_RATIO * REF_LEN / _l;

        m_DrawBuffer.start_x = 960 - _halfArmorLanding;
        m_DrawBuffer.end_x = 960 + _halfArmorLanding;

        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);

        m_DrawBuffer.graphic_name[2] = 6;

        _time = 0.6f;
        _atan = atan2(_sin - 0.5f * GRAVITY * _time * _invV, _cos);
        _dropPixel = (int16_t)(PIXEL_RATIO * tan(_gimbalPitchAngle - _atan));
        if(_dropPixel > 539)
        {
            _dropPixel = 539;
        }
        m_DrawBuffer.start_y = 540 - _dropPixel;
        m_DrawBuffer.end_y = 540 - _dropPixel;
        _l = _v * _time * _cos / cos(_atan);

        _halfArmorLanding = PIXEL_RATIO * REF_LEN / _l;

        m_DrawBuffer.start_x = 960 - _halfArmorLanding;
        m_DrawBuffer.end_x = 960 + _halfArmorLanding;

        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);

        m_DrawBuffer.graphic_name[2] = 7;

        _time = 0.8f;
        _atan = atan2(_sin - 0.5f * GRAVITY * _time * _invV, _cos);
        _dropPixel = (int16_t)(PIXEL_RATIO * tan(_gimbalPitchAngle - _atan));
        if(_dropPixel > 539)
        {
            _dropPixel = 539;
        }
        m_DrawBuffer.start_y = 540 - _dropPixel;
        m_DrawBuffer.end_y = 540 - _dropPixel;
        _l = _v * _time * _cos / cos(_atan);

        _halfArmorLanding = PIXEL_RATIO * REF_LEN / _l;

        m_DrawBuffer.start_x = 960 - _halfArmorLanding;
        m_DrawBuffer.end_x = 960 + _halfArmorLanding;

        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);

    }
    else if(m_rpcIndex == 2)
    {
        /* code */
        m_DrawBuffer.operate_tpye = JudgeSystem::GOT_Delete;

        m_DrawBuffer.graphic_tpye = JudgeSystem::GT_Line;

        m_DrawBuffer.layer = 0;

        m_DrawBuffer.color = JudgeSystem::GCT_Black;

        m_DrawBuffer.width = 2;

        m_DrawBuffer.start_x = 960;

        m_DrawBuffer.start_y = 540;

        m_DrawBuffer.radius = m_Radius;

        JudgeSystem::Instance()->ModfiyShapeOnClient(&m_DrawBuffer);
    }
}


void CrosshairDrawner::OnNotify(void* _param)
{
    m_rpcIndex = *(uint8_t*)_param;
}
