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

#include "key-sequence-edit.h"

#include <style-palette.h>

#include <QEvent>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QIcon>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QToolButton>
#include <QToolTip>

using namespace Kiran;

namespace
{
const int kEditHeight = 32;
const int kMarginLeft = 12;
const int kMarginRight = 8;
const int kClearButtonSize = 20;
const int kSpacing = 6;

/// X11 右侧修饰键 keysym（keysymdef.h）
const quint32 kKeySymShiftL = 0xffe1;
const quint32 kKeySymShiftR = 0xffe2;
const quint32 kKeySymControlL = 0xffe3;
const quint32 kKeySymControlR = 0xffe4;
const quint32 kKeySymAltL = 0xffe9;
const quint32 kKeySymAltR = 0xffea;
const quint32 kKeySymSuperL = 0xffeb;
const quint32 kKeySymSuperR = 0xffec;

/// 依据 Qt 键 + X11 keysym 判定修饰键侧别（X11 下 nativeVirtualKey() 即 keysym）
KeyEntry::Side detectModifierSide(int qtKey, quint32 keysym)
{
    switch (qtKey)
    {
    case Qt::Key_Shift:
    case Qt::Key_Super_L:
    case Qt::Key_Super_R:
        return (keysym == kKeySymShiftR || keysym == kKeySymSuperR)
                   ? KeyEntry::SideRight
                   : (keysym == kKeySymShiftL || keysym == kKeySymSuperL) ? KeyEntry::SideLeft
                                                                          : KeyEntry::SideNone;
    case Qt::Key_Control:
        return (keysym == kKeySymControlR) ? KeyEntry::SideRight
                                           : (keysym == kKeySymControlL) ? KeyEntry::SideLeft
                                                                         : KeyEntry::SideNone;
    case Qt::Key_Alt:
        return (keysym == kKeySymAltR) ? KeyEntry::SideRight
                                       : (keysym == kKeySymAltL) ? KeyEntry::SideLeft
                                                                 : KeyEntry::SideNone;
    default:
        return KeyEntry::SideNone;
    }
}

/// 纯修饰键的展示前缀顺序（与 fcitx5 存储一致：Control、Alt、Shift、Super）
QStringList modifierPrefixes(Qt::KeyboardModifiers modifiers)
{
    QStringList prefixes;
    if (modifiers & Qt::ControlModifier)
    {
        prefixes << QStringLiteral("Control");
    }
    if (modifiers & Qt::AltModifier)
    {
        prefixes << QStringLiteral("Alt");
    }
    if (modifiers & Qt::ShiftModifier)
    {
        prefixes << QStringLiteral("Shift");
    }
    if (modifiers & Qt::MetaModifier)
    {
        prefixes << QStringLiteral("Super");
    }
    return prefixes;
}
}  // namespace

KeySequenceEdit::KeySequenceEdit(QWidget* parent)
    : QWidget(parent)
{
    initUI();
    setFormatter(nullptr);
}

KeySequenceEdit::~KeySequenceEdit()
{
    delete m_formatter;
    m_formatter = nullptr;
}

void KeySequenceEdit::initUI()
{
    setObjectName("KeySequenceEdit");
    setAttribute(Qt::WA_Hover);
    setFixedHeight(kEditHeight);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);

    m_clearButton = new QToolButton(this);
    m_clearButton->setObjectName("btn_clear");
    m_clearButton->setFixedSize(kClearButtonSize, kClearButtonSize);
    m_clearButton->setIconSize(QSize(12, 12));
    m_clearButton->setIcon(QIcon::fromTheme("ksvg-window-close"));
    m_clearButton->setAutoRaise(true);
    m_clearButton->setFocusPolicy(Qt::NoFocus);
    m_clearButton->setToolTip(tr("Clear shortcut"));
    m_clearButton->hide();

    connect(m_clearButton, &QToolButton::clicked, this,
            [this]
            {
                if (m_recording)
                {
                    stopRecording(false);
                }
                clearKeyEntry();
            });
}

void KeySequenceEdit::setFormatter(KeyFormatter* formatter)
{
    if (m_formatter)
    {
        delete m_formatter;
    }
    m_formatter = formatter;
    update();
}

void KeySequenceEdit::setConflictCheck(const std::function<bool(const KeyEntry&)>& check)
{
    m_conflictCheck = check;
}

void KeySequenceEdit::setKeyEntry(const KeyEntry& key)
{
    m_keyEntry = key;
    m_hasKeyEntry = true;
    updateClearButtonVisible();
    update();
}

