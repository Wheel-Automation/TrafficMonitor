#ifndef COM_H
#define COM_H
#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QList>
class Com : public QObject
{
    Q_OBJECT
public:
    Com(QObject* parent = nullptr);
    static QStringList      findAllComPorts();
    bool            open(QString portName);
    bool            close();
    bool            isOpened();
    QString         sendWheelData(int nWheel, int nWhlType);

signals:
    void errorOccured(QString errorString);


public slots:
    void onErrorOccured(QSerialPort::SerialPortError error);

private:
    static QString byteToString(QByteArray array);

    //void   closePort(QSerialPort* serial);


private:

    QSerialPort*        m_serial;


};

#endif // COM_H
