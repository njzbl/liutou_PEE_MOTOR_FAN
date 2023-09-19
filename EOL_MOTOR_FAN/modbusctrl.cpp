#include "modbusctrl.h"

ModbusCtrl::ModbusCtrl()
{


    mModbusMaster = new ModbusMaster();
}


int ModbusCtrl::setFanAddr(int addr)
{
//    mModbusMaster->SetSingleReg(9,0x02,addr);

}

int ModbusCtrl::getFanval(void)
{
    mModbusMaster->GetSingleReg(FAN_SPEED_MODBUS_ADDR,0x00);
    mModbusMaster->GetSingleReg(FAN_NOISE_MODBUS_ADDR,0x00);
//    mModbusMaster->GetSingleReg(0X09,0x00);
//    mModbusMaster->GetSingleReg(0X09,0x02);
//    mModbusMaster->GetSingleReg(0X08,0x00);
//    mModbusMaster->GetSingleReg(0X08,0x02);
//    mModbusMaster->GetSingleReg(0X08,0x00);
//    mModbusMaster->GetSingleReg(0X08,0x02);
}


//void ModbusCtrl::setSettingsWidget(SettingsWidget* settingsWidget)
//{
//    mSettingsWidget = settingsWidget;
//}
