#include "accounts-global-info.h"
#include "config.h"
#include "kiran-account-manager.h"
#include "tools/log.h"
#include "tools/temporary-dir-manager.h"

#include <kiran-application.h>
#include <QFile>
#include <QDesktopWidget>
#include <zlog_ex.h>

#define DEFAULT_THEME ":/themes/black_theme.qss"

//TODO:修改所有消息框为kiranwidgets-qt5中的消息框

void initLog()
{
    dzlog_init_ex(nullptr,
                  "kylinsec-session",
                  "kiran-account-manager",
                  qAppName().toStdString().c_str());
    Log::instance()->init();
    qInstallMessageHandler(Log::messageHandler);
}

void initTemporaryDiaManager()
{
    if (!TemporaryDirManager::instance()->init(qAppName()))
    {
        LOG_ERROR_S() << "init temporary dir failed";
        exit(-1);
    }
}

void loadStyleSheet()
{
    QFile file(DEFAULT_THEME);
    if (file.open(QIODevice::ReadOnly))
    {
        QString style = file.readAll();
        qApp->setStyleSheet(style);
    }
    else
    {
        LOG_ERROR_S() << "can't load default theme";
        exit(-1);
    }
}

void loadAccountInfo()
{
    if (!AccountsGlobalInfo::instance()->init())
    {
        LOG_ERROR_S() << "load accounts info failed.";
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    KiranApplication app(argc, argv);

    initLog();

    initTemporaryDiaManager();

    loadStyleSheet();

    loadAccountInfo();

    QTranslator tsor;
    tsor.load(QLocale(),
              qAppName() /*filename*/,
              "." /*prefix*/,
              TRANSLATION_DIR_PATH /*dir*/,
              ".qm" /*suffix*/);
    qApp->installTranslator(&tsor);

    KiranAccountManager accountManager;
    accountManager.resize(975, 850);
    int screeNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screeNum);
    accountManager.move(screenGeometry.x() + (screenGeometry.width() - accountManager.width()) / 2,
                screenGeometry.y() + (screenGeometry.height() - accountManager.height()) / 2);
    accountManager.show();
    return QApplication::exec();
}
