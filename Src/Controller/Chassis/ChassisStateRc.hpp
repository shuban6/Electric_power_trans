#ifndef CHASSISSTATERC_HPP
#define CHASSISSTATERC_HPP

#include "StateMachine.hpp"
#include "ChassisController.hpp"

class ChassisStateRc : public State<ChassisController>
{
public:
    virtual void Init(ChassisController* pOwner);
    virtual void Enter(ChassisController* pOwner);
    virtual void Execute(ChassisController* pOwner);
    virtual void Exit(ChassisController* pOwner);

    static ChassisStateRc* Instance()
    {
        static ChassisStateRc instance;
        return &instance;
    }
};

#endif
