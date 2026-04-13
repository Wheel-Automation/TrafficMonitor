#ifndef LED_H
#define LED_H

#include <QWidget>
#include <QColor>
#include <utils.h>


namespace Ui {
class led;
}



class led : public QWidget
{
    Q_OBJECT

public:
    explicit led(QWidget *parent = nullptr);
    ~led();
public :
    void control_state(LET_STATE);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;


private:
    Ui::led *ui;
    QColor m_color[e_state_cnt];


    LET_STATE m_state;

};




#endif // LED_H
