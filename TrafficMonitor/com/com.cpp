#include <QByteArray>
#include <QDebug>
#include <QTextCodec>
#include "macro.h"
#include "com.h"

Com::Com(QObject* parent):
    QObject(parent),
    m_serial(nullptr)
{

}

QStringList   Com::findAllComPorts(){
    QList<QString> portNames;

    foreach(QSerialPortInfo port, QSerialPortInfo::availablePorts()){
        QSerialPort serial;
         serial.setPort(port);
         portNames.append(port.portName());
        // if(serial.open(QIODevice::ReadWrite))
        // {
        //     //portNames.append(port.portName());
        //     serial.close();
        // }

    }
    QStringList names(portNames);
    names.sort();
    return names;
}


// open comports
bool Com::open(QString portName)
{
    close();

    m_serial = new QSerialPort;
    //Set serial port name
    m_serial->setPortName(portName);
    //Open serial port
    if(!m_serial->open(QIODevice::ReadWrite))
    {
        close();
        return false;
    }
    //set baud rate
    m_serial->setBaudRate(9600);
    //Set the number of data bits
    m_serial->setDataBits(QSerialPort::Data8);

    //Set parity
    m_serial->setParity(QSerialPort::NoParity);

    //Set stop bit
    m_serial->setStopBits(QSerialPort::OneStop);

    //set flow control
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    //Connecting signal slot
    QObject::connect(m_serial, &QSerialPort::errorOccurred, this, &Com::onErrorOccured);
    // QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Data);

    return true;
}

// close comport
bool Com::close(){
    if(m_serial == nullptr)
        return true;
    if(!m_serial->isOpen())
    {
        //m_serial->deleteLater();
        delete m_serial;
        m_serial = nullptr;
        qDebug() << "success to close comport";
        return true;
    }
    qDebug() << "close comport-1";
    //m_serial->clear();
    m_serial->close();
    qDebug() << "close comport-3";
    m_serial->deleteLater();
    qDebug() << "close comport-4";
    //delete m_serial;
    m_serial = nullptr;
    qDebug() << "close comport-5";
    qDebug() << "success to close comport";
    return true;
}
bool   Com::isOpened(){
    if(m_serial != nullptr && m_serial->isOpen())
        return true;
    return false;

}


QString   Com::sendWheelData( int nWheel, int nWhlType){
    if(!isOpened()) // failed to send data
        return "";


#if COMPORT_SEND_WHLTYPE
    int nLen = 5 + nWheel;
    QByteArray ba(nLen, 0);
    QByteArray whlHex;

    ba[0] = 0xA5;
    ba[1] = 0x5A;
    ba[2] = nWheel;
    ba[nLen -2] = 0x5A;
    ba[nLen - 1] = 0xA5;
    // set wheel type
    for(int ii = 0; ii < nWheel; ii ++){
        int nType = nWhlType % 10;
        nWhlType = nWhlType/10;
        ba[nLen-3-ii] = nType;
    }

#else
    //BYTE	PARKET[5] = { 0xa5, 0x5a, nWheel, 0x5a, 0xa5};
    QByteArray ba(5, 0);
    QByteArray whlHex;
    whlHex.setNum(nWheel);

    uint8_t val = uint8_t(nWheel%256);
    ba.resize(5);
    ba[0] = 0xA5;
    ba[1] = 0x5A;
    ba[2] = nWheel;
    ba[3] = 0x5A;
    ba[4] = 0xA5;
    QByteArray byteHexString = ba.toHex();
#endif

    int nWritten = m_serial->write(ba);
    if(nWritten > 0 &&m_serial->waitForBytesWritten(1000)){
        qDebug() << "Success to write comport data, written bytes:" << nWritten;
    }
    else{
        qDebug() <<"timout error in comport writting";
        return "";
    }


    //QString result = QString(byteHexString );
    QString result = byteToString(ba);
    return result;
}


QString    Com::byteToString(QByteArray array){
    QString result = "";
    for(int i = 0; i < array.length(); i ++){
        QByteArray one(1, 0);
        one[0] = array[i];
        QByteArray oneHex = one.toHex();
        QString cell = QString(oneHex).toUpper();
        result = QString("%1 0x%2").arg(result).arg(cell);
    }
    return result;
}

void Com::onErrorOccured(QSerialPort::SerialPortError error){
    QString errorString;
    switch (int(error)) {
    case QSerialPort::SerialPortError::NoError:
        errorString = "NoError";
        break;
    case QSerialPort::SerialPortError::DeviceNotFoundError:
        errorString = "DeviceNotFoundError";
        break;
    case QSerialPort::SerialPortError::PermissionError:
        errorString = "PermissionError";
        break;

    case QSerialPort::SerialPortError::OpenError:
        errorString = "OpenError";
        break;
    case QSerialPort::SerialPortError::ParityError:
        errorString = "ParityError";
        break;
    case QSerialPort::SerialPortError::FramingError:
        errorString = "FramingError";
        break;
    case QSerialPort::SerialPortError::BreakConditionError:
        errorString = "BreakConditionError";
        break;
    case QSerialPort::SerialPortError::WriteError:
        errorString = "WriteError";
        break;
    case QSerialPort::SerialPortError::ReadError:
        errorString = "ReadError";
        break;
    case QSerialPort::SerialPortError::ResourceError:
        errorString = "ResourcError";
        break;
    case QSerialPort::SerialPortError::UnsupportedOperationError:
        errorString = "UnsupportedOperationError";
        break;
    case QSerialPort::SerialPortError::UnknownError:
        errorString = "UnknownError";
        break;
    case QSerialPort::SerialPortError::TimeoutError:
        errorString = "TimeoutError";
        break;
    case QSerialPort::SerialPortError::NotOpenError:
        errorString = "NotOpenError";
        break;
    }

    emit errorOccured(errorString);
}

