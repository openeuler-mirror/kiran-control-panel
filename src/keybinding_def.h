#ifndef KEYBINDING_DEF_H
#define KEYBINDING_DEF_H

#include <QString>
struct ShortcutInfo
{
    int type;
    QString kind;
    QString uid;
    QString name;
    QString action;
    QString keyCombination;
};

enum ShortcutType
{
    SHORTCUT_TYPE_SYSTEM = 0,
    SHORTCUT_TYPE_CUSTOM
};

#endif  // KEYBINDING_DEF_H
