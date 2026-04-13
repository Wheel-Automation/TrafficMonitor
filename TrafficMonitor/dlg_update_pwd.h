#ifndef DLG_UPDATE_PWD_H
#define DLG_UPDATE_PWD_H

#include <QDialog>

namespace Ui {
class dlg_update_pwd;
}

class dlg_update_pwd : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_update_pwd(QWidget *parent = nullptr);
    ~dlg_update_pwd();

private slots:
    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

    // void on_edit_pwd_textEdited(const QString &arg1);
    // void on_edit_cfm_pwd_textEdited(const QString &arg1);
    void on_edit_pwd_textChanged(const QString &arg1);

    void on_edit_cfm_pwd_textChanged(const QString &arg1);

signals:
    void signal_new_pwd(bool updated, QString newPwd);

private:
    void initUI();

private:
    Ui::dlg_update_pwd *ui;
    QString     szCurPwd;
    QString     szRePwd;
};

#endif // DLG_UPDATE_PWD_H
