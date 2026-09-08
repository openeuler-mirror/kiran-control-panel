/**
 * Copyright (c) 2020 ~ 2026 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "fcitx4/fcitx4-key-formatters.h"

#include <QChar>
#include <QStringList>
#include <QtGlobal>

namespace
{
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
const auto kSkipEmptyParts = QString::SkipEmptyParts;
#else
const auto kSkipEmptyParts = Qt::SkipEmptyParts;
#endif

/// Qt 修饰键 → KeyboardModifier 位
Qt::KeyboardModifier modifierForQtKey(int qtKey)
{
    switch (qtKey)
    {
    case Qt::Key_Control: return Qt::ControlModifier;
    case Qt::Key_Alt:
    case Qt::Key_AltGr: return Qt::AltModifier;
    case Qt::Key_Shift: return Qt::ShiftModifier;
    case Qt::Key_Meta:
    case Qt::Key_Super_L:
    case Qt::Key_Super_R: return Qt::MetaModifier;
    default: return Qt::NoModifier;
    }
}

struct Fcitx4KeyNameEntry
{
    Qt::Key key;
    const char* name;
};

const Fcitx4KeyNameEntry kFcitx4KeyNameTable[] = {
    { Qt::Key_Tab, "TAB" }, { Qt::Key_Return, "ENTER" }, { Qt::Key_Enter, "ENTER" },
    { Qt::Key_Escape, "ESCAPE" }, { Qt::Key_Space, "SPACE" },
    { Qt::Key_Backspace, "BACKSPACE" }, { Qt::Key_Delete, "DELETE" },
    { Qt::Key_Insert, "INSERT" }, { Qt::Key_Home, "HOME" }, { Qt::Key_End, "END" },
    { Qt::Key_PageUp, "PGUP" }, { Qt::Key_PageDown, "PGDN" }, { Qt::Key_Up, "UP" },
    { Qt::Key_Down, "DOWN" }, { Qt::Key_Left, "LEFT" }, { Qt::Key_Right, "RIGHT" },
};

/// 左右修饰键 keysym 名（泛名 SHIFT/CTRL 非法，须用 L/R）
struct Fcitx4ModifierNameEntry {
    int qtKey;
    const char* left;
    const char* right;
};
static const Fcitx4ModifierNameEntry kFcitx4ModifierNameTable[] = {
    { Qt::Key_Shift,   "LSHIFT",  "RSHIFT"   },
    { Qt::Key_Control, "LCTRL",   "RCTRL"    },
    { Qt::Key_Alt,     "LALT",    "RALT"     },
    { Qt::Key_Meta,    "SUPER_L", "SUPER_R"  },
};

QString fcitx4ModifierName(int qtKey, KeyEntry::Side side)
{
    for (const auto& e : kFcitx4ModifierNameTable)
        if (e.qtKey == qtKey)
            return side == KeyEntry::SideRight
                ? QString::fromLatin1(e.right)
                : QString::fromLatin1(e.left);
    return QString();
}

QString fcitx4NameForKey(int qtKey)
{
    const int count = int(sizeof(kFcitx4KeyNameTable) / sizeof(kFcitx4KeyNameTable[0]));
    for (int i = 0; i < count; ++i)
        if (kFcitx4KeyNameTable[i].key == qtKey)
            return QString::fromLatin1(kFcitx4KeyNameTable[i].name);
    if (qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F35)
        return QStringLiteral("F%1").arg(qtKey - Qt::Key_F1 + 1);
    if (qtKey >= 0x20 && qtKey < 0x7f)
        return QString(QChar(qtKey)).toUpper();
    return QString();
}

bool keyForFcitx4Name(const QString& name, KeyEntry* key)
{
    const int count = int(sizeof(kFcitx4KeyNameTable) / sizeof(kFcitx4KeyNameTable[0]));
    for (int i = 0; i < count; ++i)
    {
        if (name == QLatin1String(kFcitx4KeyNameTable[i].name))
        {
            key->qtKey = kFcitx4KeyNameTable[i].key;
            return true;
        }
    }
    if (name.size() > 1 && name.at(0) == QLatin1Char('F'))
    {
        bool ok = false;
        const int number = name.mid(1).toInt(&ok);
        if (ok && number >= 1 && number <= 35)
        {
            key->qtKey = Qt::Key_F1 + number - 1;
            return true;
        }
    }
    if (name.size() == 1 && name.at(0).isPrint() && name.at(0).unicode() < 0x80)
    {
        key->qtKey = name.at(0).unicode();
        return true;
    }
    return false;
}
}  // namespace

/// 单热键：修饰前缀（CTRL_/ALT_/…）+ 主键；主键为修饰键时用 L/R 名
bool Fcitx4KeyFormatter::encode(const KeyEntry& entry, QString* stored) const
{
    if (!stored)
        return false;

    // 剔除主键自身修饰位，避免 SHIFT_LSHIFT
    Qt::KeyboardModifiers modifiers = entry.modifiers & ~modifierForQtKey(entry.qtKey);

    const QString mainName =
        entry.isModifier() ? fcitx4ModifierName(entry.qtKey, entry.side)
                           : fcitx4NameForKey(entry.qtKey);
    if (mainName.isEmpty())
        return false;

    QString result;
    if (modifiers & Qt::ControlModifier)
        result += QStringLiteral("CTRL_");
    if (modifiers & Qt::AltModifier)
        result += QStringLiteral("ALT_");
    if (modifiers & Qt::ShiftModifier)
        result += QStringLiteral("SHIFT_");
    if (modifiers & Qt::MetaModifier)
        result += QStringLiteral("SUPER_");
    *stored = result + mainName;
    return true;
}

bool Fcitx4KeyFormatter::decode(const QString& stored, KeyEntry* key) const
{
    if (!key)
        return false;
    *key = KeyEntry();
    if (stored.trimmed().isEmpty())
        return true;

    QString rest = stored.trimmed().toUpper();
    Qt::KeyboardModifiers modifiers = Qt::NoModifier;
    while (true)
    {
        if (rest.startsWith(QLatin1String("CTRL_"))) { modifiers |= Qt::ControlModifier; rest.remove(0, 5); }
        else if (rest.startsWith(QLatin1String("ALT_"))) { modifiers |= Qt::AltModifier; rest.remove(0, 4); }
        else if (rest.startsWith(QLatin1String("SHIFT_"))) { modifiers |= Qt::ShiftModifier; rest.remove(0, 6); }
        else if (rest.startsWith(QLatin1String("SUPER_"))) { modifiers |= Qt::MetaModifier; rest.remove(0, 6); }
        else break;
    }

    bool matchedModifier = false;
    for (const auto& e : kFcitx4ModifierNameTable) {
        if (rest == QLatin1String(e.left)) {
            key->qtKey = e.qtKey; key->side = KeyEntry::SideLeft; matchedModifier = true; break;
        }
        if (rest == QLatin1String(e.right)) {
            key->qtKey = e.qtKey; key->side = KeyEntry::SideRight; matchedModifier = true; break;
        }
    }
    if (!matchedModifier) {
        // 泛名兼容历史值
        if (rest == QLatin1String("CTRL"))
            key->qtKey = Qt::Key_Control;
        else if (rest == QLatin1String("ALT"))
            key->qtKey = Qt::Key_Alt;
        else if (rest == QLatin1String("SHIFT"))
            key->qtKey = Qt::Key_Shift;
        else if (rest == QLatin1String("SUPER"))
            key->qtKey = Qt::Key_Meta;
        else if (!keyForFcitx4Name(rest, key))
            return false;
    }
    key->modifiers = modifiers;
    if (key->isModifier())
        key->modifiers |= modifierForQtKey(key->qtKey);
    return true;
}

bool Fcitx4KeyFormatter::decodeList(const QString& stored, KeyEntryList* entries) const
{
    if (!entries)
        return false;
    entries->clear();
    if (stored.trimmed().isEmpty())
        return true;

    // 空格分隔最多 2 组热键（FcitxHotkey[2]）；组内用下划线
    const QStringList values = stored.split(QLatin1Char(' '), kSkipEmptyParts);
    if (values.isEmpty())
        return false;
    for (const QString& value : values) {
        KeyEntry entry;
        if (!decode(value.trimmed(), &entry)) {
            entries->clear();
            return false;
        }
        entries->append(entry);
    }
    return true;
}

bool Fcitx4KeyFormatter::encodeList(const KeyEntryList& entries, QString* stored) const
{
    if (!stored)
        return false;
    stored->clear();
    if (entries.size() > 2)
        return false;

    QStringList values;
    for (const KeyEntry& entry : entries) {
        QString value;
        if (!encode(entry, &value))
            return false;
        values.append(value);
    }
    *stored = values.join(QLatin1Char(' '));
    return true;
}
