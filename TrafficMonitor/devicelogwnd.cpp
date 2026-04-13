#include "devicelogwnd.h"

DeviceLogWnd::DeviceLogWnd(QWidget* parent):
    QPlainTextEdit(parent)
{

}

void DeviceLogWnd::mouseReleaseEvent(QMouseEvent *event){
    emit itemClicked("DeviceLogWnd");
}

void DeviceLogWnd::wheelEvent(QWheelEvent *event){
    emit itemClicked("DeviceLogWnd");
}