void KeySequenceEdit::clearKeyEntry()
{
    if (!m_hasKeyEntry)
        return;
    m_keyEntry = KeyEntry();
    m_hasKeyEntry = false;
    updateClearButtonVisible();
    update();
    emit keyEntryCleared();
}

void KeySequenceEdit::commitEntry(const KeyEntry& key)
{
    if (m_hasKeyEntry && m_keyEntry == key)
    {
        updateClearButtonVisible();
        update();
        return;
    }

    m_keyEntry = key;
    m_hasKeyEntry = true;
    updateClearButtonVisible();
    update();

    emit keyEntryChanged(m_keyEntry);
}

void KeySequenceEdit::updateClearButtonVisible()
{
    // 有值且（悬浮或聚焦）时显示清除按钮；录制中不显示，避免误触
    bool visible = !m_recording && m_hasKeyEntry && (m_hovered || hasFocus());
    m_clearButton->setVisible(visible);
}

KeyEntry KeySequenceEdit::composeEntry(const KeyEntry& mainKey) const
{
    return composeEntry(mainKey, m_pressedModifiers);
}

KeyEntry KeySequenceEdit::composeEntry(const KeyEntry& mainKey, Qt::KeyboardModifiers modifiers) const
{
    KeyEntry entry = mainKey;
    entry.modifiers = modifiers;
    return entry;
}

bool KeySequenceEdit::isRepresentable(const KeyEntry& key) const
{
    QString stored;
    return !m_formatter || m_formatter->encode(key, &stored);
}

QString KeySequenceEdit::displayText() const
{
    if (m_recording)
    {
        // 录制中：已按下修饰键时显示实时预览（"Control + ..."）
        QStringList prefixes = modifierPrefixes(m_pressedModifiers);
        if (!prefixes.isEmpty())
        {
            return prefixes.join(QStringLiteral(" + ")) + QStringLiteral(" + ...");
        }
        return tr("Press shortcut...");
    }

    if (!m_hasKeyEntry)
    {
        return tr("None");
    }
    return KeyFormatter::displayText(m_keyEntry);
}

void KeySequenceEdit::showConflictTip(const KeyEntry& key)
{
    // 冲突提示：红框 + tooltip 说明该快捷键已存在
    m_conflict = true;
    update();
    QToolTip::showText(mapToGlobal(QPoint(width() / 2, height())),
                       tr("Shortcut \"%1\" is already in use").arg(KeyFormatter::displayText(key)),
                       this);
}

QSize KeySequenceEdit::sizeHint() const
{
    return QSize(180, kEditHeight);
}

QSize KeySequenceEdit::minimumSizeHint() const
{
    return QSize(120, kEditHeight);
}

void KeySequenceEdit::resizeEvent(QResizeEvent* event)
{
    if (event)
    {
        QWidget::resizeEvent(event);
    }

    int x = width() - kMarginRight - kClearButtonSize;
    int y = (height() - kClearButtonSize) / 2;
    m_clearButton->move(x, y);
}

void KeySequenceEdit::startRecording()
{
    if (m_recording)
    {
        return;
    }

    m_recording = true;
    m_entryBeforeRecording = m_keyEntry;
    m_hadEntryBeforeRecording = m_hasKeyEntry;
    m_pressedModifiers = Qt::NoModifier;
    m_commitModifiers = Qt::NoModifier;
    m_lastPressedModifier = KeyEntry();
    m_hadMainKey = false;
    m_conflict = false;

    // 录制期间抓取键盘，避免快捷键被上层窗口/全局快捷键吞掉
    grabKeyboard();

    updateClearButtonVisible();
    update();
}

void KeySequenceEdit::stopRecording(bool commit)
{
    if (!m_recording)
    {
        return;
    }

    m_recording = false;
    releaseKeyboard();

    m_pressedModifiers = Qt::NoModifier;
    m_commitModifiers = Qt::NoModifier;
    m_lastPressedModifier = KeyEntry();
    m_hadMainKey = false;

    if (!commit)
    {
        // 取消：恢复录制前的值
        m_keyEntry = m_entryBeforeRecording;
        m_hasKeyEntry = m_hadEntryBeforeRecording;
        updateClearButtonVisible();
        update();
    }
}

void KeySequenceEdit::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (!m_recording)
        {
            setFocus(Qt::MouseFocusReason);
            startRecording();
        }
    }
    QWidget::mousePressEvent(event);
}

