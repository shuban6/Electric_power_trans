#ifndef G6020SENSORHANDLER_HPP
#define G6020SENSORHANDLER_HPP

#include "RobotEngine.hpp"
#include "CanMsgDispatcher.hpp"
#include "CanMsgHandler.hpp"
#include "MotorFeedback.hpp"
#include "Math.hpp"

class G6020SensorHandler : public SensorEntity, public CanMsgHandler, public MotorFeedback
{
private:
    uint16_t ecd;
    int16_t speed_rpm;
    int16_t given_current;
    uint8_t temperate;
    CAN_TypeDef* can;
    uint32_t canId;
    bool newDataReady;
    const static float RawPos2Rad;  /* 2Pi / 8191 */
    const static float RawRpm2Rps;  /* 2Pi / 60 */
public:
    uint16_t offset;

    G6020SensorHandler();

    void RegistSensor(CAN_TypeDef* _can, int _canId)
    {
        can = _can;
        canId = _canId;
        CanMsgDispatcher::Instance()->RegisterHandler(can, canId, this);
    }

    virtual void HandleNewCanRxMsg(CanRxMsg* _msg)
    {
        CanMsgHandler::HandleNewCanRxMsg(_msg);
        newDataReady = true;
    }

    virtual bool HasNewData()
    {
        bool _dataReadyShadow = newDataReady;
        newDataReady = false;
        return _dataReadyShadow;
    }

    virtual void Init();
    virtual void Update();
};

#endif
