#ifndef KEYBINDING_DEF_H
#define KEYBINDING_DEF_H

#include <QMap>
#include <QString>
#include <QStringList>

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

static const QMap<QString, QString> SpecialKeyMap = {
    {"exclam", "!"}, {"at", "@"}, {"numbersign", "#"}, {"dollar", "$"}, {"percent", "%"}, {"asciicircum", "^"}, {"ampersand", "&"}, {"asterisk", "*"}, {"parenleft", "("}, {"parenRight", ")"}, {"underscore", "_"}, {"plus", "+"}, {"braceleft", "{"}, {"braceright", "}"}, {"bar", "|"}, {"colon", ":"}, {"quotedbl", "\""}, {"quoteleft", "`"}, {"less", "<"}, {"greater", ">"}, {"question", "?"}, {"minus", "-"}, {"equal", "="}, {"bracketleft", "["}, {"brecketright", "]"}, {"backslash", "\\"}, {"semicolon", ";"}, {"apostrophe", "'"}, {"comma", ","}, {"period", "."}, {"slash", "/"}, {"up", "↑"}, {"left", "←"}, {"down", "↓"}, {"right", "→"}, {"asciitilde", "~"}, {"grave", "`"}, {"control", "Ctrl"}, {"super_l", "Super"}, {"super_r", "Super"}};

static const QMap<QString, QString> MediaKeyMap = {
    {"XF86AudioPlay", "Audio Play"}, {"XF86Search", "Search"}, {"XF86WWW", "WWW"}, {"XF86AudioLowerVolume", "Lower Volume"}, {"XF86AudioRaiseVolume", "Raise Volume"}, {"XF86AudioMicMute", "Input Mute"}, {"XF86AudioStop", "Audio Stop"}, {"XF86Explorer", "Explorer"}, {"XF86Calculator", "Calculator"}, {"XF86AudioMute", "Output Mute"}, {"XF86AudioPause", "Audio Pause"}, {"XF86AudioPrev", "Audio Prev"}, {"XF86AudioMedia", "Audio Media"}, {"XF86AudioNext", "Audio Next"}, {"XF86Mail", "Mail"}, {"XF86Tools", "Tools"}, {"XF86Eject", "Eject"}

};
#endif  // KEYBINDING_DEF_HSSS
