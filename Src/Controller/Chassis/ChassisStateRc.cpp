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

    pOwner->SetVy( - Dr16::Instance()->GetRVAxis() * 80.0f);
	
    if (Dr16::Instance()->QuerySwState(Dr16::RC_SW_R, Dr16::RC_SW_M2D))
    {
        pOwner->GetPushrod()->SetPushRodState(0);
    }
    else if (Dr16::Instance()->QuerySwState(Dr16::RC_SW_R, Dr16::RC_SW_M2U))
    {
        pOwner->GetPushrod()->SetPushRodState(1);
    }
}

void ChassisStateRc::Exit(ChassisController* pOwner)
{
    ;
}
