#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>
#include <QDebug>
#include <QString>
#include <QMap>
#include <QList>
#include "macro.h"
#include "utils.h"


namespace Ui {
class LogViewer;
}

struct LOG_RECORD{
    QString _time;
    QString _nWheel;
    QString _nType;
    LOG_RECORD(QString time, QString nWheel, QString nType){
        _time =time; _nWheel = nWheel; _nType = nType;
    }
};

class LogViewer : public QDialog
{
    Q_OBJECT

public:
    explicit LogViewer(QWidget *parent = nullptr);
    ~LogViewer();


private slots:
    void on_combIP_currentIndexChanged(int index);
    void on_tblDate_cellDoubleClicked(int row, int column);

private:
    void        initUIControls();
    void        loadLogFileNames();
    void        loadLogFile(QString logFile);

    void        logWindow();

private:
    Ui::LogViewer *ui;
    QList<QPair<QString, QStringList>>       m_logFileMap;   // key: device ip, value: log file name list
    int         m_nInitDateRow;         // init row count of date table view
    int         m_nInitContentRow;      // init row count of log content view

};

#endif // LOGVIEWER_H
