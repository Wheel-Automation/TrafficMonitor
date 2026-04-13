#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QObject>
#include <QTableWidget>

class MyTableWidget: public QTableWidget
{
 Q_OBJECT
public:
    MyTableWidget(QWidget* parent = nullptr);

signals:
    void userActionTiggered(QString objectName);

protected:
    void initTableWidget();

protected slots:
    void wheelEvent(QWheelEvent* event);

};

#endif // MYTABLEWIDGET_H
