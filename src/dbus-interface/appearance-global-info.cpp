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
        cout << "Call SetTheme method failed :"
             << " Error: "<< reply.error().message().toStdString() << endl;
        return false;
    }
    else
        return true;
}

QString AppearanceGlobalInfo::getTheme(int themeType)
{
    QDBusPendingReply<QString> reply = m_appearanceInterface->GetTheme(themeType);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        cout << "Call GetTheme method failed : Theme type: " << themeType
             << " Error: "<< reply.error().message().toStdString() << endl;
        //FIXME: return ??
        //return false;
    }
    else if(reply.count() <1)
    {
        cout << "Don't get correct reply!" << endl;
        //FIXME: return ??
        //return false;
    }
    else
    {
        return  reply.argumentAt(0).toString();
    }
}

QString AppearanceGlobalInfo::getDesktopBackground()
{
    return m_appearanceInterface->desktop_background();
}

bool AppearanceGlobalInfo::setDesktopBackground(QString path)
{
    QDBusPendingReply<> reply = m_appearanceInterface->SetDesktopBackground(path);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        cout << "Call set desktop background failed :"
             << " Error: "<< reply.error().message().toStdString() << endl;
        return false;
    }
    else
        return true;
}

QString AppearanceGlobalInfo::getLockScreenBackground()
{
    return m_appearanceInterface->lock_screen_background();
}

bool AppearanceGlobalInfo::setLockScreenBackground(QString path)
{
    QDBusPendingReply<> reply = m_appearanceInterface->SetLockScreenBackground(path);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        cout << "Call set lock screen background failed :"
             << " Error: "<< reply.error().message().toStdString() << endl;
        return false;
    }
    else
        return true;
}

QStringList AppearanceGlobalInfo::getFont(int type)
{
    QString fontInfo;
    QStringList fontInfoList;
    QStringList font ;
    QString fontName;
    QString fontSize;

    QDBusPendingReply<QString> reply = m_appearanceInterface->GetFont(type);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        cout << "Call GetFont method failed : Font type: " << type
             << " Error: "<< reply.error().message().toStdString() << endl;
        //FIXME: return ??
    }

    else if(reply.count() <1)
    {
        cout << "Don't get correct reply!" << endl;
        //FIXME: return ??
    }
    else
    {
        fontInfo = reply.argumentAt(0).toString();
        cout << "Font type is: " << type
             <<" Font info is:" << fontInfo.toStdString() << endl;

        fontInfoList = fontInfo.split(" ",QString::SkipEmptyParts);
        if(!fontInfoList.isEmpty())
        {
            fontSize = fontInfoList.takeLast();
            fontName = fontInfoList.join(" ");
            cout << fontName.toStdString() << endl;
            cout << fontSize.toStdString() << endl;
            font << fontName << fontSize;
        }
    }
    return font;
}

bool AppearanceGlobalInfo::setFont(int fontType, QStringList fontInfoList)
{
    QString fontInfo;
    fontInfo = fontInfoList.join(" ");
    cout << "setFont : fontInfo = " << fontInfo.toStdString() << endl;

    QDBusPendingReply<> reply = m_appearanceInterface->SetFont(fontType,fontInfo);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        cout << "Call GetFont method failed : Font type: " << fontType
             << " Error: "<< reply.error().message().toStdString() << endl;
        return false;
    }
    return true;
}


