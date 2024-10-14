#include "ChassisStateDummy.hpp"

void ChassisStateDummy::Init(ChassisController* pOwner)
{
    m_pBoardManager = BoardPacketManager::Instance();
}

void ChassisStateDummy::Enter(ChassisController* pOwner)
{
    ;
}

void ChassisStateDummy::Execute(ChassisController* pOwner)
{
    GimbalRefMsg& gimbalRefMsg = m_pBoardManager->GetGimbalRefMsg();
    GraspCtrlMsg& graspCtrlMsg = m_pBoardManager->GetGraspCtrlMsg();
    PushrodCtrlMsg& pushrodCtrlMsg = m_pBoardManager->GetPushrodCtrlMsg();
    ChargeCtrlMsg& chargeCtrlMsg = m_pBoardManager->GetChargeCtrlMsg();

    if(!gimbalRefMsg.IsMsgTimeout())
    {
        pOwner->SetVx(gimbalRefMsg.GetVx());
    }
    else
    {
        pOwner->Stop();
    }

    if (!graspCtrlMsg.IsMsgTimeout())
    {
        pOwner->SetVy(graspCtrlMsg.GetGraspSpeed());
    }

    if (!pushrodCtrlMsg.IsMsgTimeout())
    {
        pOwner->GetPushrod()->SetPushRodState(pushrodCtrlMsg.GetPushrod());
    }

    if (!chargeCtrlMsg.IsMsgTimeout())
    {
        pOwner->GetPushrod()->SetChargeState(chargeCtrlMsg.GetPushrod());
    }
    
}

void ChassisStateDummy::Exit(ChassisController* pOwner)
{
    ;
}
