#include "frmmain.h"
#include "ui_frmmain.h"
#include "iconhelper.h"
#include "myhelper.h"

frmMain::frmMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);

    myHelper::FormInCenter(this);
    this->InitStyle();
    initSerialPort();




    mStatusMachine = 3;
//    QFont ft;
//    ft.setPointSize(20);
    QPixmap pic(QString(":/image/wait1.png"));
//    ui->label_motor1_name->setFont(ft);
    ui->label_motor1_sta->setPixmap(pic);
    ui->label_motor2_sta->setPixmap(pic);
    ui->label_motor3_sta->setPixmap(pic);
    ui->label_motor4_sta->setPixmap(pic);
    ui->label_fan1_sta->setPixmap(pic);
    ui->label_fan2_sta->setPixmap(pic);
    ui->label_fan3_sta->setPixmap(pic);
    ui->label_s1_sta->setPixmap(pic);
    ui->label_s2_sta->setPixmap(pic);
    ui->label_start_sta->setPixmap(pic);
//    ui->label_start_sta->setVisible(false);

    initQTimer();

}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::InitStyle()
{
    //设置窗体标题栏隐藏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    location = this->geometry();
    max = false;
    mousePressed = false;

    //安装事件监听器,让标题栏识别鼠标双击
    ui->lab_Title->installEventFilter(this);

    IconHelper::Instance()->SetIcon(ui->btnMenu_Close, QChar(0xf00d), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu_Min, QChar(0xf068), 10);
    IconHelper::Instance()->SetIcon(ui->btnMenu, QChar(0xf0c9), 10);
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf015), 12);
    ui->btnMenu_Max->setVisible(false);
    ui->btnMenu_Min->setVisible(false);
    ui->btnMenu->setVisible(false);
}

bool frmMain::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->on_btnMenu_Max_clicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void frmMain::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void frmMain::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void frmMain::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void frmMain::on_btnMenu_Close_clicked()
{
    qApp->exit();
}

void frmMain::on_btnMenu_Max_clicked()
{
    if (max) {
        this->setGeometry(location);
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf096), 10);
        ui->btnMenu_Max->setToolTip("最大化");
    } else {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
        IconHelper::Instance()->SetIcon(ui->btnMenu_Max, QChar(0xf079), 10);
        ui->btnMenu_Max->setToolTip("还原");
    }
    max = !max;
}

void frmMain::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}

void frmMain::initSerialPort(void)
{
    // Set default settings.
    ui->baudRateComboBox->setCurrentText("19200");
    ui->dataBitsComboBox->setCurrentText("8");
    ui->stopBitsComboBox->setCurrentText("1");
    ui->parityComboBox->setCurrentText("0");

    // Load initial settings.
    mSerialPort.setBaudRate(ui->baudRateComboBox->currentText().toInt());
    mSerialPort.setDataBits(static_cast<QSerialPort::DataBits>(
        ui->dataBitsComboBox->currentText().toInt()));
    mSerialPort.setStopBits(static_cast<QSerialPort::StopBits>(
        ui->stopBitsComboBox->currentText().toInt()));
    mSerialPort.setParity(static_cast<QSerialPort::Parity>(
        ui->parityComboBox->currentText().toInt()));
    // Handle settings changed.
    connect(ui->portNameComboBox, &QComboBox::currentTextChanged,
            [=](const QString& val) { mSerialPort.setPortName(val); });

    connect(ui->baudRateComboBox, &QComboBox::currentTextChanged,
            [=](const QString& val) { mSerialPort.setBaudRate(val.toInt()); });

    connect(ui->dataBitsComboBox, &QComboBox::currentTextChanged,
            [=](const QString& val) {
                mSerialPort.setDataBits(
                    static_cast<QSerialPort::DataBits>(val.toInt()));
            });

    connect(ui->stopBitsComboBox, &QComboBox::currentTextChanged,
            [=](const QString& val) {
                mSerialPort.setStopBits(
                    static_cast<QSerialPort::StopBits>(val.toInt()));
            });

    connect(ui->parityComboBox, &QComboBox::currentTextChanged,
            [=](const QString& val) {
                mSerialPort.setParity(
                    static_cast<QSerialPort::Parity>(val.toInt()));
            });

    // List all serial ports.
    foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts())
        ui->portNameComboBox->addItem(info.portName());

    // 接收数据
    connect(&mSerialPort, &QSerialPort::readyRead, [=]() { recvMessage(); });

}

