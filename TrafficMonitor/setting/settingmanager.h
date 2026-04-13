#ifndef SETTINGMANAGER_H
#define SETTINGMANAGER_H
#include <QObject>
#include <QString>
#include <QDir>
#include <QMap>
#include <utils.h>


class SettingManager: public QObject
{
Q_OBJECT

public:
    SettingManager(QObject* parent = nullptr);
    void    loadSetting(PANEL_PARM& setting);
    void    saveSetting(PANEL_PARM setting);
    void    loadProxySetting(QMap<QString, PROXY_SETTING>& proxyTable);
    void    saveProxySetting(QMap<QString, PROXY_SETTING> proxyTable);
protected:
    void    setDefaultValue(PANEL_PARM& param);


protected:
    QString     m_homeDir;      // home directory
    QString     m_configFile;
    QString     m_configProxyFile;  /// proxy configuratioon file


};

#endif // SETTINGMANAGER_H
