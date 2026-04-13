#include "camera_thread.h"
#include "mainwindow.h"
#include <QReadWriteLock>
#include <QDebug>
QReadWriteLock lock;

camera_thread::camera_thread(QObject* parent):
    QThread(parent)
{
    m_bExit = false;
    // m_param = ((MainWindow*)parent)->get_panel_param();
    // connect(this, SIGNAL(signal_thread_ended()), parent, SLOT(slot_thread_ended()));
}

camera_thread::~camera_thread(){
    free_memory();
}

void camera_thread::free_memory(){

}

void camera_thread::setExit(){
    lock.lockForWrite();
    m_bExit = true;
    lock.unlock();
}

void camera_thread::run(){

    while(!m_bExit){
        msleep(2);

        qDebug()<<"thread running\n";
    }

    // emit signal_thread_ended();
}
