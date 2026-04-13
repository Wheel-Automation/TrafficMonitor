#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

#include <QFrame>
#include "cumstomipeditor.h"
#include "utils.h"

namespace Ui {
class control_panel;
}

class control_panel : public QFrame
{
    Q_OBJECT

public:
    explicit control_panel(QWidget *parent = nullptr);
    ~control_panel();

private slots:
    void on_btn_connect_clicked();

    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

signals:
    void signal_jetson_connnect(PANEL_PARM); // jetson connect signal
    void signal_panel_ok(PANEL_PARM);
    void signal_panel_cancel();

 public:
    PANEL_PARM get_panel_info();
    void set_parameter(PANEL_PARM& param);

private:
    void InitUI();
    void InitEventHandler();
    void free_memory();
    std::string  get_cam_url();
    void EnableDetailInput(bool);

private:
    Ui::control_panel *ui;
    CustomIpEditor* control_jet_ip;
    CustomIpEditor* control_cam_ip;
    PANEL_PARM       panel;
};

#endif // CONTROL_PANEL_H
