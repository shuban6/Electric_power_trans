#ifndef CHASSIS_CTRL_MSG_HPP
#define CHASSIS_CTRL_MSG_HPP

#include "CanMsgHandler.hpp"
#include "LimitSwitch.hpp"

#define MAX_CHASSIS_CTRL_SPEED 100.0f

class ChassisCtrlMsg : public CanMsgHandler
{
private:
    float m_Vx;
    uint16_t m_pVx;
    uint16_t m_infrared;
    uint8_t m_limitswitch_front;
    uint8_t m_limitswitch_rear;

    CAN_TypeDef* m_pCan;
    uint32_t m_canId;

    float UncompressUint16(uint16_t _in);
    uint16_t CompressFloat(float _in);
public:
    float GetVx() const { return m_Vx; }
    void SetVx(float _vx) { m_Vx = _vx; }
    void SetInfrared (uint16_t infrared) { m_infrared = infrared; }
    void SetLimitSwitch_front(uint8_t limitswitch_front) { m_limitswitch_front = limitswitch_front; }
    void SetLimitSwitch_rear(uint8_t limitswitch_rear) { m_limitswitch_rear = limitswitch_rear; }

    void Init(CAN_TypeDef* can, uint32_t _stdId);
    void SendMsg();

    virtual void HandleNewCanRxMsg(CanRxMsg* _msg);
};

#endif
