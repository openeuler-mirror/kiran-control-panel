#include "thread-object.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/keybinding-i.h>
#include <QJsonDocument>
#include <QJsonParseError>
ThreadObject::ThreadObject(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QList<ShortcutInfo *>>("QList<ShortcutInfo *>");
    m_keybindingInterface = new KeybindingBackEndProxy(KEYBINDING_DBUS_NAME,
                                                       KEYBINDING_OBJECT_PATH,
                                                       QDBusConnection::sessionBus());
}

ThreadObject::~ThreadObject()
{
}

int ThreadObject::getJsonValueFromString(QString jsonString)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        KLOG_DEBUG() << " please check the string " << jsonString.toLocal8Bit().data();
        return -1;
    }
    if (jsonDocument.isObject())
    {
        QJsonObject obj = jsonDocument.object();
        convetToValue(obj, KEYBINDING_SHORTCUT_JK_SYSTEM);
        convetToValue(obj, KEYBINDING_SHORTCUT_JK_CUSTOM);
    }
    return m_shortcutsList.size();
}

void ThreadObject::convetToValue(QJsonObject obj, QString key)
{
    if (obj.contains(key))
    {
        QJsonValue value = obj.value(key);
        if (value.type() == QJsonValue::Array)
        {
            QJsonArray array = value.toArray();
            int num = array.size();
            for (int i = 0; i < num; i++)
            {
                ShortcutInfo *shortcutInfo = new ShortcutInfo;
                QJsonValue value = array.at(i);
                if (value.type() == QJsonValue::Object)
                {
                    QJsonObject valueObj = value.toObject();
                    if (valueObj.contains(KEYBINDING_SHORTCUT_JK_UID))
                    {
                        QJsonValue val = valueObj.value(KEYBINDING_SHORTCUT_JK_UID);
                        if (val.isString())
                        {
                            QString uid = val.toString();
                            shortcutInfo->uid = uid;
                        }
                    }
                    if (valueObj.contains(KEYBINDING_SHORTCUT_JK_KIND))
                    {
                        QJsonValue val = valueObj.value(KEYBINDING_SHORTCUT_JK_KIND);
                        if (val.isString())
                        {
                            QString kind = val.toString();
                            shortcutInfo->kind = kind;
                        }
                    }
                    if (valueObj.contains(KEYBINDING_SHORTCUT_JK_KEY_COMBINATION))
                    {
                        QJsonValue val = valueObj.value(KEYBINDING_SHORTCUT_JK_KEY_COMBINATION);
                        if (val.isString())
                        {
                            QString keyCombination = val.toString();
                            shortcutInfo->keyCombination = keyCombination;
                        }
                    }
                    if (valueObj.contains(KEYBINDING_SHORTCUT_JK_NAME))
                    {
                        QJsonValue val = valueObj.value(KEYBINDING_SHORTCUT_JK_NAME);
                        if (val.isString())
                        {
                            QString name = val.toString();
                            shortcutInfo->name = name;
                        }
                    }
                    if (valueObj.contains(KEYBINDING_SHORTCUT_JK_ACTION))
                    {
                        QJsonValue val = valueObj.value(KEYBINDING_SHORTCUT_JK_ACTION);
                        if (val.isString())
                        {
                            QString action = val.toString();
                            shortcutInfo->action = action;
                        }
                    }
                    if (key == KEYBINDING_SHORTCUT_JK_SYSTEM)
                        shortcutInfo->type = SHORTCUT_TYPE_SYSTEM;
                    else
                        shortcutInfo->type = SHORTCUT_TYPE_CUSTOM;

                    m_shortcutsList.append(shortcutInfo);
                    KLOG_INFO() << shortcutInfo->keyCombination;
                }
            }
        }
    }
}

void ThreadObject::loadShortcutInfo()
{
    QDBusPendingReply<QString> reply = m_keybindingInterface->ListShortcuts();
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call ListShortcuts method failed "
                     << " Error: " << reply.error().message();
        return;
    }
    else
    {
        QString json = reply.argumentAt(0).toString();
        if (0 < getJsonValueFromString(json))
        {
            emit getShortcutInfo(m_shortcutsList);
        }
    }
}
