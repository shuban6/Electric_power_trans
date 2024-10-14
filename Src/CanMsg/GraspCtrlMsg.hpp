#ifndef CRASP_CTRL_MSG_HPP
#define CRASP_CTRL_MSG_HPP

#include "CanMsgHandler.hpp"

#define MAX_CHASSIS_CTRL_SPEED 100.0f

class GraspCtrlMsg : public CanMsgHandler
{
private:
    float m_speed;

    CAN_TypeDef *m_pCan;
    uint32_t m_canId;

    float UncompressUint16(uint16_t _in);
    uint16_t CompressFloat(float _in);

public:
    
    void SetGraspSpeed(uint8_t _vx) { m_speed = _vx; }
    
    float GetGraspSpeed() const { return m_speed; }

    void Init(CAN_TypeDef *can, uint32_t _stdId);
    void SendMsg();

    virtual void HandleNewCanRxMsg(CanRxMsg *_msg);
};

#endif
