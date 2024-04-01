#ifndef HUDMANAGER_HPP
#define HUDMANAGER_HPP

#include "RobotEngine.hpp"
#include "JudgeSystem.hpp"
#include "Observer.hpp"
#include "BoardPacket.hpp"

enum HudMangerOperationType
{
    HMOT_NullOp  = 0xFF,
    HMOT_Setup   = 0x00,
    HMOT_Update  = 0x01,
    HMOT_Refresh = 0x02,
};

enum GimbalHudElementType
{
    GHET_FireMode,
    GHET_AimbotStatus,
    GHET_AmmoLid,
    GHET_Spin,
    GHET_FireRate,
    GHET_FricOn,
    GHET_PitchAngle,

    GHET_LENGTH
};

class HudManager : public ControllerEntity, public Observer
{
private:
    JudgeSystem* m_pJudge;

    JudgeSystem::JS_ClientCustomCharacter m_GheDrawBuffer[GHET_LENGTH];
    JudgeSystem::JS_GraphicData m_CrosshairBuffer[7];
    JudgeSystem::JS_GraphicData m_SCapBuffer[2];

    ChassisCtrlMsg* m_pCtrlMsg;
    ChassisCtrlMsg2* m_pCtrlMsg2;

    HudMangerOperationType m_opIndex;

    uint8_t m_lastProjectileVelocitySet;
    GimbalFireModeType m_lastGimbalFireMode;
    AimbotStatusType m_lastAimbotStatus;
    bool m_lastAmmoLidStatus;
    bool m_lastSpinningStatus;
    bool m_lastLowFireRate;
    bool m_lastFricOn;

    float m_pitchAngle;
    bool m_useSCap;
    float m_sCapVoltage;
    float m_sCapPercentage;
    bool m_sCapDirty;

    bool m_enableDynamicCrosshair;
    uint32_t m_initTick;
    bool m_hasInited;

    const static float kGravity;
    const static float kInvGravity;
    const static float kGimbalHeight;
    const static float kArmorLength;
    const static float kPixelRatio;
    const static float kRefLen;

public:
    HudManager() : ControllerEntity(ECT_HudManager)
    {}
    
    // voltage should be between 12.0f to 24.0f
    void SetSCapVoltage(float voltage);

    void SetupCrosshair();
    void UpdateCrosshair();
    void DeleteCrosshair();

    void SetupFireMode();
    void UpdateFireMode();
    void DeleteFireMode();

    void SetupAimbotStatus();
    void UpdateAimbotStatus();
    void DeleteAimbot();

    void SetupAmmoLid();
    void UpdateAmmoLid();
    void DeleteAmmoLid();

    void SetupSpin();
    void UpdateSpin();
    void DeleteSpin();

    void SetupFireRate();
    void UpdateFireRate();
    void DeleteFireRate();

    void SetupFricOn();
    void UpdateFricOn();
    void DeleteFricOn();

    void SetupPitchAngle();
    void UpdatePitchAngle();
    void DeletePitchAngle();

    void SetupSCap();
    void UpdateSCap();
    void DeleteSCap();

    virtual void Init();
    virtual void Update();

    virtual void OnNotify(void* _param);
};

#endif
