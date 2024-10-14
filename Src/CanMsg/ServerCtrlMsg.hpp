#ifndef SERVER_CTRL_MSG_HPP
#define SERVER_CTRL_MSG_HPP

#include "CanMsgHandler.hpp"

class ServerCtrlMsg : public CanMsgHandler
{
private:
    uint8_t m_id;
    uint8_t m_command;

    CAN_TypeDef *m_pCan;
    uint32_t m_canId;

public:
    void SetID(uint8_t _id) { m_id = _id; }
    void SetCommand(uint8_t _command) { m_command = _command; }

    uint8_t GetID() const { return m_id; }
    uint8_t GetCommand() const { return m_command; } 

    void Init(CAN_TypeDef *can, uint32_t _stdId);
    void SendMsg();

    virtual void HandleNewCanRxMsg(CanRxMsg *_msg);
};

#endif
