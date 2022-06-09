#ifndef KEYBINDING_DEF_H
#define KEYBINDING_DEF_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QSharedPointer>

#define SHORTCUT_KIND_SOUND "Sound"
#define SHORTCUT_KIND_WINDOW_MANAGE "窗口管理"
#define SHORTCUT_KIND_SYSTEM "System"
#define SHORTCUT_KIND_ACCESSIBILITY "Accessibility"
#define SHORTCUT_KIND_DESKTOP "桌面"
#define SHORTCUT_KIND_CUSTOM "自定义"

struct ShortcutInfo
{
    int type;
    QString kind;
    QString uid;
    QString name;
    QString action;
    QString keyCombination;
};
typedef QSharedPointer<ShortcutInfo> ShortcutInfoPtr;

static QStringList ignoreKeys = {
    "Tab",
    "Return",
    "Enter",
    "Space",
    "Esc",
    "Home",
    "End",
    "PgUp",
    "PgDown",
    "Up",
    "Down",
    "Left",
    "Right",
    "Shift",
    "Ctrl",
    "Alt"};

enum ShortcutType
{
    SHORTCUT_TYPE_SYSTEM = 0,
    SHORTCUT_TYPE_CUSTOM
};

//static const QMap<QString,QString>
#endif  // KEYBINDING_DEF_HSSS
