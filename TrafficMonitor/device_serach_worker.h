#ifndef DEVICE_SERACH_WORKER_H
#define DEVICE_SERACH_WORKER_H


#include <QObject>
#include <QString>

class device_serach_worker: public QObject
{
    Q_OBJECT
public:
	explicit device_serach_worker(int timeout,  QObject* parent = nullptr);
signals:
    void finished();
    void foundDeviceIP(QString ip, QString deviceName);

public slots:
    void stopSearch();
    void run();
private:
    bool	_bEnable;
	int		_searchTimeout; 
};

#endif // DEVICE_SERACH_WORKER_H
