#ifndef SERIALCTRL_H
#define SERIALCTRL_H

#include <QObject>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtDebug>

#define   CMD_READ_EOL                    0x40
#define   CMD_WRITE_EOL                   0x41
#define   CMD_READACK_EOL                 0xC0
#define   CMD_WRITEACK_EOL                0xC1

#define   CMD_LEN                         0x08


#define BUFFER_SIZE 512
class SerialCtrl : public QObject
{
    Q_OBJECT

public:
    SerialCtrl();

signals:
    void sendACK(void);
    void process(void);
    void SerialCtrl_Process(uint8_t data);

public slots:
    void recvMessage1(void);
    void processData(void);
    void sendDataforGetKeySta(void);    
    void sendDataforSetOutSta(char data);
    uint16_t crc(uint8_t *puchMsg,uint16_t usDataLen);

public:
    QSerialPort     mQSerialPort1;
    uint8_t            mSerialsBuf[BUFFER_SIZE];
    qint32          mReadSeek;
    qint32          mReadTotal;
    qint32          mWriteSeek;
    qint32          mWriteTotal;
    bool            mQSerialPort1Enabled;

};

#endif // SERIALCTRL_H
