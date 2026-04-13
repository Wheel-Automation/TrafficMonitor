#ifndef DLG_LOGIN_H
#define DLG_LOGIN_H

#include <QDialog>
#include "cumstomipeditor.h"
#include "utils.h"
namespace Ui {
class dlg_login;
}

class dlg_login : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_login(QWidget *parent = nullptr, bool bPrevLogin = false, QString devIP = "0.0.0.0", QString devPwd = "", DETECT_MODE det_mode = DETECT_ONE_DIRECT, int nprevcamera = 2);
    ~dlg_login();
    void on_log_in_success(bool bSuccess);
    void on_reset_pwd_event(bool success);
signals:
    void signal_login(DETECT_MODE det_mode, QString devip, QString devpwd, int cam_index);  // login signal
    void update_password(QString szNewPwd);

private slots:
    void on_btn_ok_clicked();
    void on_btn_reset_pwd_clicked();

    void on_btn_close_clicked();

    //void on_com_ctrl_kind_currentIndexChanged(int index);

private :
    void initUI();


private:
    Ui::dlg_login *     ui;
    CustomIpEditor*     device_ip;
    QString             _szDevIP, _szDevPwd;
    bool                _bLogedIn;
    //DETECT_MODE         _det_mode;

    int                 _nPrevCamera;
};

#endif // DLG_LOGIN_H
