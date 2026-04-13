#ifndef DEVICELOGWND_H
#define DEVICELOGWND_H
#include <QObject>
#include <QString>
#include <QDebug>
#include <QMouseEvent>
#include <QPlainTextEdit>

class DeviceLogWnd:public QPlainTextEdit
{
    Q_OBJECT
public:
    DeviceLogWnd(QWidget* parent = nullptr);

signals:
    void itemClicked(QString objectName);

protected slots:
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

};

#endif // DEVICELOGWND_H
