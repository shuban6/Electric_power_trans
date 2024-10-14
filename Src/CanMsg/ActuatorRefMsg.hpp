#ifndef ACTUATOR_REF_MSG_HPP
#define ACTUATOR_REF_MSG_HPP

#include "CanMsgHandler.hpp"

class ActuatorRefMsg : public CanMsgHandler
{
private:
    uint8_t success;

    CAN_TypeDef *m_pCan;
    uint32_t m_canId;

public:
    void SetSuccess(uint8_t _success) { success = _success; }

    uint8_t GetSucess() const { return success; }

    void Init(CAN_TypeDef *can, uint32_t _stdId);
    void SendMsg();

    virtual void HandleNewCanRxMsg(CanRxMsg *_msg);
};

#endif
