#include "ChassisController.hpp"
#include "ChassisStateDummy.hpp"
#include "ChassisStateRc.hpp"
#include "arm_math.h"
#include "Led.hpp"
#include "RGB.hpp"

const float ChassisController::M3508StandardSpeedKp = 800.0f;
const float ChassisController::M3508StandardSpeedKi = 0.0f;
const float ChassisController::M3508StandardSpeedKd = 4.5f;
const float ChassisController::M3508StandardSpeedMaxout = 5500.0f;
const float ChassisController::M3508StandardSpeedIMaxout = 0.0f;
const float ChassisController::M3508MaxSpeed = 60.0f;

const float ChassisController::RadiusOfWheel = 0.0175f;// Meter


ChassisController::ChassisController() : ControllerEntity(ECT_ChassisController),
    m_MaxWheelSpd(M3508MaxSpeed),chassisFsm(this)
{}

void ChassisController::Init()
{
    SetDefaultTicksToUpdate(1);

    for(int i = 0; i < CWT_LENGTH; ++i)
    {
        m_ChassisWheel[i].RegistMotor(CAN1, 0x201 + i);
        m_ChassisWheel[i].controlMode = Motor::SPD_MODE;
        m_ChassisWheel[i].pidSpeed.kp = M3508StandardSpeedKp;
        m_ChassisWheel[i].pidSpeed.ki = M3508StandardSpeedKi;
        m_ChassisWheel[i].pidSpeed.kd = M3508StandardSpeedKd;
        m_ChassisWheel[i].pidSpeed.maxOut = M3508StandardSpeedMaxout;
        m_ChassisWheel[i].pidSpeed.maxIOut = M3508StandardSpeedIMaxout;
    }

    m_pDr16 = Dr16::Instance();
    m_pLimitSwitch = LimitSwitch::Instance();
    m_pInfrared = Infrared::Instance();
    m_pPushrod = Pushrod::Instance();

    LimitSwitch::Instance() -> Init();
    Infrared::Instance()->Init();
    Pushrod::Instance()->Init();

    m_pCtrlMsg = &BoardPacketManager::Instance()->GetChassisCtrlMsg();
    m_pGimbalRefMsg = &BoardPacketManager::Instance()->GetGimbalRefMsg();
    m_pGraspCtrlMsg = &BoardPacketManager::Instance()->GetGraspCtrlMsg();
    m_pPushrodCtrlMsg = &BoardPacketManager::Instance()->GetPushrodCtrlMsg();
    m_pChargeCtrlMsg = &BoardPacketManager::Instance()->GetChargeCtrlMsg();

    Vx_filters[0].SetTau(0.15f);
    Vx_filters[0].SetUpdatePeriod(1);

    Vx_filters[1].SetTau(0.05f);
    Vx_filters[1].SetUpdatePeriod(1);

    chassisFsm.Init();
}

void ChassisController::Update()
{
    chassisFsm.HandleInput();

    chassisFsm.Update();
    
    // Grasp::Instance()->Update();
    Infrared::Instance()->Update();
    Pushrod::Instance()->Update();
    
    ChassisSpd2MotorSpd();

    FilterUpdate();
    MaxDeltaVLimit();

    m_pCtrlMsg->SetVx(m_VxFdb);
    m_pCtrlMsg->SetInfrared(Infrared::Instance()->GetInfraredState());
    m_pCtrlMsg->SetLimitSwitch_front(LimitSwitch::Instance()->GetLimitSwitchFrontState());
    m_pCtrlMsg->SetLimitSwitch_rear(LimitSwitch::Instance()->GetLimitSwitchRearState());
    
    if (Time::GetTick() % 10 == 0)
    {
        m_pCtrlMsg->SendMsg();
    }

    if (abs(m_Vx) > 5)
    {
        rgb::On_green();
    }
    else if (abs(m_Vy) > 5)
    {
        rgb::On_red();
    }
    else
    {
        rgb::On_yellow();
    }

    MotorSpd2ChassisSpdFdb();

    if(!m_ChassisWheel[CWT_RightFront].sensorFeedBack.IsTimeout())
    {
        Led::On(Led::DebugA0);
    }
    else
    {
        Led::Off(Led::DebugA0);
    }

    if(!m_ChassisWheel[CWT_LeftFront].sensorFeedBack.IsTimeout())
    {
        Led::On(Led::DebugB1);
    }
    else
    {
        Led::Off(Led::DebugB1);
    }

    if(!m_ChassisWheel[CWT_LeftRear].sensorFeedBack.IsTimeout())
    {
        Led::On(Led::DebugC2);
    }
    else
    {
        Led::Off(Led::DebugC2);
    }

    if(!m_ChassisWheel[CWT_RightRear].sensorFeedBack.IsTimeout())
    {
        Led::On(Led::DebugD3);
    }
    else
    {
        Led::Off(Led::DebugD3);
    }
}

