#include "mainwindow.h"
#include <iostream>
#include <QApplication>
#include <QStandardPaths>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include "macro.h"
#include "utils.h"
#include "dlglaunchpassword.h"
#include "setting/runguard.h"

#ifndef QT_NO_SYSTEMTRAYICON

#define TO_STRING_AUX( x )  #x
#define TO_STRING( x )      TO_STRING_AUX( x )
#define BUILD_VERSION 0.1.0



int main(int argc, char *argv[])
{

    RunGuard guard( "some_random_key" );
    if ( !guard.tryToRun() )
       return 0;

    Q_INIT_RESOURCE(trafficMornitor);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug() << "This system doesn't support tray icon";
        return 0;
    }
    QApplication::setQuitOnLastWindowClosed(false);

   register_datatype();


#if PASSWORD_LOGIN
    DlgLaunchPassword* dlgPass = new DlgLaunchPassword();
    dlgPass->exec();
    if(dlgPass->IsVerified())
#else
    if(1)
#endif
    {
        MainWindow w;
        w.show();
        return a.exec();
    }
    else
    {
        return 0;
    }
}

#endif
