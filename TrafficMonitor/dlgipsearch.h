#ifndef DLGIPSEARCH_H
#define DLGIPSEARCH_H

#include <QDialog>
#include <QVector>
#include <QMutex>
#include <QList>
#include <QTimer>
#include "macro.h"
#include "cumstomipeditor.h"
#include "ipsearch.h"
#include "device_serach_worker.h"


struct Device{
    QString dev_ip;
    QString dev_name;
};



namespace Ui {
class dlgipsearch;
}

class dlgipsearch : public QDialog
{
    Q_OBJECT

public:
    explicit dlgipsearch(QWidget *parent = nullptr, int deviceType = DEVTYPE);
    ~dlgipsearch();

public slots: 
	void	timerEvent();

public:
    void    addIpAddress(QString ip, QString devname) ;

private:
    void    initUI();
    void    closeEvent (QCloseEvent *event);
	void	startProgTimer(); 
	void	clearDeviceList(); 

    // start to search IP device in local(Hi3519)
    void    startLocalIpSearch();
    // stop search IP device in local(Hi3519)
    void    stopLocalIpsearch();


private:
    Ui::dlgipsearch *ui;

    QVector<QString>        _ipresult;
    bool                    _bStarted;
    ipsearch*               _srch;
    device_serach_worker*    _srch_rv;
    QList<Device>           _searchList;
    //QTimer*                  _timer;
    int                     _searchTime;  // ip search time in ms
	int						_cntTime; 
	int						_interval; 
	QTimer*					_timer; 
	QMutex					_mutex; 

signals:
    void stopSearch();
	

private slots:
    void onButtonStart();
    void OnButtonClear();
    void onIPRetreived(QString device_ip, QString device_name);

 private:
    int m_devType;


};

#endif // DLGIPSEARCH_H
