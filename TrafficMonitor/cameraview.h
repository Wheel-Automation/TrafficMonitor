#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QLabel>
#include <QWidget>
#include <Qt>
#include <QMouseEvent>


class CameraView: public QLabel
{
    Q_OBJECT

public:
    explicit CameraView(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~CameraView();

signals:
    void signal_mouse_pressed(QMouseEvent*);
    void signal_mouse_moved(QMouseEvent*);
    void signal_mouse_released();

protected:
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
};

#endif // CAMERAVIEW_H
