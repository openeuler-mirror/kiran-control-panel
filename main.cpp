#include "kiran-display-configuration-window.h"
#include "kiran-display-config-global.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QTranslator>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning("Cannot connect to the D-Bus session bus.\n"
                 "Please check your system settings and try again.\n");
        return 1;
    }

    QString locale = QLocale::system().name();
    QString qmFile = QString("/usr/share/kiran-display-configuration/kiran-display-configuration.%1.qm").arg(locale);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        a.installTranslator(&translator);

    DisplayModesStu::registerMetaType();

    KiranDisplayConfigurationWindow w;
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    w.move(screenRect.center()-QPoint(w.rect().width()/2, w.rect().height()/2));
    w.show();

    return a.exec();
}
