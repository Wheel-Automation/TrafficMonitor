#ifndef WARNINGDLG_H
#define WARNINGDLG_H

#include <QDialog>
#include <QFrame>

namespace Ui {
class WarningDlg;
}

class WarningDlg : public QDialog
{
    Q_OBJECT

public:
    explicit WarningDlg(QWidget *parent = nullptr);
    ~WarningDlg();

private:
    Ui::WarningDlg *ui;
};

#endif // WARNINGDLG_H
