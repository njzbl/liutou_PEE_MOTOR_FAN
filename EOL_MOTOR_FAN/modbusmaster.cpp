#include <QDebug>
#include <QModbusServer>
#include <QModbusRtuSerialMaster>
#include <modbusmaster.h>


ModbusMaster::ModbusMaster(QObject* parent)
    : QObject(parent)
{
////    QByteArray portName;
////    foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts())
////        portName.append(info.portName());
//    // 初始化modbusDevice
//    modbusDevice = new QModbusRtuSerialMaster(this);
//    connect(modbusDevice, &QModbusClient::stateChanged,
//            this, &ModbusMaster::stateChanged);
//    if (modbusDevice->state() != QModbusDevice::ConnectedState){
//        modbusDevice->setConnectionParameter(
//                    QModbusDevice::SerialPortNameParameter, "COM11");
//        modbusDevice->setConnectionParameter(
//                    QModbusDevice::SerialBaudRateParameter, 9600);
//        modbusDevice->setConnectionParameter(
//                    QModbusDevice::SerialDataBitsParameter, 8);
//        modbusDevice->setConnectionParameter(
//                    QModbusDevice::SerialStopBitsParameter, 1);
//        modbusDevice->setConnectionParameter(
//                    QModbusDevice::SerialParityParameter, QSerialPort::NoParity);
//        modbusDevice->setTimeout(1000);
//        modbusDevice->setNumberOfRetries(3);
//        qDebug("modbusDevice is seted...OK");
//    }
//    // Try connection
//    modbusDevice->connectDevice();
}

ModbusMaster::~ModbusMaster(void)
{
//    modbusDevice->disconnectDevice();
}

void ModbusMaster::stateChanged(QModbusDevice::State state)
{
    switch (state) {
    case QModbusDevice::ConnectedState:
        is_connected = true;
        qDebug("Device is connected...OK");
        break;
    case QModbusDevice::UnconnectedState:
        is_connected = false;
        qDebug("Device is disconnected...ERR");
        break;
    default:
        is_connected = false;
        break;
    }
}
//void ModbusMaster::SetMulReg(quint16 startReg)
//{
//    if(!is_connected)
//        return;
//    QModbusDataUnit dataUnit(
//                QModbusDataUnit::HoldingRegisters, startReg, mRegCount);
//    qDebug("sizeof(mGa88paReg)/2 = %d",sizeof(mGa88paReg)/2);
//    qint16 *data = (qint16 *)&mGa88paReg;
//    for(int i = 0;i < mRegCount; i++)
//        dataUnit.setValue(i, data[i + startReg]);
//    sendWriteRequest(dataUnit, CVR_MODBUS_GA88PAID);
//}

void ModbusMaster::SetSingleReg(quint16 DevAddr, quint16 reg, quint16 value)
{
    if(!is_connected)
        return;
    writeHoldingRegisters(reg, value, DevAddr);
}

void ModbusMaster::GetSingleReg(quint16 DevAddr, quint16 reg)
{
    if(!is_connected)
        return;
    readHoldingRegisters(reg, DevAddr);
}

void ModbusMaster::writeHoldingRegisters(quint16 address, quint16 data, quint8 id)
{
    QModbusDataUnit dataUnit(
                QModbusDataUnit::HoldingRegisters, address, 1);
    dataUnit.setValue(0, data);
    sendWriteRequest(dataUnit, id);
}

void ModbusMaster::readHoldingRegisters(quint16 address, quint8 id)
{
    QModbusDataUnit dataUnit(
            QModbusDataUnit::HoldingRegisters, address, 1);//1:每次读1个寄存器，16bit的寄存器   QModbusDataUnit::HoldingRegisters, address, 1);
    sendReadRequest(dataUnit, id);
}

