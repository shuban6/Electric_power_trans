#include "CanMsgDispatcher.hpp"

void CanMsgDispatcher::RegisterHandler(CAN_TypeDef* _canX, int32_t _canId, CanMsgHandler* _handler)
{
    int _canIndex = 0;
    if(_canX == CAN2)
    {
        _canIndex = 1;
    }

    m_CanIdHandlerTable[_canIndex].Insert(_canId, _handler);
}

void CanMsgDispatcher::Update()
{
    CanManager& _canManager = *CanManager::Instance();
    for(int i = 0; i < 2; ++i)
    {
        while(!_canManager.MsgQueue(i)->IsEmpty())
        {
            CanRxMsg& _rxMsg = *_canManager.MsgQueue(i)->Dequeue();
            CanMsgHandler** _handler = m_CanIdHandlerTable[i].Search(_rxMsg.StdId);

            if(_handler != nullptr)
            {
                (*_handler)->HandleNewCanRxMsg(&_rxMsg);
            }
        }
    }
}
