#ifndef TESTBOT_HPP
#define TESTBOT_HPP

#include "RobotEngine.hpp"
#include "ChassisController.hpp"


class Testbot : public RobotEngine
{
private:
    ChassisController chassisController;

public:
    Testbot() : RobotEngine(this)
    {
    }
};

#endif
