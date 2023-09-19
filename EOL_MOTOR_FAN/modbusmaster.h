#ifndef CVR_MIL_MODBUSPORT_H
#define CVR_MIL_MODBUSPORT_H

#include <QObject>
#include <QModbusClient>
#include <QModbusServer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

// 前置声明
class QSerialPort;

struct ModbusRevData {
    uint16_t modbusAddr;
    uint32_t serialNumber;
    QModbusDataUnit dataUnit;
};

// 前置声明
#define DOOR_POS_HIGH_MODBUS_ADDR          0X09
#define DOOR_POS_LOW_MODBUS_ADDR           0X08
#define FAN_SPEED_MODBUS_ADDR              0X07
#define FAN_NOISE_MODBUS_ADDR              0X06

class ModbusMaster : public QObject {
    Q_OBJECT

public:
    /// 构造
    explicit ModbusMaster(QObject *parent = Q_NULLPTR);

    /// 析构函数
    virtual ~ModbusMaster(void);
    void SetSingleReg(quint16 DevAddr, quint16 reg, quint16 value);
    void GetSingleReg(quint16 DevAddr, quint16 reg);
//    void SetMulReg(quint16 startReg);
    /// Modbus device object
    QModbusClient   *modbusDevice;

//    struct GA88PAREG {
//        qint16 BackCamPowerVal;
//        qint16 LeftCamPowerVal;
//        qint16 RightCamPowerVal;
//        qint16 CamVideoChannelVal;
//        qint16 SetBrightnessVal;
//    }mGa88paReg;
public slots:

    void onRecieved(void);
    void onWrited(void);
    void errorModbus(QModbusDevice::Error error);

signals:
    void revModbusDataUnit(ModbusRevData modbusRevdata);

public:
    void stateChanged(QModbusDevice::State state);
    void writeHoldingRegisters(quint16 address, quint16 data, quint8 id);
    void readHoldingRegisters(quint16 address, quint8 id);
    void sendWriteRequest(QModbusDataUnit dataUnit, quint8 id);
    void sendReadRequest(QModbusDataUnit dataUnit, quint8 id);

    /// Connection flag
    bool is_connected;
};


#endif // CVR_MIL_MODBUSPORT_H
