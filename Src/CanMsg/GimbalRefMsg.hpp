#ifndef GIMBAL_REF_MSG_HPP
#define GIMBAL_REF_MSG_HPP

#include "CanMsgHandler.hpp"

#define MAX_CHASSIS_CTRL_SPEED 100.0f

class GimbalRefMsg : public CanMsgHandler
{
private:
    float m_Vx;

    CAN_TypeDef *m_pCan;
    uint32_t m_canId;

    float UncompressUint16(uint16_t _in);
    uint16_t CompressFloat(float _in);

public:
    float GetVx() const { return m_Vx; }
    void SetVx(float _vx) { m_Vx = _vx; }

    void Init(CAN_TypeDef *can, uint32_t _stdId);
    void SendMsg();

    virtual void HandleNewCanRxMsg(CanRxMsg *_msg);
};

#endif
