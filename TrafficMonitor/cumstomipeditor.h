#ifndef CUMSTOM_IP_EDITOR_H
#define CUMSTOM_IP_EDITOR_H
#include "string.h"
#include <vector>
#include "customlineedit.h"

class CustomIpEditor : public QFrame
{
    Q_OBJECT
public:
    explicit CustomIpEditor(QWidget *parent = 0);
    virtual ~CustomIpEditor() {}
    QString     getIP();
    std::string getStdIP();
    bool        isValidIP(); // confirm not empty
    bool        isValidMask(std::vector<int>& black_ips); // confirm not empty
    void        SetText(QString szIP);
    void        setEnable_sub(bool);
    void        setFocus(){lines[0]->setFocus(); }
    bool        isSameAddress(QString otherAdd);
    bool        enableItem(bool bEnable);
    std::vector<int> getSections();

private:

    QList <CustomLineEdit*> lines;

};



#endif // CUMSTOMIPEDITOR_H
