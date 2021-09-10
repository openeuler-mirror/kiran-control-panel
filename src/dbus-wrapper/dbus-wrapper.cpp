#include "dbus-wrapper.h"
#include <kiran-session-daemon/keyboard-i.h>
#include "KeyboardBackEndProxy.h"
//#include <kiran-session-daemon/>
DbusWrapper::DbusWrapper(QObject *parent)
{
    m_keyboardInterface = QSharedPointer<KeyboardBackEndProxy>(new KeyboardBackEndProxy(KEYBOARD_DBUS_NAME,
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

QSharedPointer<KeyboardBackEndProxy> DbusWrapper::getKeyboardInterface()
{
    return m_keyboardInterface;
}
