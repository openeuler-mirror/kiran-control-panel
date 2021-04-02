#include "accounts-global-info.h"
#include "config.h"
#include "kiran-account-manager.h"
#include "tools/log.h"
#include "tools/temporary-dir-manager.h"

#include <kiran-application.h>
#include <QFile>

#define LOG_PATH "/tmp/kiran-account-manager.log"
#define DEFAULT_THEME ":/themes/black_theme.qss"
#define TRANSLATION_FILE_DIR "/usr/share/kiran-account-manager/translations/"

//TODO:修改所有消息框为kiranwidgets-qt5中的消息框

void initLog()
{
    if (!Log::instance()->init(LOG_PATH))
    {
        qCritical() << QString("Log init failed(%1)").arg(LOG_PATH);
    }
    qInstallMessageHandler(Log::messageHandler);
}

void initTemporaryDiaManager()
{
    if (!TemporaryDirManager::instance()->init(qAppName()))
    {
        qCritical() << "init temporary dir failed";
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
        qCritical() << "can't load default theme";
        exit(-1);
    }
}

void loadAccountInfo()
{
    if (!AccountsGlobalInfo::instance()->init())
    {
        qCritical() << "load accounts info failed.";
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
    accountManager.show();
    return QApplication::exec();
}
