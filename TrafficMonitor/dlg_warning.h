#ifndef DLG_WARNING_H
#define DLG_WARNING_H

#include <QDialog>
#include <QString>
#include <utils.h>

namespace Ui {
class dlg_warning;
}

class dlg_warning : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_warning(QWidget *parent = nullptr);
    explicit dlg_warning(QWidget* parent = nullptr, QString msg = "", PROMPT_TYPE type = e_prompt_note);
    ~dlg_warning();

signals:
    void signal_close();

private slots:


    void on_btn_okay_clicked();

private:
    Ui::dlg_warning *ui;
};

#endif // DLG_WARNING_H
