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
    {"exclam", "!"}, {"at", "@"}, {"numbersign", "#"}, {"dollar", "$"}, {"percent", "%"}, {"asciicircum", "^"}, {"ampersand", "&"}, {"asterisk", "*"}, {"parenleft", "("}, {"parenRight", ")"}, {"underscore", "_"}, {"plus", "+"}, {"braceleft", "{"}, {"braceright", "}"}, {"bar", "|"}, {"colon", ":"}, {"quotedbl", "\""}, {"quoteleft", "`"}, {"less", "<"}, {"greater", ">"}, {"question", "?"}, {"minus", "-"}, {"equal", "="}, {"bracketleft", "["}, {"bracketright", "]"}, {"backslash", "\\"}, {"semicolon", ";"}, {"apostrophe", "'"}, {"comma", ","}, {"period", "."}, {"slash", "/"}, {"up", "↑"}, {"left", "←"}, {"down", "↓"}, {"right", "→"}, {"asciitilde", "~"}, {"grave", "`"}, {"control", "Ctrl"}, {"super_l", "Super"}, {"super_r", "Super"}};

static const QMap<QString, QString> MediaKeyMap = {
    {"XF86AudioPlay", QT_TRANSLATE_NOOP("Audio Play", "音频播放")}, {"XF86Search", QT_TRANSLATE_NOOP("Search", "搜索")}, {"XF86WWW", QT_TRANSLATE_NOOP("WWW", "万维网")}, {"XF86AudioLowerVolume", QT_TRANSLATE_NOOP("Audio Lower Volume", "减小音量")}, {"XF86AudioRaiseVolume", QT_TRANSLATE_NOOP("Raise Volume", "增大音量")}, {"XF86AudioMicMute", QT_TRANSLATE_NOOP("Mic Mute", "输入静音")}, {"XF86AudioStop", QT_TRANSLATE_NOOP("Audio Stop", "音频停止")}, {"XF86Explorer", QT_TRANSLATE_NOOP("Explorer", "浏览")}, {"XF86Calculator", QT_TRANSLATE_NOOP("Calculator", "计算器")}, {"XF86AudioMute", QT_TRANSLATE_NOOP("Audio Mute", "静音")}, {"XF86AudioPause", QT_TRANSLATE_NOOP("Audio Pause", "音频暂停")}, {"XF86AudioPrev", QT_TRANSLATE_NOOP("Audio Prev", "音频上一个")}, {"XF86AudioMedia", QT_TRANSLATE_NOOP("Audio Media", "音频媒体")}, {"XF86AudioNext", QT_TRANSLATE_NOOP("Audio Next", "音频下一个")}, {"XF86Mail", QT_TRANSLATE_NOOP("Mail", "邮件")}, {"XF86Tools", QT_TRANSLATE_NOOP("Tools", "工具")}, {"XF86Eject", QT_TRANSLATE_NOOP("Eject", "弹出")}

};
//static const QMap<QString,QString>
#endif  // KEYBINDING_DEF_HSSS