bool KeySequenceEdit::event(QEvent* event)
{
    // ShortcutOverride 是 Qt 在真正的 KeyPress 前发出的预分发事件。这里只声明
    // 该组合不应被 Qt 快捷键系统抢占；不能在这里录入或结束录制：同一个物理按键
    // 随后还会带着完整修饰状态到达 KeyPress。尤其 Ctrl+Space 常已被输入法注册，
    // 在预分发阶段提交并释放键盘抓取会使该组合无法稳定完成录入。
    if (m_recording && event->type() == QEvent::ShortcutOverride)
    {
        event->accept();
        return true;
    }
    return QWidget::event(event);
}

void KeySequenceEdit::keyPressEvent(QKeyEvent* event)
{
    if (!m_recording)
    {
        // 未录制时空格/回车进入录制，便于键盘操作
        if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return ||
            event->key() == Qt::Key_Enter)
        {
            startRecording();
            event->accept();
            return;
        }
        QWidget::keyPressEvent(event);
        return;
    }

    // Esc 取消录制并恢复原值
    if (event->key() == Qt::Key_Escape && event->modifiers() == Qt::NoModifier)
    {
        stopRecording(false);
        event->accept();
        return;
    }

    if (event->isAutoRepeat())
    {
        event->accept();
        return;
    }

    // AltGr 在 X11 下是 Control+Alt，会污染录入（如录入 Ctrl+Alt+5 实为 AltGr+5），显式忽略
    if (event->key() == Qt::Key_AltGr)
    {
        event->accept();
        return;
    }

    // 清掉上一次冲突提示
    m_conflict = false;

    const int qtKey = event->key();
    const quint32 keysym = quint32(event->nativeVirtualKey());

    // 修饰键：只更新按下状态，不提交
    KeyEntry mainKey(qtKey, event->modifiers(), keysym, detectModifierSide(qtKey, keysym));
    if (mainKey.isModifier())
    {
        // 更新按下集合：合并事件修饰位 + 主键自身位。
        // 注意 X11 修饰键 press 事件的 state 不含自身位（Ctrl_L 按下 state 无
        // Control 位），Qt modifiers() 亦然，必须显式补上，否则组合会被记录成空。
        Qt::KeyboardModifiers selfMod = Qt::NoModifier;
        switch (qtKey)
        {
        case Qt::Key_Shift:
            selfMod = Qt::ShiftModifier;
            break;
        case Qt::Key_Control:
            selfMod = Qt::ControlModifier;
            break;
        case Qt::Key_Alt:
        case Qt::Key_AltGr:
            selfMod = Qt::AltModifier;
            break;
        case Qt::Key_Meta:
        case Qt::Key_Super_L:
        case Qt::Key_Super_R:
            selfMod = Qt::MetaModifier;
            break;
        default:
            break;
        }
        m_pressedModifiers |= (event->modifiers() & (Qt::ControlModifier | Qt::AltModifier |
                                                     Qt::ShiftModifier | Qt::MetaModifier)) |
                              selfMod;

        // 记录当前按下组合（纯修饰键提交用：松开最后一个修饰键时提交完整组合，
        // 而非仅最后一个修饰键——修复 Ctrl+Shift 被降级为 Shift 的误报冲突）
        m_commitModifiers = m_pressedModifiers;
        // 纯修饰键场景记录最后按下的修饰键（含侧别），松开时作为组合主键提交
        m_lastPressedModifier = mainKey;
        update();
        event->accept();
        return;
    }

    // 主键：结合当前按下的修饰键集合生成序列并提交
    m_hadMainKey = true;
    KeyEntry commitKey;
    commitKey.qtKey = qtKey;
    commitKey.keysym = keysym;
    commitKey.side = detectModifierSide(qtKey, keysym);

    const KeyEntry entry = composeEntry(commitKey);
    if (!isRepresentable(entry))
    {
        // 无法映射的键（如某些特殊键）：忽略，保持录制
        event->accept();
        return;
    }

    // 冲突检测：冲突则拒绝提交并提示，保持录制等待重新按键
    if (m_conflictCheck && m_conflictCheck(entry))
    {
        showConflictTip(entry);
        event->accept();
        return;
    }

    commitEntry(entry);
    stopRecording(true);
    event->accept();
}

