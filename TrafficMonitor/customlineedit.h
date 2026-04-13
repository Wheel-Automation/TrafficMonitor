#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H
#include <QLineEdit>


class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CustomLineEdit(const QString & contents = "", QWidget *parent = 0);
    virtual ~CustomLineEdit() {}

signals:
    void jumpForward();
    void jumpBackward();

public slots:
    void jumpIn();

protected:
    virtual void focusInEvent(QFocusEvent *event);
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    bool selectOnMouseRelease;
};

#endif // CUSTOMLINEEDIT_H
