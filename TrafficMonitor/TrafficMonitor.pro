QT       += core gui
QT       += network
QT       += serialport
RC_ICONS = asset/demo.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
#CONFIG += console

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += WINDOWS_IGNORE_PACKING_MISMATCH

INCLUDEPATH+="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt"
INCLUDEPATH+="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um"
INCLUDEPATH+="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared"
INCLUDEPATH+="C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\winrt"

#LIBS+=-L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/ucrt/x64"
#LIBS+=-L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64"

SOURCES += \
    cameraview.cpp \
    camviewerworker.cpp \
    cgi/cgiclient.cpp \
    com/com.cpp \
    com/proxythread.cpp \
    connectworker.cpp \
    cumstomipeditor.cpp \
    customlineedit.cpp \
    device_calibrator.cpp \
    device_serach_worker.cpp \
    devicelogwnd.cpp \
    dlg_camera_setting.cpp \
    dlg_login.cpp \
    dlg_update_pwd.cpp \
    dlg_warning.cpp \
    dlgipsearch.cpp \
    dlglaunchpassword.cpp \
    dlgparamsetting.cpp \
    dlgproxysetting.cpp \
    ipsearch.cpp \
    led.cpp \
    longconnectserver.cpp \
    main.cpp \
    mainwindow.cpp \
    modifyip.cpp \
    mytablewidget.cpp \
    proxyworker.cpp \
    setting/logsettingdlg.cpp \
    setting/logviewer.cpp \
    setting/logwriterthread.cpp \
    setting/runguard.cpp \
    setting/settingmanager.cpp \
    setting/settingutils.cpp \
    state_panel.cpp \
    test.cpp \
    utils.cpp

HEADERS += \
    camViewerWorker.h \
    cameraview.h \
    cgi/cgiclient.h \
    com/com.h \
    com/proxythread.h \
    connectworker.h \
    cumstomipeditor.h \
    customlineedit.h \
    device_calibrator.h \
    device_serach_worker.h \
    devicelogwnd.h \
    dlg_camera_setting.h \
    dlg_login.h \
    dlg_update_pwd.h \
    dlg_warning.h \
    dlgipsearch.h \
    dlglaunchpassword.h \
    dlgparamsetting.h \
    dlgproxysetting.h \
    ipsearch.h \
    led.h \
    longconnectserver.h \
    macro.h \
    mainwindow.h \
    modifyip.h \
    mytablewidget.h \
    proxyworker.h \
    setting/logsettingdlg.h \
    setting/logviewer.h \
    setting/logwriterthread.h \
    setting/runguard.h \
    setting/settingmanager.h \
    setting/settingutils.h \
    state_panel.h \
    test.h \
    utils.h


INCLUDEPATH +=$$PWD/../include/
INCLUDEPATH +=$$PWD/../EncryptLib/
INCLUDEPATH +=$$PWD/../include/network/rv1126
INCLUDEPATH +=$$PWD/../include/network/searchDevice
#DEPENDPATH += $$PWD/../EncryptLib/


LIB_OPENCV = $$PWD/../lib/x86

win32:CONFIG(release, debug|release): LIBS += -L$$LIB_OPENCV/ -lopencv_core440 -lopencv_features2d440 \
    -lopencv_highgui440 -lopencv_imgcodecs440 -lopencv_imgproc440 -lopencv_objdetect440 -lopencv_video440 -lopencv_videoio440  -lNetClient -lDevSearchSDK
else:win32:CONFIG(debug, debug|release): LIBS += -L$$LIB_OPENCV/ -lopencv_core440d -lopencv_features2d440d \
    -lopencv_highgui440d -lopencv_imgcodecs440d -lopencv_imgproc440d -lopencv_objdetect440d -lopencv_video440d -lopencv_videoio440d -lNetClient -lDevSearchSDK


#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../EncryptLib/release -lEncryptLib
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../EncryptLib/debug -lEncryptLib


FORMS += \
    dialog.ui \
    dlg_camera_setting.ui \
    dlg_login.ui \
    dlg_update_pwd.ui \
    dlg_warning.ui \
    dlgipsearch.ui \
    dlglaunchpassword.ui \
    dlgparamsetting.ui \
    dlgproxysetting.ui \
    led.ui \
    mainwindow.ui \
    modifyip.ui \
    setting/logsettingdlg.ui \
    setting/logviewer.ui \
    state_panel.ui \
    test.ui

TRANSLATIONS += \
    TrafficMonitor_zh_CN.ts

CODECFORSRC     = UTF-8

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    trafficMornitor.qrc
