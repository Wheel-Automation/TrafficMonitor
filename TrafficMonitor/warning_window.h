#ifndef WARNING_WINDOW_H
#define WARNING_WINDOW_H

#include <QFrame>

namespace Ui {
class warning_window;
}

class warning_window : public QFrame
{
    Q_OBJECT

public:
    explicit warning_window(QWidget *parent = nullptr);
    ~warning_window();

private:
    Ui::warning_window *ui;
};

#endif // WARNING_WINDOW_H
