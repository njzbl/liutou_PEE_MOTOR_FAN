#ifndef OPERTIONEXCEL_H
#define OPERTIONEXCEL_H

#include <QObject>
#include <QWidget>
#include <QAxObject>
#include <QAxWidget>
#include <QList>
#include <QVariant>
#include <QDir>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QString>
#include <QQueue>
#include <QSemaphore>

class OpertionExcel : public QThread
{
    Q_OBJECT

public:
    explicit OpertionExcel(QThread *parent = nullptr);

protected:
    virtual void run();

public:
    QAxObject *excel;
};

#endif // OPERTIONEXCEL_H
