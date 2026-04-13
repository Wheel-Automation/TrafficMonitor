QT -= gui

TEMPLATE = lib
DEFINES += ENCRYPTLIB_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH+="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt"
INCLUDEPATH+="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um"
INCLUDEPATH+="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared"
INCLUDEPATH+="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\winrt"

LIBS+=-L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/ucrt/x64"
LIBS+=-L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64"

SOURCES += \
    encryptlib.cpp

HEADERS += \
    EncryptLib_global.h \
    encryptlib.h

INCLUDEPATH +=$$PWD/../include/
INCLUDEPATH +=$$PWD/../lib/
DEPENDPATH += $$PWD/../lib/


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lcdll8
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lcdll8

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

