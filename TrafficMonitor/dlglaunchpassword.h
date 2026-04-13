#ifndef DLGLAUNCHPASSWORD_H
#define DLGLAUNCHPASSWORD_H

#include <QDialog>

namespace Ui {
class DlgLaunchPassword;
}

class DlgLaunchPassword : public QDialog
{
    Q_OBJECT

public:
    explicit DlgLaunchPassword(QWidget *parent = nullptr);
    ~DlgLaunchPassword();

    bool IsVerified(){return _bVerified;}
protected:
    bool    _bVerified;
private:
    Ui::DlgLaunchPassword *ui;
    void closeEvent(QCloseEvent *event);

public slots:
    void onButtonOkay();



};

#endif // DLGLAUNCHPASSWORD_H
