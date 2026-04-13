#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QIntValidator>
#include <QPushButton>
#include <string.h>
#include "cumstomipeditor.h"

//=============================================================================
void makeCommonStyle(QLineEdit* line) {
    line->setContentsMargins(0, 0, 0, 0);
    line->setAlignment(Qt::AlignCenter);
    line->setStyleSheet("QLineEdit { border: 0px none; color:white; }");
    line->setFrame(false);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

QLineEdit* makeIpSpliter() {
    QLineEdit *spliter = new QLineEdit(".");
    makeCommonStyle(spliter);

    //spliter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spliter->setMaximumWidth(10);
    spliter->setReadOnly(true);
    spliter->setFocusPolicy(Qt::NoFocus);
    return spliter;
}

CustomIpEditor::CustomIpEditor(QWidget *parent)
    //:QFrame(parent)
{
    parent->setContentsMargins(1, 0, 1, 0);
    //setStyleSheet("QFrame { background-color: white;  border: 1px solid white; border-radius: 15px; }");
    // parent->setStyleSheet("QFrame { background-color: yellow;  border: 1px solid white; }");
    parent->setStyleSheet("QFrame { border: 1px solid #505050;}");


    QList <CustomLineEdit *>::iterator linesIterator;

    lines.append(new CustomLineEdit);
    lines.append(new CustomLineEdit);
    lines.append(new CustomLineEdit);
    lines.append(new CustomLineEdit);

    QHBoxLayout *mainLayout = new QHBoxLayout(parent);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(1);

    for (linesIterator = lines.begin(); linesIterator != lines.end(); ++linesIterator) {
        makeCommonStyle(*linesIterator);
        mainLayout->addWidget(*linesIterator);

        if (*linesIterator != lines.last()) {
            connect(*linesIterator, &CustomLineEdit::jumpForward,
                    *(linesIterator+1), &CustomLineEdit::jumpIn);
            mainLayout->addWidget(makeIpSpliter());
        }
        if (*linesIterator != lines.first()) {
            connect(*linesIterator, &CustomLineEdit::jumpBackward,
                    *(linesIterator-1), &CustomLineEdit::jumpIn);
        }
    }
}

// get ip address
QString CustomIpEditor::getIP(){
    QList <CustomLineEdit *>::iterator iter;
    QString ip;
    for (int i = 0; i < lines.size(); i ++) {
        ip += lines[i]->text();
        if(i != lines.size() - 1)
            ip += ".";

    }
    return ip;
}
std::string CustomIpEditor::getStdIP(){

    QString ip = getIP();
    std::string std_ip = ip.toStdString();
    return std_ip;
}


void CustomIpEditor::SetText(QString szIP){
    QStringList ips = szIP.split(".");
    if(ips.count() != 4)
        return;
    for (int i = 0; i < 4; i ++) {
        lines[i]->setText(ips[i]);

    }

}

void CustomIpEditor::setEnable_sub(bool bEnable){
    for (int i = 0; i < 4; i ++) {
        lines[i]->setEnabled(bEnable);
    }
}

bool CustomIpEditor::isValidIP(){

    for (int i = 0; i < 4; i ++) {
        QString msg = lines[i]->text();
        if(msg.length() == 0)
            return false;
        bool valid= false;
        int nValue = msg.toInt(&valid);
        if(!valid || nValue > 255 || nValue < 0) // invalid value in ip addres
            return false;
    }
    return true;
}


std::vector<int> CustomIpEditor::getSections(){
    std::vector <int> sections ;
    for (int i = 0; i < 4; i ++) {
        QString msg = lines[i]->text();
        if(msg.length() == 0)
            return sections;
        bool valid= false;
        int nValue = msg.toInt(&valid);
        if(!valid || nValue > 255 || nValue < 0) // invalid value in ip addres
            return sections;
        sections.push_back(nValue);
    }
    return sections;
}


bool       CustomIpEditor::isValidMask(std::vector<int>& black_ips){
   std::vector<int> masks;
   black_ips.clear();
    for (int i = 0; i < 4; i ++) {
        QString msg = lines[i]->text();
        if(msg.length() == 0)
            break;
        bool valid= false;
        int nValue = msg.toInt(&valid);
        masks.push_back(nValue);
    }
    if(masks.size() != 4)
        return false;
    if(masks[0] != 255) // first should be 255
        return false;
    if(masks[3] == 255)
        return false;

    // check network position
    for(int i = 3; i >= 1; i --){
        if((masks[i] > masks[i - 1]))
            return false;
    }

    int last_position = 3;
    for(int i = 3; i >= 1; i --){
        if(masks[i] > 0){
            if(masks[i] == 255)
                last_position = i + 1;
            else
                last_position = i;
            break;
        }
    }
    std::vector<int> valid_suffix = {0,128, 192, 224, 240, 248};
    if(std::find(valid_suffix.begin(), valid_suffix.end(), masks[last_position]) == valid_suffix.end()){
        qDebug() << "Invalid mask";
        return false;
    }

    //if(last_position == 3)
    {
        int step = 0;
        int v = masks[last_position];
        step = 0;
        switch(v){
        case 0:
            step = 256;
            break;
        case 128:
            step = 128;
            break;
        case 192:
            step = 64;
            break;
        case 224:
            step = 32;
            break;
        case 240:
            step = 16;
            break;
        case 248:
            step = 8;
            break;
        default:
            break;
        }
        if(step == 0){
            return false;
        }
        int cnt = 0;
        while(true){
            int black_ip = cnt* step - 1;
            if(black_ip <= 255 && black_ip >= 0)
                black_ips.push_back(black_ip);

            black_ip ++;
            if(black_ip <= 255 && black_ip >= 0)
                black_ips.push_back(black_ip);
            if(black_ip > 255)
                break;
            cnt ++;
        }
    }
    return true;
}

bool   CustomIpEditor::enableItem(bool bEnable){
    for (int i = 0;i < 4; i ++) {
        lines[i]->setEnabled(bEnable);
    }
    return true;
}


// check the same address
bool   CustomIpEditor::isSameAddress(QString otherAdd){
    QStringList sublist = otherAdd.split(".");
    if (sublist.length() != 4)
        return false;
    bool bOther = false;
    for (int i = 0; i < 4; i ++) {
        if(lines[i]->text()!= sublist[i]){
            bOther = true;
            break;
        }
    }
    if(bOther) return false;
    return true;
}