void frmMain::initQTimer(void)
{
    //定时器
    mQTimer = new QTimer(this);
    mQTimer->stop();
    // 设置超时间隔
    mQTimer->setInterval(100); //100ms

//    connect(ui->closeButton, SIGNAL(clicked(bool)), mQTimer, SLOT(stop()));
    connect(mQTimer,SIGNAL(timeout()),this,SLOT(on_time_update()));
}

int frmMain::dataProcess(QByteArray bytearray)
{
    uint16_t crc16 = 0,reccrc16 = 0;
    char *data;
    int re = 0;
    if(bytearray.size() < 34)
        return 1;
    if(bytearray.at(0) != 0x1b || bytearray.at(1) != 0x10)
        return 2;
    data = bytearray.data();
    crc16 = crc((uint8_t *)data, bytearray.size() - 4);
    reccrc16 = bytearray.at(33);
    reccrc16 = (reccrc16 << 8) + bytearray.at(33);
    if(reccrc16 != crc16)
        return 3;
    switch (bytearray.at(2)) {
        case 0x2f:
            flashlabel(bytearray);
        break;
        case 0x7f:      //软件版本号
            re = 0x7f;
        break;
        default:

        break;
    }
    return re;
}

void frmMain::flashlabel(QByteArray bytearray)
{
    QPixmap picwait(QString(":/image/wait1.png"));
    QPixmap picok(QString(":/image/ok.png"));
    QPixmap picng(QString(":/image/ng.png"));
    if(bytearray.at(27) == 1){
        if(bytearray.at(28) & 0x01)
            ui->label_s1_sta->setPixmap(picng);
        else
            ui->label_s1_sta->setPixmap(picok);
        if(bytearray.at(28) & 0x02)
            ui->label_s2_sta->setPixmap(picng);
        else
            ui->label_s2_sta->setPixmap(picok);
        if(bytearray.at(29) & 0x01)
            ui->label_motor1_sta->setPixmap(picng);
        else
            ui->label_motor1_sta->setPixmap(picok);
        if(bytearray.at(29) & 0x02)
            ui->label_motor2_sta->setPixmap(picng);
        else
            ui->label_motor2_sta->setPixmap(picok);
        if(bytearray.at(29) & 0x04)
            ui->label_motor3_sta->setPixmap(picng);
        else
            ui->label_motor3_sta->setPixmap(picok);
        if(bytearray.at(29) & 0x08)
            ui->label_motor4_sta->setPixmap(picng);
        else
            ui->label_motor4_sta->setPixmap(picok);
        if(bytearray.at(29) & 0x10)
            ui->label_fan1_sta->setPixmap(picng);
        else
            ui->label_fan1_sta->setPixmap(picok);
        if(bytearray.at(29) & 0x20)
            ui->label_fan2_sta->setPixmap(picng);
        else
            ui->label_fan2_sta->setPixmap(picok);
        if(bytearray.at(29) & 0x40)
            ui->label_fan3_sta->setPixmap(picng);
        else
            ui->label_fan3_sta->setPixmap(picok);
        if((bytearray.at(29) & 0x7f) == 0x7f)
            ui->label_start_sta->setPixmap(picng);
        else
            ui->label_start_sta->setPixmap(picok);
    }
}

void frmMain::flashlabel(void)
{
    QPixmap picwait(QString(":/image/wait1.png"));
    ui->label_s1_sta->setPixmap(picwait);
    ui->label_s2_sta->setPixmap(picwait);
    ui->label_motor1_sta->setPixmap(picwait);
    ui->label_motor2_sta->setPixmap(picwait);
    ui->label_motor3_sta->setPixmap(picwait);
    ui->label_motor4_sta->setPixmap(picwait);
    ui->label_fan1_sta->setPixmap(picwait);
    ui->label_fan2_sta->setPixmap(picwait);
    ui->label_fan3_sta->setPixmap(picwait);
    ui->label_start_sta->setPixmap(picwait);
}

