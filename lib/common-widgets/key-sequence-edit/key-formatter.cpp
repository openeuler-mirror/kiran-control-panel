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

#include "key-formatter.h"

#include <QKeySequence>
#include <QStringList>

namespace
{
bool isModifierQtKey(int qtKey)
{
    return qtKey == Qt::Key_Shift || qtKey == Qt::Key_Control || qtKey == Qt::Key_Alt ||
           qtKey == Qt::Key_AltGr || qtKey == Qt::Key_Meta || qtKey == Qt::Key_Super_L ||
           qtKey == Qt::Key_Super_R;
}
}

bool KeyEntry::isModifier() const
{
    return isModifierQtKey(qtKey);
}

bool KeyEntry::operator==(const KeyEntry& other) const
{
    // X11 keysym 是跨布局且可区分左右键的 canonical identity；仅当任一方
    // 没有 native keysym 时，才回退到 Qt 键值。
    if (keysym != 0 && other.keysym != 0)
        return keysym == other.keysym && modifiers == other.modifiers;
    // 回退路径只比较 qtKey+modifiers，会把左右修饰键（LSHIFT vs RSHIFT）都归一
    // 到同一 qtKey 而误判相等。side 记录左/右信息，此处必须一并比较，
    // 否则"重录右侧 Shift"会被短路（录制控件与写回均判等而失效）。
    return qtKey == other.qtKey && modifiers == other.modifiers && side == other.side;
}

QString KeyFormatter::displayText(const KeyEntry& entry)
{
    QStringList parts;
    Qt::KeyboardModifiers modifiers = entry.modifiers;
    QString mainModifier;
    if (entry.isModifier())
    {
        switch (entry.qtKey)
        {
        case Qt::Key_Control: mainModifier = QStringLiteral("Ctrl"); modifiers &= ~Qt::ControlModifier; break;
        case Qt::Key_Alt:
        case Qt::Key_AltGr: mainModifier = QStringLiteral("Alt"); modifiers &= ~Qt::AltModifier; break;
        case Qt::Key_Shift: mainModifier = QStringLiteral("Shift"); modifiers &= ~Qt::ShiftModifier; break;
        case Qt::Key_Meta:
        case Qt::Key_Super_L:
        case Qt::Key_Super_R: mainModifier = QStringLiteral("Super"); modifiers &= ~Qt::MetaModifier; break;
        default: break;
        }
        if (entry.side == KeyEntry::SideLeft)
            mainModifier.prepend(QStringLiteral("Left "));
        else if (entry.side == KeyEntry::SideRight)
            mainModifier.prepend(QStringLiteral("Right "));
    }
    if (modifiers & Qt::ControlModifier) parts << QStringLiteral("Ctrl");
    if (modifiers & Qt::AltModifier) parts << QStringLiteral("Alt");
    if (modifiers & Qt::ShiftModifier) parts << QStringLiteral("Shift");
    if (modifiers & Qt::MetaModifier) parts << QStringLiteral("Super");
    if (!mainModifier.isEmpty())
        parts << mainModifier;
    else if (!entry.isModifier()) {
        const QString text = QKeySequence(entry.qtKey).toString(QKeySequence::NativeText);
        if (!text.isEmpty()) parts << text;
    }
    return parts.join(QStringLiteral(" + "));
}
