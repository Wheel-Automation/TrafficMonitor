#ifndef LOGWRITERTHREAD_H
#define LOGWRITERTHREAD_H

#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QList>
#include <QTime>
#include <QQueue>

struct LOG_UNIT{
    int count; // wheel count
    QString type; // wheel type
    LOG_UNIT(){
        count = 0;
        type  = "";
    }
    LOG_UNIT(int nwheel, QString wheelType){
        count = nwheel;
        type = wheelType;

    }
};

class logWriterThread : public QObject
{
    Q_OBJECT
public:
    explicit logWriterThread(QObject* parent = nullptr);
    void     addLog(QString camIP, int nWheel, QString wheelType);
    void     changeKeepLogDays(int nDays);
    void     stop();

public slots:
    void     run();


signals:
    void     finished();

private:
    bool     readLog(QString &camIP, LOG_UNIT& logData);
    bool     enableThread();
    QString  getLogFileName(QString camIP);
    bool     removeOldLogFiles();

private:
    bool                            m_bEnableThread;
    QReadWriteLock                  m_locker;
    QString                         m_camIP;
    LOG_UNIT                        m_data;
    bool                            m_bupdated;
    QString                         m_logHomeFolder;
    int                             m_nKeepLogDays; // days to keep log files
};

#endif // LOGWRITERTHREAD_H
