#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QWidget>
#include <QSettings>
#include <QString>

#define EOL_SETTINGS_FILE       "D:/data/settings.ini"
#define EOL_DATA_FILE           "D:/data"

class Settings : public QSettings
{
    Q_OBJECT
public:
    Settings();
//    QVariant getMode(void);
};

#endif // SETTINGS_H
