#include <kiran-message-box.h>
#include <kiran-single-application.h>
#include <QDebug>
#include <QDesktopWidget>
#include <QTranslator>
#include <zlog_ex.h>

#include "config.h"
#include "kiran-power-manager-window.h"
#include "power.h"
#include "log.h"

bool loadTranslator()
{
    QTranslator tsor;
    tsor.load(QLocale(),
              qAppName() /*filename*/,
              "." /*prefix*/,
              KIRAN_POWER_MANAGER_TRANSLATIONS_DIR /*dir*/,
              ".qm" /*suffix*/);
    return QApplication::installTranslator(&tsor);
}

int main(int argc, char* argv[])
{
    KiranSingleApplication app(argc, argv);

    dzlog_init_ex(NULL,
                  "kylinsec-session",
                  "kiran-power-manager",
                  qAppName().toStdString().c_str());
    Log::instance()->init();
    qInstallMessageHandler(Log::messageHandler);

    if (!loadTranslator())
    {
        LOG_WARNING_S() << "load translator failed!";
    }

    if (!PowerInterface::instance()->isValid())
    {
        LOG_WARNING_S() << "can't connect kiran-session-daemon power plugin!";
        KiranMessageBox::message(nullptr, QObject::tr("Error"), QObject::tr("failed to connect to back end of control center!"), KiranMessageBox::Ok);
        return EXIT_FAILURE;
    }

    KiranPowerManagerWindow window;
    int screeNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screeNum);
    window.move(screenGeometry.x() + (screenGeometry.width() - window.width()) / 2,
                screenGeometry.y() + (screenGeometry.height() - window.height()) / 2);
    window.show();

    return QApplication::exec();
}