void KeySequenceEdit::keyReleaseEvent(QKeyEvent* event)
{
    if (!m_recording)
    {
        QWidget::keyReleaseEvent(event);
        return;
    }

    if (event->isAutoRepeat())
    {
        event->accept();
        return;
    }

    // 主键 release：已在 press 时提交，忽略
    const int qtKey = event->key();
    const bool isModifierRelease = KeyEntry{qtKey, Qt::NoModifier, 0, KeyEntry::SideNone}.isModifier();
    if (!isModifierRelease)
    {
        event->accept();
        return;
    }

    // 修饰键 release：从按下集合移除
    // 注意：Qt 将左右修饰键归一化为 Key_Shift/Key_Control/Key_Alt/Key_Meta，
    // 侧别信息只存在于 nativeVirtualKey() 的 keysym 中（录入侧别时已使用）。
    if (qtKey == Qt::Key_Control)
    {
        m_pressedModifiers &= ~Qt::ControlModifier;
    }
    else if (qtKey == Qt::Key_Alt || qtKey == Qt::Key_AltGr)
    {
        m_pressedModifiers &= ~Qt::AltModifier;
    }
    else if (qtKey == Qt::Key_Shift)
    {
        m_pressedModifiers &= ~Qt::ShiftModifier;
    }
    else if (qtKey == Qt::Key_Meta || qtKey == Qt::Key_Super_L || qtKey == Qt::Key_Super_R)
    {
        m_pressedModifiers &= ~Qt::MetaModifier;
    }

    // 纯修饰键提交：期间未按下主键，且最后一个修饰键已松开 → 提交按下的修饰键组合。
    // 例如按住 Ctrl 再按 Shift_L 松开 → "Control+Shift_L"（主键自身位由 formatter 剔除）。
    if (!m_hadMainKey && m_pressedModifiers == Qt::NoModifier &&
        m_lastPressedModifier.isModifier())
    {
        const KeyEntry entry = composeEntry(m_lastPressedModifier, m_commitModifiers);
        m_lastPressedModifier = KeyEntry();
        m_commitModifiers = Qt::NoModifier;

        if (isRepresentable(entry))
        {
            if (m_conflictCheck && m_conflictCheck(entry))
            {
                showConflictTip(entry);
            }
            else
            {
                commitEntry(entry);
                stopRecording(true);
            }
        }
    }

    update();
    event->accept();
}

void KeySequenceEdit::focusOutEvent(QFocusEvent* event)
{
    // 失焦时取消录制，避免控件停留在录制态
    if (m_recording)
    {
        stopRecording(false);
    }
    updateClearButtonVisible();
    QWidget::focusOutEvent(event);
}

void KeySequenceEdit::enterEvent(QEvent* event)
{
    m_hovered = true;
    updateClearButtonVisible();
    update();
    QWidget::enterEvent(event);
}

void KeySequenceEdit::leaveEvent(QEvent* event)
{
    m_hovered = false;
    updateClearButtonVisible();
    update();
    QWidget::leaveEvent(event);
}

void KeySequenceEdit::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    StylePalette* palette = StylePalette::instance();
    StylePalette::ColorState group = isEnabled() ? StylePalette::Active
                                                 : StylePalette::Disabled;

    QRectF backgroundRect = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    QPainterPath backgroundPath;
    backgroundPath.addRoundedRect(backgroundRect, 4, 4);

    // 录制中用选中色突出；冲突时用警示色边框；其余按悬浮/常态取色
    StylePalette::ColorState backgroundGroup = group;
    if (m_recording)
    {
        backgroundGroup = StylePalette::Checked;
    }
    else if (m_hovered)
    {
        backgroundGroup = StylePalette::Hover;
    }
    painter.fillPath(backgroundPath,
                     palette->color(backgroundGroup, StylePalette::Widget, StylePalette::Background));

    QColor borderColor = palette->color(m_recording ? StylePalette::Checked : group,
                                        StylePalette::Widget, StylePalette::Border);
    if (m_conflict)
    {
        // 冲突：红框强调
        borderColor = QColor(QStringLiteral("#e64545"));
    }
    painter.setPen(borderColor);
    painter.drawPath(backgroundPath);

    // 文本
    QColor textColor = palette->color(group, StylePalette::Widget, StylePalette::Foreground);
    QString text = displayText();
    if (!m_recording && !m_hasKeyEntry)
    {
        // 「未设置」用弱化色
        textColor.setAlphaF(0.45);
    }
    painter.setPen(textColor);

    int textRight = m_clearButton->isVisible()
                        ? width() - kMarginRight - kClearButtonSize - kSpacing
                        : width() - kMarginRight;
    int textWidth = qMax(0, textRight - kMarginLeft);

    QFontMetrics fm(font());
    painter.drawText(QRect(kMarginLeft, 0, textWidth, height()),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     fm.elidedText(text, Qt::ElideRight, textWidth));
}
