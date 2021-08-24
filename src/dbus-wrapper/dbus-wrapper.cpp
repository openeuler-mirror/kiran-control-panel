#include "dbus-wrapper.h"
#include <kiran-session-daemon/keyboard-i.h>
#include "KSKKeyboardProxy.h"

DbusWrapper::DbusWrapper(QObject *parent)
{
    m_keyboardInterface = QSharedPointer<KSKKeyboardProxy>(new KSKKeyboardProxy(KEYBOARD_DBUS_NAME,
                                                                                KEYBOARD_OBJECT_PATH,
                                                                                QDBusConnection::sessionBus()));
}

DbusWrapper::~DbusWrapper()
{
}

bool DbusWrapper::isValidConnect()
{
    if (!m_keyboardInterface->isValid())
        return false;
    return true;
}

QSharedPointer<KSKKeyboardProxy> DbusWrapper::getKeyboardInterface()
{
    return m_keyboardInterface;
}
