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

    if(!gimbalRefMsg.IsMsgTimeout())
    {
        pOwner->SetVx(gimbalRefMsg.GetVx());
    }
    else
    {
        pOwner->Stop();
    }
}

void ChassisStateDummy::Exit(ChassisController* pOwner)
{
    ;
}
