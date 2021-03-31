#include "kiran-display-configuration-window.h"
#include "kiran-display-config-global.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include "zlog_ex.h"
#include <kiran-application.h>
#include <QDesktopWidget>
#include <QTranslator>
#include <QDBusInterface>
#include <QPushButton>
#include <QDialogButtonBox>

int main(int argc, char *argv[])
{
    KiranApplication a(argc, argv);
    //报警中也需要翻译，所以加载翻译文件需要前置。
    QString locale = QLocale::system().name();
    QString qmFile = QString("%1.%2.qm").arg(APP_TRANSLATE_PATH_PREFIX).arg(locale);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        dzlog_cerr("load qm: [%s] error.", qmFile);
    else
        a.installTranslator(&translator);

    if (!QDBusConnection::sessionBus().isConnected()) {
        dzlog_warn("Cannot connect to the D-Bus session bus.\n"
                 "Please check your system settings and try again.\n");
        return 1;
    }
    if(!QDBusInterface(KIRAN_DBUS_SERVICE_NAME, KIRAN_DBUS_DISPLAY).isValid())
    {
        KiranMessageBox box;
        box.setTitle(QObject::tr("Tips"));

        QPushButton btn;
        btn.setText(QObject::tr("OK(K)"));
        btn.setFixedSize(QSize(200, box.buttonSize().height()));
        btn.setShortcut(Qt::CTRL + Qt::Key_K);
        box.addButton(&btn, QDialogButtonBox::AcceptRole);
        box.setText(QObject::tr("D-Bus service failed to connect, display setting failed to start, please check if D-Bus service is start."));
        box.exec();
        return 1;
    }

    DisplayModesStu::registerMetaType();

    KiranDisplayConfigurationWindow w;
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    w.move(screenRect.center()-QPoint(w.rect().width()/2, w.rect().height()/2));
    w.show();

    return a.exec();
}
