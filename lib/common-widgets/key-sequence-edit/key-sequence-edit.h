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

#ifndef KEY_SEQUENCE_EDIT_H
#define KEY_SEQUENCE_EDIT_H

#include <QString>
#include <QWidget>

#include <functional>

#include "key-formatter.h"

class KeyFormatter;
class QToolButton;

/// 通用快捷键录制控件（格式无关）
///
/// 录制交互对齐 FcitxQtKeySequenceWidget / KKeySequenceWidget 的成熟做法：
///   - 点击进入录制态，grabKeyboard() 全局抓键（焦点丢失不断录）
///   - 录制中拦截 ShortcutOverride，避免触发面板/窗口自身快捷键
///   - autoRepeat 过滤、AltGr 忽略、死键文本丢弃
///   - 维护「当前按下的修饰键集合」，主键按下即提交（组合键）；
///     仅按修饰键时，松开修饰键以纯修饰键提交（如 "Shift_L"，输入法备选切换键默认值）
///   - 冲突检测：提交前经注入的检查器，冲突拒绝并提示
///   - Esc 取消、清除按钮、录制中实时修饰键预览
///
/// 数据模型（KeyEntry）以 X11 keysym 为主键（布局无关、可区分左右修饰键），
/// 组件对外只交换格式无关的 KeyEntry；KeyFormatter 仅在展示和后端持久化边界
/// 序列化，避免把某个后端的字符串语法泄漏到页面逻辑。
class KeySequenceEdit : public QWidget
{
    Q_OBJECT
public:
    explicit KeySequenceEdit(QWidget* parent = nullptr);
    ~KeySequenceEdit() override;

    bool hasKeyEntry() const { return m_hasKeyEntry; }
    KeyEntry keyEntry() const { return m_keyEntry; }
    /// 设置当前快捷键，不发出 keyEntryChanged
    void setKeyEntry(const KeyEntry& key);
    void clearKeyEntry();

    bool isRecording() const { return m_recording; }

    /// 注入冲突检查器：返回 true 表示与现存快捷键冲突（提交被拒绝并提示）
    void setConflictCheck(const std::function<bool(const KeyEntry&)>& check);

    /// 注入展示/持久化格式器；控件接管传入对象的所有权。
    void setFormatter(KeyFormatter* formatter);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    /// 录制完成后发出（值确实发生变化时）
    void keyEntryChanged(const KeyEntry& key);
    /// 清除后发出
    void keyEntryCleared();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    /// 录制期间接管 ShortcutOverride，避免被 Qt 快捷键系统抢占；实际录入在 KeyPress 完成
    bool event(QEvent* event) override;

private:
    void initUI();
    void startRecording();
    void stopRecording(bool commit);
    KeyEntry composeEntry(const KeyEntry& mainKey) const;
    KeyEntry composeEntry(const KeyEntry& mainKey, Qt::KeyboardModifiers modifiers) const;
    bool isRepresentable(const KeyEntry& key) const;
    /// 提交格式无关键值（冲突检查通过才落地）
    void commitEntry(const KeyEntry& key);
    void updateClearButtonVisible();
    QString displayText() const;
    void showConflictTip(const KeyEntry& key);

private:
    QToolButton* m_clearButton = nullptr;
    KeyFormatter* m_formatter = nullptr;

    KeyEntry m_keyEntry;
    bool m_hasKeyEntry = false;
    /// 录制前的值，用于 Esc 取消
    KeyEntry m_entryBeforeRecording;
    bool m_hadEntryBeforeRecording = false;

    /// 录制期间当前按下的修饰键位掩码（不直接信 event->modifiers()，自行维护）
    Qt::KeyboardModifiers m_pressedModifiers = Qt::NoModifier;
    /// 录制期间按下过的修饰键组合（含主键自身位），纯修饰键提交用：
    /// 按住 Ctrl 再按 Shift_L → 松开时提交 "Control+Shift_L"，而非仅最后一个 Shift
    Qt::KeyboardModifiers m_commitModifiers = Qt::NoModifier;
    /// 最后一次按下的修饰键（含侧别），纯修饰键提交用
    KeyEntry m_lastPressedModifier;
    /// 录制期间是否按下过主键（纯修饰键判定的依据）
    bool m_hadMainKey = false;
    /// 提交被冲突检查拒绝（红色边框提示）
    bool m_conflict = false;

    bool m_recording = false;
    bool m_hovered = false;

    std::function<bool(const KeyEntry&)> m_conflictCheck;
};

#endif  // KEY_SEQUENCE_EDIT_H
