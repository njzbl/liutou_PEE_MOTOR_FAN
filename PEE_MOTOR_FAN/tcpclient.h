#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QWidget>
#include <QtNetWork>
#include <QTcpSocket>
#include <QDebug>

class TcpClient: public QObject
{
    Q_OBJECT
public:
    TcpClient();
    ~TcpClient();

signals:
    void connectedStatus(bool status);
    void receiveData_MES(QByteArray data);

public slots:

    void setIp(QString ipAddress,int ipPort);
    void newTCPConnect(void);
    void disconnectTcp(void);
    void sendData(QByteArray data);

private slots:
   void readMessage(void); //接收来自服务端的数据
   void displayError(QAbstractSocket::SocketError);

private:
    QTcpSocket *mTcpSocket;
    QString mIpAddress;
    int mIpPort;
public:
    bool isConnected;
};

#endif // TCPCLIENT_H
