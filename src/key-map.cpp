#include "key-map.h"

KeyMap::KeyMap(QObject *parent) : QObject(parent)
{
    m_keyEnum = QMetaEnum::fromType<KeyMap::Keys>();
}

QString KeyMap::keycodeToString(int keycode)
{
    return m_keyEnum.valueToKey(keycode);
}