void ChassisController::MotorSpd2ChassisSpdFdb()
{
    float temp_speed[4] = {0};

    for (int i = 0; i < 4; i++)
    {
        temp_speed[i] = m_ChassisWheel[i].pFeedback->speedFdb * RadiusOfWheel; // meter / second
    }
    
    m_VxFdb = (- temp_speed[2] + temp_speed[3]) * 0.5f / RadiusOfWheel;
    
	m_PosX += m_VxFdb*0.001f;
	
	m_V_fdb_length = m_VxFdb;
}

void ChassisController::ChassisSpd2MotorSpd()
{
    bool frontLimitSwitchActive = m_pLimitSwitch->GetLimitSwitchFrontState();
    bool rearLimitSwitchActive = m_pLimitSwitch->GetLimitSwitchRearState();

    if (frontLimitSwitchActive && rearLimitSwitchActive)
    {
        m_Vy = m_Vy_Expection;
    }
    else if (!frontLimitSwitchActive && rearLimitSwitchActive)
    {
        if (m_Vy_Expection > 5)
        {
            m_Vy = m_Vy_Expection;
        }
        else
        {
            m_Vy = 0;
        }
    }
    else if (frontLimitSwitchActive && !rearLimitSwitchActive)
    {
        if (m_Vy_Expection < 5)
        {
            m_Vy = m_Vy_Expection;
        }
        else
        {
            m_Vy = 0;
        }
    }

    float temp_vector[4] = {+ m_Vy,
                            + m_Vx,
                            - m_Vx,
                            + m_Vx};
    
    float temp_max = 0.0f;
    for(int i = 0;i < 4;++i){
        if(temp_max < fabs(temp_vector[i])){
            temp_max = fabs(temp_vector[i]);
        }
    }
    m_V_set_length = m_Vx;

    for (int i = 0; i < 4; ++i)
    {
        m_ChassisWheel[i].speedSet = temp_vector[i];
    }
}
	

void ChassisFsm::HandleInput()
{	
    if(Dr16::Instance()->QuerySwState(Dr16::RC_SW_L, Dr16::RC_SW_M2D))
    {
        ChangeState(ChassisStateRc::Instance());
        return;
    }

    if(Dr16::Instance()->QuerySwState(Dr16::RC_SW_L, Dr16::RC_SW_M2U))
    {
        ChangeState(ChassisStateDummy::Instance());
        return;
    }
}

void ChassisFsm::Init()
{
    ChassisStateDummy::Instance()->Init(m_pOwner);
    ChassisStateRc::Instance()->Init(m_pOwner);
    SetCurrentState(ChassisStateDummy::Instance());
}

void ChassisController::FilterUpdate()
{
	Vx_filters[0].SetInput(m_Vx);
	Vx_filters[0].Update();
	Vx_filters[1].SetInput(Vx_filters[0].GetResult());
	Vx_filters[1].Update();
	Vx_filters[2].SetInput(Vx_filters[1].GetResult());
	Vx_filters[2].Update();
	m_Vx = Vx_filters[2].GetResult();
	
}

void ChassisController::MaxDeltaVLimit()
{
	float VxFdbAbs = m_VxFdb* 13.1148f;
	float delta = m_Vx - VxFdbAbs;
	
	if(VxFdbAbs <0.0f)
	{
		VxFdbAbs = -VxFdbAbs;
	}
	
	float max_delta = 1.2f + (m_VxFdb*m_VxFdb) * 13.1148f/50.0f;
	
	if(max_delta > 3.0f)
	{
		max_delta = 3.0f;
	}
	
	if(m_Vx * m_VxFdb < -0.01f)
	{
		max_delta*=0.005f;
	}
	
	if(delta>max_delta)
	{
		m_Vx = VxFdbAbs + max_delta;
	}
	else if(delta<-max_delta)
	{
		m_Vx = -VxFdbAbs -max_delta;
	}
	
	for(int i = 0; i < CWT_LENGTH; ++i)
    {
        m_ChassisWheel[i].pidSpeed.maxOut = M3508StandardSpeedMaxout + VxFdbAbs * 200.0f;
    }
}
