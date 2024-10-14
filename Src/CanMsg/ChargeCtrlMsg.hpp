#ifndef CHARGE_CTRL_MSG_HPP
#define CHARGE_CTRL_MSG_HPP

#include "CanMsgHandler.hpp"

class ChargeCtrlMsg : public CanMsgHandler
{
private:
    uint8_t m_charge;

    CAN_TypeDef *m_pCan;
    uint32_t m_canId;

public:
    void SetCharge(uint8_t _vx) { m_charge = _vx; }
    uint8_t GetPushrod() const { return m_charge; }

    void Init(CAN_TypeDef *can, uint32_t _stdId);
    void SendMsg();

    virtual void HandleNewCanRxMsg(CanRxMsg *_msg);
};

#endif
