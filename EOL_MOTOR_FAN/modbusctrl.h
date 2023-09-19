#ifndef MODBUSCTRL_H
#define MODBUSCTRL_H

#include <QObject>
#include <QModbusServer>
#include "modbusmaster.h"

class ModbusCtrl : public QObject {
    Q_OBJECT

public:
    ModbusCtrl();

public slots:
    int setFanAddr(int addr);
    int getFanval(void);
//    void setSettingsWidget(SettingsWidget* settingsWidget);

public:
    ModbusMaster *mModbusMaster;


};

#endif // MODBUSCTRL_H
