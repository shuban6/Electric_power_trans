#include "ChassisStateRc.hpp"

void ChassisStateRc::Init(ChassisController* pOwner)
{
    ;
}

void ChassisStateRc::Enter(ChassisController* pOwner)
{
    ;
}

void ChassisStateRc::Execute(ChassisController* pOwner)
{
    pOwner->SetVx(Dr16::Instance()->GetLVAxis() * 50.0f);
	//pOwner->SetVy(Dr16::Instance()->GetLHAxis() * 10.0f);

    if (Dr16::Instance()->QuerySwState(Dr16::RC_SW_R, Dr16::RC_SW_M2D))
    {
        pOwner->GetGrasp()->SetGrasp_State(2);
    }
    else if (Dr16::Instance()->QuerySwState(Dr16::RC_SW_R, Dr16::RC_SW_M2U))
    {
        pOwner->GetGrasp()->SetGrasp_State(1);
    }
    else if (Dr16::Instance()->QuerySwState(Dr16::RC_SW_R, Dr16::RC_SW_D2M) || Dr16::Instance()->QuerySwState(Dr16::RC_SW_R, Dr16::RC_SW_U2M))
    {
        pOwner->GetGrasp()->SetGrasp_State(0);
    }
}

void ChassisStateRc::Exit(ChassisController* pOwner)
{
    ;
}
