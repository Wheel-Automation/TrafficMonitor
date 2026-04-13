#ifndef STATE_PANEL_H
#define STATE_PANEL_H


#include <QWidget>
#include <QString>
#include <led.h>
#include <utils.h>


namespace Ui {
class state_panel;
}

class state_panel : public QWidget
{
    Q_OBJECT

public:
    explicit state_panel(QWidget *parent = nullptr);
    ~state_panel();
    void initUI();

signals:
    void    showStream();
    void    hideStream();

public slots:
    void    onChangeConnectionState( int state);
    void    onChangePlayState(int state);
public:


    void    set_deviceip(QString device_ip);
    void    set_cameraip(QString camera_ip);

private slots:
    void    on_btnPlay_toggled(bool checked);

protected:

private:
    Ui::state_panel *ui;
    QString     iconPlay, iconStop, iconConnect, iconDisconnect;
    LET_STATE   _playState;

};

#endif // STATE_PANEL_H
