#ifndef DLG_CAMERA_SETTING_H
#define DLG_CAMERA_SETTING_H

#include <QDialog>
#include "cumstomipeditor.h"
#include "utils.h"

namespace Ui {
class dlg_camera_setting;
}

class dlg_camera_setting : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_camera_setting(QWidget *parent = nullptr, PANEL_PARM panel = PANEL_PARM());
    ~dlg_camera_setting();

    void on_set_param(bool bSucces);
    void enable_editable(bool bEditable);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();



signals:
    void  signal_get_param(PANEL_PARM panel);
private:
    void initUI();
    void freeMemory();

private:
    Ui::dlg_camera_setting *ui;
    // CustomIpEditor* ip_cam;
    CustomIpEditor* ip_server;
    PANEL_PARM      _panel;
};

#endif // DLG_CAMERA_SETTING_H
