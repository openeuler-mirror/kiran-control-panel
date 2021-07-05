#include "appearance-global-info.h"
#include "Appearance.h"
#include "kiran-session-daemon/appearance-i.h"

#include <iostream>
#include <QMutex>
#include <QScopedPointer>

using namespace std;
AppearanceGlobalInfo::AppearanceGlobalInfo(QObject *parent)
    : QObject(parent),
    m_appearanceInterface(nullptr)
{
    m_appearanceInterface = ComKylinsecKiranSessionDaemonAppearanceInterface::instance();
}

AppearanceGlobalInfo::~AppearanceGlobalInfo()
{

}

AppearanceGlobalInfo *AppearanceGlobalInfo::instance()
{
    static QMutex mutex;
    static QScopedPointer<AppearanceGlobalInfo> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new AppearanceGlobalInfo);
        }
    }

    return pInst.data();
}

bool AppearanceGlobalInfo::getAllThemes(int themeType, QString &themesJson)
{
    QDBusPendingReply<QString> reply = m_appearanceInterface->GetThemes(themeType);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        cout << "Call GetThemes method failed : Theme type: " << themeType
             << " Error: "<< reply.error().message().toStdString() << endl;
        return false;
    }
    else if(reply.count() <1)
    {
        cout << "Don't get correct reply!" << endl;
        return false;
    }
    else
    {
        themesJson = reply.argumentAt(0).toString();
        cout << "theme type is : "<< themeType
             <<" theme is :" << themesJson.toStdString() << endl;
        return true;
    }
}

bool AppearanceGlobalInfo::setTheme(int themeType, QString themeName)
{
    QDBusPendingReply<> reply = m_appearanceInterface->SetTheme(themeType,themeName);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        cout << "Call GetThemes method failed :"
             << " Error: "<< reply.error().message().toStdString() << endl;
        return false;
    }
    else
        return true;
}
