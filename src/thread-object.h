#ifndef THREADOBJECT_H
#define THREADOBJECT_H
#include <QObject>
#include "dbus-wrapper/keybinding-backEnd-proxy.h"
#include "keybinding_def.h"

class ThreadObject : public QObject
{
    Q_OBJECT
public:
    explicit ThreadObject(QObject *parent = nullptr);
    ~ThreadObject();

private:
    int getJsonValueFromString(QString jsonString);
    void convetToValue(QJsonObject obj, QString key);

public slots:
    void loadShortcutInfo();

signals:
    void getShortcutInfo(QList<ShortcutInfo *> shortcutsList);

private:
    QList<ShortcutInfo *> m_shortcutsList;
    KeybindingBackEndProxy *m_keybindingInterface;
};

#endif  // THREADOBJECT_H
