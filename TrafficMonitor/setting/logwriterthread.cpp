#include "logwriterthread.h"
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QElapsedTimer>
#include <QDateTime>
#include <QDate>
#include "utils.h"


using namespace std;

logWriterThread::logWriterThread(QObject* parent):
    QObject(parent),
    m_bupdated(false),
    m_bEnableThread(true),
    m_nKeepLogDays(7)
{
    // make a log folder
    m_logHomeFolder = Utils::getLogHomeDir();
}

void     logWriterThread::addLog(QString camIP, int nWheel, QString wheelType){
    m_locker.lockForWrite();
    m_bupdated = true;
    m_data = LOG_UNIT(nWheel, wheelType);
    m_camIP = camIP;
    m_locker.unlock();
}

void logWriterThread::changeKeepLogDays(int nDays){
    m_locker.lockForWrite();
    m_nKeepLogDays = nDays;
    m_locker.unlock();

}
bool  logWriterThread::readLog(QString &camIP, LOG_UNIT& logData){
    bool bRes = false;
    m_locker.lockForRead();
    bRes = m_bupdated;
    if(bRes)
        logData =m_data;
    camIP = m_camIP;
    m_locker.unlock();

    if(bRes){
        m_locker.lockForWrite();
        m_bupdated = false;
        m_locker.unlock();
    }
    return bRes;
}

void logWriterThread::stop(){
    m_locker.lockForWrite();
    m_bEnableThread = false;
    m_locker.unlock();
}

bool  logWriterThread::enableThread(){
    bool bEnable = true;
    m_locker.lockForRead();
    bEnable = m_bEnableThread;
    m_locker.unlock();
    return bEnable;
}

QString  logWriterThread::getLogFileName(QString camIP){
    QString logFolder = QString("%1/%2").arg(m_logHomeFolder).arg(camIP);
    if(!QDir(logFolder).exists()){
        QDir().mkdir(logFolder);
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString logFile = QString("%1/%2.csv").arg(logFolder, timestamp);
    return logFile;
}

bool     logWriterThread::removeOldLogFiles(){

    QStringList subDirs = Utils::findAllSubDirectories(m_logHomeFolder);
    QDate today = QDate::currentDate();
    QList<QString>   removeFiles;
    for(auto subDir:subDirs){
        QStringList logFiles = Utils::findAllLogFiles(subDir);
        if(logFiles.size() <= 0){
            continue;
        }
        // check files
        for(auto file: logFiles){
            QString fileName = Utils::FilePathToName(file);
            QString dateOfFile = QFileInfo(file).completeBaseName();
            QStringList elems = dateOfFile.split('-', Qt::SkipEmptyParts);
            if(elems.size() != 3){
                removeFiles.append(file);
                continue;
            }
            bool by, bm, bd;
            int y = elems[0].toInt(&by);
            int m = elems[1].toInt(&bm);
            int d = elems[2].toInt(&bd);
            if(!by || !bm || !bd ){
                removeFiles.append(file);
                continue;
            }
            QDate fileDate = QDate(y,m,d);
            int nElapsedDays = int(today.toJulianDay() - fileDate.toJulianDay());
            if(nElapsedDays > m_nKeepLogDays){
                removeFiles.append(file);
            }
        }
    }
    for(auto file: removeFiles){
        QFile ff(file);
        if(ff.exists()){
            ff.remove();
        }
    }
    return true;
}

void logWriterThread::run(){
    qDebug() << "start log thread";
    QString     camIP;
    LOG_UNIT     logData;
    QString      logFileName;

    removeOldLogFiles();
    QElapsedTimer      timer;
    while(true){
        if(!enableThread())
            break;

        if(!readLog(camIP, logData))
        {
            QThread::msleep(10);
            continue;
        }
        logFileName = getLogFileName(camIP);
        // qDebug() << "log file name" << logFileName;

        QString tm = QDateTime::currentDateTime().toString("hh:mm:ss");
        QString line = QString("%1,%2,%3\n").arg(tm).arg(logData.count).arg(logData.type);
        std::string sLine = line.toStdString();

        qint64 elapsedSec = timer.elapsed()/1000;
        //qDebug() << "elapsed ms" << elapsedSec;

        if(elapsedSec >= 60 * 60 * 12){ // remove old log file every 1 hours
            removeOldLogFiles();
            timer.restart();
        }


        try {// write to log file
            std::string fn = logFileName.toStdString();
            FILE* fp;
            if(fopen_s(&fp,fn.c_str(), "a") != 0){
                qDebug() << "failed to open log file";
                continue;
            }
            fwrite(sLine.c_str(),1, sLine.length(), fp);
            fclose(fp);
        } catch (...) {
            qDebug() << "Failed to write log";
        }
        QThread::msleep(10);
    }
    emit finished();
    return;
}