void frmMain::recvMessage()
{
    int re;
    QByteArray bytearray;
    QByteArray versionarray;

    char buf[50];
    mCount = 0;
    bytearray.append("串口接收数据：");
    bytearray.append(mSerialPort.read(80));
    re = dataProcess(bytearray);
    memcpy(buf,bytearray,bytearray.size() - 2);
    buf[bytearray.size() - 2] = 0;
    if(re == 0x7f) {
        versionarray.append("software version:");
        versionarray.append(buf + 2);
    }
    ui->textBrowser_message->append((const QString &)versionarray);
}

void frmMain::on_time_update(void)
{
    uint8_t sendBuf[10];
    uint16_t crc16;
    sendBuf[0] = 0x1b;
    sendBuf[1] = 0x10;
    sendBuf[2] = 0x2f;
    sendBuf[3] = 0x00;
    sendBuf[4] = 0x00;
    sendBuf[5] = 0x00;
    sendBuf[6] = 0x00;
    sendBuf[7] = 0x00;
    crc16 = crc(sendBuf + 2, 6);
    sendBuf[8] = crc16 & 0xff;
    sendBuf[9] = (crc16 >> 8) & 0xff;
    mSerialPort.write((const char*)sendBuf, 10);
    mSerialPort.waitForBytesWritten(50);
    mCount++;
    if(mCount > 10){ //1000ms没有接收到数据认为已经断开
        flashlabel();
    }
    update();
}

uint16_t frmMain::crc(uint8_t *s,uint16_t len)
{     uint16_t acc = 0,i;
      while (len--)
      {     acc = acc ^ (*s++ << 8);
            for (i = 0; i++ < 8;)
            {     if (acc & 0x8000)
                          acc = (acc << 1) ^ 0x8005;
                  else
                          acc <<= 1;
            }
       }
      return  acc;
}

void frmMain::configCanAdapter(void)
{
    // 设置文本模式
    mSerialPort.setTextModeEnabled(true);
    mSerialPort.flush();
    QTimer::singleShot(500, [=]() {
        ui->openButton->setEnabled(false);
        ui->closeButton->setEnabled(true);
    });

}

void frmMain::on_openButton_clicked()
{
        qDebug() << "PortName:" << mSerialPort.portName();
        qDebug() << "BaudRate:" << mSerialPort.baudRate();
        qDebug() << "DataBits:" << mSerialPort.dataBits();
        qDebug() << "StopBits:" << mSerialPort.stopBits();
        qDebug() << "Parity  :" << mSerialPort.parity();

        // Disable flow control.
        mSerialPort.setFlowControl(QSerialPort::NoFlowControl);

        if (mSerialPort.open(QIODevice::ReadWrite)) {
            configCanAdapter();
        }
        else {
            qDebug() << "SerialPort open failed:" << mSerialPort.error();
        }

}


void frmMain::on_closeButton_clicked()
{
    mSerialPort.close();
    ui->openButton->setEnabled(true);
    ui->closeButton->setEnabled(false);
}





