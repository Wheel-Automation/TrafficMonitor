#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

#include <QThread>
#include    "utils.h"

class camera_thread : public QThread
{
    Q_OBJECT

public:
    explicit camera_thread(QObject* parent=nullptr);
private:
    ~camera_thread();
    void free_memory();

public:
    void run() override;
    void setExit();


private:
    PANEL_PARM m_param;
    bool        m_bExit;

signals:
    void signal_thread_ended();

};

#endif // CAMERA_THREAD_H
