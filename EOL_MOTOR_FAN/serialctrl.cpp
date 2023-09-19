#include "serialctrl.h"

SerialCtrl::SerialCtrl()
{
    mQSerialPort1Enabled = false;
    mReadSeek = 0;
    mReadTotal = 0;
    mWriteSeek = 0;
    mWriteTotal = 0;
//    connect(this, SIGNAL(readyRead()),this,SLOT(recvMessage1()));
    connect(this, SIGNAL(process()),this,SLOT(processData()));
}

void SerialCtrl::recvMessage1(void)
{
    char buf[512];
    qint32 maxLen = sizeof(mSerialsBuf);

    qint32 len = mQSerialPort1.read(buf,sizeof(buf));
    if(len <= 0)
        return;
    if(maxLen - mWriteSeek >= len){
        memcpy(mSerialsBuf + mWriteSeek, buf, len);
        mWriteSeek += len;
        if(mWriteSeek >= maxLen)
            mWriteSeek = mWriteSeek % maxLen;
    }
    else {
        memcpy(mSerialsBuf + mWriteSeek, buf, maxLen - mWriteSeek);
        memcpy(mSerialsBuf, buf + (maxLen - mWriteSeek), len - (maxLen - mWriteSeek));
        mWriteSeek = len - (maxLen - mWriteSeek);
    }
    mWriteTotal += len;
    emit process();
//    qDebug() << "emit process();";
    return;
}

void SerialCtrl::processData(void)
{

    int cmdLen = 0;
    uint16_t crc16val = 0;
    uint8_t keyval = 0;
    for( ;mReadTotal < mWriteTotal - 4; ) {
        if( mSerialsBuf[(mReadTotal + 0) % BUFFER_SIZE] != 0x1B ){
            mReadTotal++;
            qDebug() << " !0x1b";
            continue;
        }
        if( mSerialsBuf[(mReadTotal + 1) % BUFFER_SIZE] != 0x10){
            mReadTotal += 2;
            qDebug() << " !0x1b 0x10";
            continue;
        }
        if(mReadTotal < mWriteTotal - CMD_LEN)
            return;
        crc16val = crc(mSerialsBuf + (mReadTotal % BUFFER_SIZE), CMD_LEN);
//        if((((crc16val >> 8) & 0xff) != mSerialsBuf[(mReadTotal + CMD_LEN - 2) % BUFFER_SIZE]) || ((crc16val & 0xff) != mSerialsBuf[(mReadTotal + CMD_LEN - 1) % BUFFER_SIZE]))
//        {
        if(crc16val != 0) {
            mReadTotal = mReadTotal + CMD_LEN;       //丢弃本次非法数据
            qDebug("crc16val = 0x%x,0x%x,0x%x mReadTotal = %d mWriteTotal = %d mWriteSeek = %d",crc16val,mSerialsBuf[(mReadTotal + CMD_LEN - 2) % BUFFER_SIZE],mSerialsBuf[(mReadTotal + CMD_LEN - 1) % BUFFER_SIZE],mReadTotal,mWriteTotal,mWriteSeek);
            qDebug("0x%x,%x,%x,%x,%x,%x,%x,%x",mSerialsBuf[(mReadTotal % BUFFER_SIZE)]
                    ,mSerialsBuf[(mReadTotal + 1) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 2) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 3) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 4) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 5) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 6) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 7) % BUFFER_SIZE]);
            return;
        }
        else {
            qDebug("crc is ok %x,%x,%x,%x,%x,%x,%x,%x",mSerialsBuf[(mReadTotal % BUFFER_SIZE)]
                    ,mSerialsBuf[(mReadTotal + 1) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 2) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 3) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 4) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 5) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 6) % BUFFER_SIZE]
                    ,mSerialsBuf[(mReadTotal + 7) % BUFFER_SIZE]);
        }
        keyval = uint8_t(mSerialsBuf[(mReadTotal + 3) % BUFFER_SIZE]);
        mReadTotal = mReadTotal + CMD_LEN;
        emit SerialCtrl_Process(keyval);
//        qDebug("emit SerialCtrl_Process(0x%x);",keyval);
    }
}

void SerialCtrl::sendDataforGetKeySta(void)
{
    char sendBuf[10];
    uint16_t crc16;
    if(mQSerialPort1Enabled == false)
           return;
    //>>>>>>>>>>>>>>>>  读IO板状态 >>>>>>>>>>>>>>>>>>
    sendBuf[0] = 0x1b;
    sendBuf[1] = 0x10;
    sendBuf[2] = CMD_READ_EOL;
    sendBuf[3] = 0x00;
    sendBuf[4] = 0x00;
    sendBuf[5] = 0x00;
    crc16 = crc((uint8_t *)sendBuf, CMD_LEN - 2);
    sendBuf[6] = (crc16 >> 8) & 0xff;
    sendBuf[7] = crc16 & 0xff;
    mQSerialPort1.write((const char*)sendBuf, CMD_LEN);
    mQSerialPort1.waitForBytesWritten(50);
    qDebug("sendDataforGetKeySta(0x%x);",CMD_READ_EOL);
    //<<<<<<<<<<<<<<<<  读IO板状态 <<<<<<<<<<<<<<<<<
}

void SerialCtrl::sendDataforSetOutSta(char data)
{
    char sendBuf[10];
//    uint8_t Buf[10];
    uint16_t crc16;
    if(mQSerialPort1Enabled == false)
           return;
    //>>>>>>>>>>>>>>>>  读IO板状态 >>>>>>>>>>>>>>>>>>
    sendBuf[0] = 0x1b;
    sendBuf[1] = 0x10;
    sendBuf[2] = CMD_WRITE_EOL;
    sendBuf[3] = data;
    sendBuf[4] = 0x00;
    sendBuf[5] = 0x00;
    crc16 = crc((uint8_t *)sendBuf, CMD_LEN - 2);
    sendBuf[6] = (crc16 >> 8) & 0xff;
    sendBuf[7] = crc16 & 0xff;
    mQSerialPort1.write((const char*)sendBuf, CMD_LEN);
    mQSerialPort1.waitForBytesWritten(50);
    //<<<<<<<<<<<<<<<<  读IO板状态 <<<<<<<<<<<<<<<<<
    qDebug("sendDataforSetOutSta(0x%x);",data);
//    for(int i = 0;i < 8;i++) {
//        Buf[i] = (uint8_t)sendBuf[i];
//    }
//    qDebug("sendDataforGetKeySta(0x%x,%x,%x,%x,%x,%x,%x,%x);",Buf[0],Buf[1],Buf[2],Buf[3]
//                                                            ,Buf[4],Buf[5],Buf[6],Buf[7]);
}

//CRC-16/modbus   x16+x15+x2+1
uint16_t SerialCtrl::crc(uint8_t *puchMsg,uint16_t usDataLen)
{
    uint16_t wCRCin = 0x0000;
    uint16_t wCPoly = 0x1021;
    uint8_t wChar = 0;

    while (usDataLen--) {
    wChar = *(puchMsg++);
    // InvertUint8(&wChar,&wChar);
    wCRCin ^= (wChar << 8);
    for (int i = 0; i < 8; i++) {
        if (wCRCin & 0x8000)
        wCRCin = (wCRCin << 1) ^ wCPoly;
        else
        wCRCin = wCRCin << 1;
    }
    }
    // InvertUint16(&wCRCin,&wCRCin);
    return wCRCin; // return (wCRCin^0xFFFF) ;//
}
