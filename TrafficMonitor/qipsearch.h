#ifndef QIPSEARCH_H
#define QIPSEARCH_H


#include <QTcpSocket>


class QIpSearch : public QTcpSocket
{
    Q_OBJECT
public:
    QIpSearch( int sock, QObject *parent=0, const char *name=0 ) :
        QTcpSocket( parent, name )
    {
        line = 0;
        connect( this, SIGNAL(readyRead()),
                SLOT(readClient()) );
        connect( this, SIGNAL(connectionClosed()),
                SLOT(deleteLater()) );
        setSocket( sock );
    }

    ~ClientSocket()
    {
    }

signals:
    void logText( const QString& );

private slots:
    void readClient()
    {
        QTextStream ts( this );
        while ( canReadLine() ) {
            QString str = ts.readLine();
            emit logText( tr("Read: '%1'\n").arg(str) );

            ts << line << ": " << str << endl;
            emit logText( tr("Wrote: '%1: %2'\n").arg(line).arg(str) );

            line++;
        }
    }

private:
    int line;
};


#endif // QIPSEARCH_H
