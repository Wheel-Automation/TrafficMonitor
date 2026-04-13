#ifndef DLGPARAMSETTING_H
#define DLGPARAMSETTING_H

#include <QDialog>
#include "utils.h"

namespace Ui {
class DlgParamSetting;
}

class DlgParamSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DlgParamSetting(QWidget *parent = nullptr);
    explicit DlgParamSetting(QWidget *parent, PANEL_PARM paramSets);

    ~DlgParamSetting();

signals:
    void modifyParam(PANEL_PARM);

public:
    void onParamSetResult( bool bRes);

protected:
    void initParameters();
    void visibleLedTimeSetting(bool bEnable);
private slots:
    void on_btnOK_clicked();

    void on_btnClose_clicked();

    void on_checkLed_stateChanged(int arg1);

private:
    Ui::DlgParamSetting *ui;
    PANEL_PARM          _param;

    int TURN_ON_MIN;       // turn on LED between 17~21 clock
    int TURN_ON_MAX;
    int TURN_OFF_MIN;       // turn off LED between 4~8 clock
    int TURN_OFF_MAX;
    int MAX_TIMEOUT;     // max timeout value 8 second


};

#endif // DLGPARAMSETTING_H
