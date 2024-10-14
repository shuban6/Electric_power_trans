#ifndef PUSHROD_CTRL_MSG_HPP
#define PUSHROD_CTRL_MSG_HPP

#include "CanMsgHandler.hpp"
#include "Pushrod.hpp"


class PushrodCtrlMsg : public CanMsgHandler
{
private:
    uint8_t m_pushrod;

    CAN_TypeDef *m_pCan;
    uint32_t m_canId;

public:
    void SetPushrod(uint8_t _vx) { m_pushrod = _vx; }

    uint8_t GetPushrod() const { return m_pushrod; }

    void Init(CAN_TypeDef *can, uint32_t _stdId);
    void SendMsg();

    virtual void HandleNewCanRxMsg(CanRxMsg *_msg);
};

#endif
