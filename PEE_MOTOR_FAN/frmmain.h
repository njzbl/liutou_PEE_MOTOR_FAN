#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QDebug>
#include <QState>
#include <QSignalTransition>
#include <QEventTransition>
#include <QStateMachine>
#include <QGraphicsDropShadowEffect>
#include <QNetworkInterface>
#include <QTimer>
#include <QVector>
#include <QLine>
#include <QIntValidator>

namespace Ui {
class frmMain;
}

class frmMain : public QDialog
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void initSerialPort(void);
    void initQTimer(void);

private slots:
    void InitStyle();
    void recvMessage();
    void configCanAdapter(void);
    uint16_t crc(uint8_t *s,uint16_t len);

    void on_btnMenu_Close_clicked();

    void on_btnMenu_Max_clicked();

    void on_btnMenu_Min_clicked();

    void on_time_update();

    void on_openButton_clicked();

    void on_closeButton_clicked();

    void on_comboBox_model_currentTextChanged(const QString &arg1);

public slots:

private:
    Ui::frmMain *ui;

    QPoint mousePoint;
    bool mousePressed;
    bool max;
    QRect location;
    QSerialPort         mSerialPort;


    qint32 iRet;
    QTimer *mQTimer;
    uint8_t mStatusMachine;


};

#endif // FRMMAIN_H
