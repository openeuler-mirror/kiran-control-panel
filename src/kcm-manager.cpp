#include "kcm-manager.h"
#include "KSMMouseProxy.h"
#include "KSMTouchPadProxy.h"
#include <kiran-session-daemon/mouse-i.h>
#include <kiran-session-daemon/touchpad-i.h>
#include <QDBusConnection>
KCMManager::KCMManager(QObject *parent):
    QObject(parent)
{
    m_mouseInterface = QSharedPointer<KSMMouseProxy>(new KSMMouseProxy(MOUSE_DBUS_NAME,
                                                                       MOUSE_OBJECT_PATH,
                                                                       QDBusConnection::sessionBus()));

    m_touchPadInterface =QSharedPointer<KSMTouchPadProxy>(new KSMTouchPadProxy(TOUCHPAD_DBUS_NAME,
                                                                               TOUCHPAD_OBJECT_PATH,
                                                                               QDBusConnection::sessionBus()));
}

KCMManager::~KCMManager()
{

}

bool KCMManager::isValidConnect()
{
    if(!m_mouseInterface->isValid() || !m_touchPadInterface->isValid())
        return false;
    return true;
}

QSharedPointer<KSMMouseProxy> KCMManager::getMouseInterface()
{
    return m_mouseInterface;
}

QSharedPointer<KSMTouchPadProxy> KCMManager::getTouchPadInterface()
{
    return m_touchPadInterface;
}

bool KCMManager::hasTouchPad()
{
    if(m_touchPadInterface->has_touchpad())
    {
        return true;
    }
    return false;
}
