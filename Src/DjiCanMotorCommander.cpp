#include "DjiCanMotorCommander.hpp"

void DjiCanMotorCommander::Update()
{
    for(int i = 0; i < 2; ++i)
    {
        CAN_TypeDef* _can = (i == 0) ? CAN1 : CAN2;
        for(int j = 0; j < 3; ++j)
        {
            if(dirtyFlag[i][j])
            {
                uint32_t _id = 0;
                switch (j)
                {
                case 0:
                    _id = 0x200;
                    break;
                
                case 1:
                    _id = 0x1FF;
                    break;

                case 2:
                    _id = 0x2FF;
                    break;

                default:
                    break;
                }

                CanTxMsg _txMessage;
                _txMessage.StdId = _id;
                _txMessage.IDE = CAN_ID_STD;
                _txMessage.RTR = CAN_RTR_DATA;
                _txMessage.DLC = 0x08;
                _txMessage.Data[0] = msgBuffer[i][j * 4] >> 8;
                _txMessage.Data[1] = msgBuffer[i][j * 4];
                _txMessage.Data[2] = msgBuffer[i][j * 4 + 1] >> 8;
                _txMessage.Data[3] = msgBuffer[i][j * 4 + 1];
                _txMessage.Data[4] = msgBuffer[i][j * 4 + 2] >> 8;
                _txMessage.Data[5] = msgBuffer[i][j * 4 + 2];
                _txMessage.Data[6] = msgBuffer[i][j * 4 + 3] >> 8;
                _txMessage.Data[7] = msgBuffer[i][j * 4 + 3];

                CAN_Transmit(_can, &_txMessage);

                dirtyFlag[i][j] = false;
            }
        }
    }
}

void DjiCanMotorCommander::Add2Buffer(CAN_TypeDef* _can, int32_t _canId, int16_t _current)
{
    int _i = (_can == CAN1) ? 0 : 1;
    int _j = _canId - 0x201;
    msgBuffer[_i][_j] = _current;
    dirtyFlag[_i][_j / 4] = true;
	if(_i == 0 && _j == 4)
	{
		msgBuffer[_i][_j] = _current;
	}
}