void ModbusMaster::sendReadRequest(QModbusDataUnit dataUnit, quint8 id)
{
    QModbusReply *reply = modbusDevice->sendReadRequest(dataUnit, id);

    if (reply != nullptr){
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished,
                    this, &ModbusMaster::onRecieved);
            connect(reply, &QModbusReply::errorOccurred,
                    this, &ModbusMaster::errorModbus);
        }
        else
            reply->deleteLater();
    }
    else{
        qDebug("ERROR: Master send data error");
    }
}
void ModbusMaster::onRecieved(void)
{
    QModbusReply *reply = qobject_cast<QModbusReply *>(sender());
    if (reply == nullptr) {
        qDebug("ERROR: empty reply");
        return ;
    }
    // Check by exception
    if (reply->rawResult().isException())
    {
        qDebug("ERROR: Reply exception = %s ", modbusDevice->errorString());
    }

    // Check by invalid PDU in reply
    if (reply->rawResult().isValid())
    {
        QModbusDataUnit dataUnit = reply->result();
        if (reply->error() == QModbusDevice::NoError){
            quint16 count = static_cast<quint16>(dataUnit.valueCount());
            quint16 data[16];
            for (int i = 0; i < count && i < 16; i++){
               data[i] = dataUnit.value(i);
//               qDebug("data[%d]=0x%x",i,data[i]);
            }
            ModbusRevData modbusRevdata;
            modbusRevdata.modbusAddr = reply->serverAddress();
            modbusRevdata.dataUnit = dataUnit;
            emit revModbusDataUnit(modbusRevdata);
        }
        else{
            qDebug("ERROR: Invalid PDU = %s", reply->errorString());
        }
    }
    else
    {
        qDebug("ERROR: Invalid PDU = %s", modbusDevice->errorString());
    }
    reply->deleteLater();
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusMaster::onWrited(void)
{
    QModbusReply *reply = qobject_cast<QModbusReply *>(sender());

    // Check by empty reply
    if (reply == nullptr)
    {
        qDebug("In onWrited ERROR: empty reply");

    }

    // Check by exception
    if (reply->rawResult().isException())
    {
        qDebug("In onWrited ERROR: Reply exception = %s", modbusDevice->errorString());
    }

    // Check by invalid PDU in reply
    if (reply->rawResult().isValid())
    {

        //
        QModbusDataUnit dataUnit = reply->result();
        if (reply->error() == QModbusDevice::NoError){
            quint16 count = static_cast<quint16>(dataUnit.valueCount());
            quint16 data[16];
            for (int i = 0; i < count && i < 16; i++)
            {   data[i] = dataUnit.value(i);
            }
        }
        else{
            qDebug("ERROR: Invalid PDU = %s", reply->errorString());
        }

    }
    else
    {
        qDebug("In onWrited ERROR: Invalid PDU = %s", modbusDevice->errorString());
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusMaster::sendWriteRequest(QModbusDataUnit dataUnit, quint8 id)
{
    // Send reply
    QModbusReply *reply = modbusDevice->sendWriteRequest(dataUnit, id);
    if (reply != nullptr){
        if (!reply->isFinished()){
             connect(reply, &QModbusReply::finished,
                     this, &ModbusMaster::onWrited);
             connect(reply, &QModbusReply::errorOccurred,
                     this, &ModbusMaster::errorModbus);
         }
         else
             reply->deleteLater();
    }
    else{
        qDebug("ERROR: Master send data error");
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ModbusMaster::errorModbus(QModbusDevice::Error error)
{
    switch (error)
    {
    case QModbusDevice::NoError:
        qDebug("This is QModbusDevice::NoError = %s",modbusDevice->errorString());
        break;

    case QModbusDevice::ReadError:
        qDebug("This is QModbusDevice::ReadError = %s",modbusDevice->errorString());
        break;

    case QModbusDevice::WriteError:
        qDebug("This is QModbusDevice::WriteError = %s",modbusDevice->errorString());
        break;

    case QModbusDevice::ConnectionError:
        qDebug("This is QModbusDevice::ConnectionError = %s",modbusDevice->errorString());
        break;

    case QModbusDevice::ConfigurationError:
        qDebug("This is QModbusDevice::ConfigurationError = %s",modbusDevice->errorString());
        break;

    case QModbusDevice::ProtocolError:
        qDebug("This is QModbusDevice::ProtocolError = %s",modbusDevice->errorString());
        break;

    case QModbusDevice::TimeoutError:
        qDebug("This is QModbusDevice::TimeoutError = %s",modbusDevice->errorString());
        break;

    case QModbusDevice::UnknownError:
        qDebug("This is QModbusDevice::UnknownError = %s",modbusDevice->errorString());
        break;

    default:
        modbusDevice->disconnectDevice();
        modbusDevice->connectDevice();
        qDebug("This is default Error = %s  and modbusDevice->connectDevice();",modbusDevice->errorString());
        break;
    }
}