void frmMain::on_comboBox_model_currentTextChanged(const QString &arg1)
{
    QString model_03("03");
    QString model_0304("0304");
    QString model_13("13");
    QString model_1314("1314");
    QString model_180("180");
    if(arg1 == model_03){
        ui->label_motor1_name->setVisible(true);
        ui->label_motor1_sta->setVisible(true);
        ui->label_motor2_name->setVisible(false);
        ui->label_motor2_sta->setVisible(false);
        ui->label_motor3_name->setVisible(false);
        ui->label_motor3_sta->setVisible(false);
        ui->label_motor4_name->setVisible(false);
        ui->label_motor4_sta->setVisible(false);
        ui->label_fan1_name->setVisible(true);
        ui->label_fan1_sta->setVisible(true);
        ui->label_fan2_name->setVisible(false);
        ui->label_fan2_sta->setVisible(false);
        ui->label_fan3_name->setVisible(false);
        ui->label_fan3_sta->setVisible(false);
        ui->label_s1_name->setVisible(true);
        ui->label_s1_sta->setVisible(true);
        ui->label_s2_name->setVisible(true);
        ui->label_s2_sta->setVisible(true);
        ui->label_start_name->setVisible(true);
        ui->label_start_sta->setVisible(true);
    }
    else if(arg1 == model_0304){
            ui->label_motor1_name->setVisible(true);
            ui->label_motor1_sta->setVisible(true);
            ui->label_motor2_name->setVisible(true);
            ui->label_motor2_sta->setVisible(true);
            ui->label_motor3_name->setVisible(true);
            ui->label_motor3_sta->setVisible(true);
            ui->label_motor4_name->setVisible(false);
            ui->label_motor4_sta->setVisible(false);
            ui->label_fan1_name->setVisible(true);
            ui->label_fan1_sta->setVisible(true);
            ui->label_fan2_name->setVisible(false);
            ui->label_fan2_sta->setVisible(false);
            ui->label_fan3_name->setVisible(false);
            ui->label_fan3_sta->setVisible(false);
            ui->label_s1_name->setVisible(true);
            ui->label_s1_sta->setVisible(true);
            ui->label_s2_name->setVisible(true);
            ui->label_s2_sta->setVisible(true);
            ui->label_start_name->setVisible(true);
            ui->label_start_sta->setVisible(true);
        }
    else if(arg1 == model_13){
            ui->label_motor1_name->setVisible(true);
            ui->label_motor1_sta->setVisible(true);
            ui->label_motor2_name->setVisible(false);
            ui->label_motor2_sta->setVisible(false);
            ui->label_motor3_name->setVisible(false);
            ui->label_motor3_sta->setVisible(false);
            ui->label_motor4_name->setVisible(false);
            ui->label_motor4_sta->setVisible(false);
            ui->label_fan1_name->setVisible(true);
            ui->label_fan1_sta->setVisible(true);
            ui->label_fan2_name->setVisible(false);
            ui->label_fan2_sta->setVisible(false);
            ui->label_fan3_name->setVisible(false);
            ui->label_fan3_sta->setVisible(false);
            ui->label_s1_name->setVisible(true);
            ui->label_s1_sta->setVisible(true);
            ui->label_s2_name->setVisible(true);
            ui->label_s2_sta->setVisible(true);
            ui->label_start_name->setVisible(true);
            ui->label_start_sta->setVisible(true);
        }
    else if(arg1 == model_1314){
            ui->label_motor1_name->setVisible(true);
            ui->label_motor1_sta->setVisible(true);
            ui->label_motor2_name->setVisible(true);
            ui->label_motor2_sta->setVisible(true);
            ui->label_motor3_name->setVisible(false);
            ui->label_motor3_sta->setVisible(false);
            ui->label_motor4_name->setVisible(false);
            ui->label_motor4_sta->setVisible(false);
            ui->label_fan1_name->setVisible(true);
            ui->label_fan1_sta->setVisible(true);
            ui->label_fan2_name->setVisible(false);
            ui->label_fan2_sta->setVisible(false);
            ui->label_fan3_name->setVisible(false);
            ui->label_fan3_sta->setVisible(false);
            ui->label_s1_name->setVisible(true);
            ui->label_s1_sta->setVisible(true);
            ui->label_s2_name->setVisible(true);
            ui->label_s2_sta->setVisible(true);
            ui->label_start_name->setVisible(true);
            ui->label_start_sta->setVisible(true);
        }
    else if(arg1 == model_180){
            ui->label_motor1_name->setVisible(true);
            ui->label_motor1_sta->setVisible(true);
            ui->label_motor2_name->setVisible(true);
            ui->label_motor2_sta->setVisible(true);
            ui->label_motor3_name->setVisible(false);
            ui->label_motor3_sta->setVisible(false);
            ui->label_motor4_name->setVisible(false);
            ui->label_motor4_sta->setVisible(false);
            ui->label_fan1_name->setVisible(true);
            ui->label_fan1_sta->setVisible(true);
            ui->label_fan2_name->setVisible(false);
            ui->label_fan2_sta->setVisible(false);
            ui->label_fan3_name->setVisible(false);
            ui->label_fan3_sta->setVisible(false);
            ui->label_s1_name->setVisible(true);
            ui->label_s1_sta->setVisible(true);
            ui->label_s2_name->setVisible(true);
            ui->label_s2_sta->setVisible(true);
            ui->label_start_name->setVisible(true);
            ui->label_start_sta->setVisible(true);
        }
}
