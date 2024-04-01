#ifndef JUDGEMSGHANDLER_H
#define JUDGEMSGHANDLER_H

#include "stm32f4xx.h"
#include <string.h>

class JudgeMsgHandler
{
protected:
    uint8_t lastMsg[128];
    uint16_t msgLen;
public:
    virtual void HandleNewMsg(uint8_t* _pData, uint16_t _len)
    {
        memcpy(lastMsg, _pData, _len);
        msgLen = _len;
    }
};

#endif
