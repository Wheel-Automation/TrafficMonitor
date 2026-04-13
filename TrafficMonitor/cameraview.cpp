#include "cameraview.h"
#include <QLabel>


CameraView::CameraView(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {

}
CameraView::~CameraView(){

}

void CameraView::mouseMoveEvent(QMouseEvent *ev){
    emit signal_mouse_moved(ev);

}


void CameraView::mousePressEvent(QMouseEvent* ev){
    emit signal_mouse_pressed(ev);

}

void CameraView::mouseReleaseEvent(QMouseEvent(* ev)){
    emit signal_mouse_released();
}



