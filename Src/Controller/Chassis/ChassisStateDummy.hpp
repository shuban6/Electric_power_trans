#ifndef CHASSISSTATEDUMMY_HPP
#define CHASSISSTATEDUMMY_HPP

#include "StateMachine.hpp"
#include "ChassisController.hpp"
#include "Dr16.hpp"
#include "BoardPacket.hpp"
#include "IspImp.hpp"


class ChassisStateDummy : public State<ChassisController>
{
private:
    BoardPacketManager *m_pBoardManager;
public:
    virtual void Init(ChassisController* pOwner);
    virtual void Enter(ChassisController* pOwner);
    virtual void Execute(ChassisController* pOwner);
    virtual void Exit(ChassisController* pOwner);

    static ChassisStateDummy* Instance()
    {
        static ChassisStateDummy instance;
        return &instance;
    }
};

#endif
