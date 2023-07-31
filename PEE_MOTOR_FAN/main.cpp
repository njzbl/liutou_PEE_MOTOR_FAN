#include "frmmain.h"
#include <QApplication>
#include "myhelper.h"
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    myHelper::SetUTF8Code();
    myHelper::SetStyle("black");//黑色风格
//    myHelper::SetStyle("blue");//蓝色风格
//    myHelper::SetStyle("gray");//灰色风格
//    myHelper::SetStyle("navy");//天蓝色风格
    myHelper::SetChinese();


    QTranslator *translator = new QTranslator;
    bool re = translator->load("D:/workspace/project/QT_APP/liutou_PEE_MOTOR_FAN/PEE_MOTOR_FAN/Translations_china.qm");
    if(re == false)
        qDebug()<<"false";
    else
        qDebug()<<"true";
    re = a.installTranslator(translator);
    if(re == false)
        qDebug()<<"false2";
    else
        qDebug()<<"true2";

    frmMain w;
    w.show();

    return a.exec();
}
