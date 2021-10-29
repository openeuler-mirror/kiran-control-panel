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

// clang-format off
static const QMap<QString, QString> SpecialKeyMap = {
    {"exclam", "!"}, {"at", "@"}, {"numbersign", "#"}, {"dollar", "$"},
    {"percent", "%"}, {"asciicircum", "^"}, {"ampersand", "&"},
    {"asterisk", "*"}, {"parenleft", "("}, {"parenRight", ")"},
    {"underscore", "_"}, {"plus", "+"}, {"braceleft", "{"}, {"braceright", "}"},
    {"bar", "|"}, {"colon", ":"}, {"quotedbl", "\""}, {"quoteleft", "`"}, {"less", "<"},
    {"greater", ">"}, {"question", "?"}, {"minus", "-"}, {"equal", "="},
    {"bracketleft", "["}, {"bracketright", "]"}, {"backslash", "\\"},
    {"semicolon", ";"}, {"apostrophe", "'"}, {"comma", ","}, {"period", "."},
    {"slash", "/"}, {"up", "↑"}, {"left", "←"}, {"down", "↓"}, {"right", "→"},
    {"asciitilde", "~"}, {"grave", "`"}, {"control", "Ctrl"}, {"super_l", "Super"},
    {"super_r", "Super"}};


static const QMap<QString, QString> MediaKeyMap = {
    {"XF86AudioPlay", QT_TRANSLATE_NOOP("Media Key", "Audio Play")},
    {"XF86Search", QT_TRANSLATE_NOOP("Media Key", "Search")},
    {"XF86WWW", QT_TRANSLATE_NOOP("Media Key", "WWW")},
    {"XF86AudioLowerVolume", QT_TRANSLATE_NOOP("Media Key", "Audio Lower Volume")},
    {"XF86AudioRaiseVolume", QT_TRANSLATE_NOOP("Media Key", "Audio Raise Volume")},
    {"XF86AudioMicMute", QT_TRANSLATE_NOOP("Media Key", "Mic Mute")},
    {"XF86AudioStop", QT_TRANSLATE_NOOP("Media Key", "Audio Stop")},
    {"XF86Explorer", QT_TRANSLATE_NOOP("Media Key", "Explorer")},
    {"XF86Calculator", QT_TRANSLATE_NOOP("Media Key", "Calculator")},
    {"XF86AudioMute", QT_TRANSLATE_NOOP("Media Key", "Audio Mute")},
    {"XF86AudioPause", QT_TRANSLATE_NOOP("Media Key", "Audio Pause")},
    {"XF86AudioPrev", QT_TRANSLATE_NOOP("Media Key", "Audio Prev")},
    {"XF86AudioMedia", QT_TRANSLATE_NOOP("Media Key", "Audio Media")},
    {"XF86AudioNext", QT_TRANSLATE_NOOP("Media Key", "Audio Next")},
    {"XF86Mail", QT_TRANSLATE_NOOP("Media Key", "Mail")},
    {"XF86Tools", QT_TRANSLATE_NOOP("Media Key", "Tools")},
    {"XF86Eject", QT_TRANSLATE_NOOP("Media Key", "Eject")}

};
// clang-format on

//static const QMap<QString,QString>
#endif  // KEYBINDING_DEF_HSSS
