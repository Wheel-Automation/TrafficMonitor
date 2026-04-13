#ifndef LOGSETTINGDLG_H
#define LOGSETTINGDLG_H

#include <QDialog>

namespace Ui {
class LogSettingDlg;
}

class LogSettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LogSettingDlg(QWidget *parent = nullptr);
    ~LogSettingDlg();
    void setKeepDay(int nDay);

signals:
    void upDateSaveDay(int nSaveDay);

private slots:
    void on_editLogDay_textChanged(const QString &arg1);

    void on_btnOkay_clicked();

    void on_btnCancel_clicked();

    bool checkSaveDay(QString szDayText);

private:
    Ui::LogSettingDlg *ui;

};

#endif // LOGSETTINGDLG_H
