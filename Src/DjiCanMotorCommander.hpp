#ifndef DJICAMMOTORCOMMANDER_HPP
#define DJICAMMOTORCOMMANDER_HPP

#include "CanManager.hpp"

class DjiCanMotorCommander
{
private:
    int16_t msgBuffer[2][12];
    bool dirtyFlag[2][3];

    DjiCanMotorCommander()
    {
        for(int i = 0; i < 2; ++i)
        {
            for(int j = 0; j < 3; ++j)
            {
                dirtyFlag[i][j] = false;
            }
        }
    }
public:
    
    static DjiCanMotorCommander* Instance()
    {
        static DjiCanMotorCommander instance;
        return &instance;
    }

    virtual void Update();
    void Add2Buffer(CAN_TypeDef* _can, int32_t _canId, int16_t _current);
};

#endif
