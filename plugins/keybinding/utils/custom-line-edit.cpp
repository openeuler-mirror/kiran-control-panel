/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "custom-line-edit.h"
#include <kiran-log/qt5-log-i.h>
#include <QInputMethodEvent>
#include <QPainter>
#include <QStyleOption>
#include "keycode-translator.h"
#include "logging-category.h"

CustomLineEdit::CustomLineEdit(QWidget *parent) : QLineEdit(parent)
{
    initUI();
}

CustomLineEdit::~CustomLineEdit()
{
}

void CustomLineEdit::initUI()
{
    setFixedHeight(40);
    setFocusPolicy(Qt::ClickFocus);
    setReadOnly(true);
    setObjectName("CustomLineEdit");
    //    setStyleSheet("#CustomLineEdit{border:1px solid #393939;border-radius:6px;padding-left:10px;padding-right:10px;}"
    //                  "#CustomLineEdit:focus{border:1px solid #2eb3ff;}");
}

void CustomLineEdit::keyReleaseEvent(QKeyEvent *event)
{
    QList<int> keycodes;
    int qtkey = 0;

    KLOG_DEBUG(qLcKeybinding) << "Key Release Event:" << event->key() << "Modifiers:" << event->modifiers();

    // 忽略无效按键
    if (event->key() == 0 || event->key() == Qt::Key_unknown)
    {
        return;
    }

    // 特殊处理单独按下Backspace
    if (event->key() == Qt::Key_Backspace && event->modifiers() == Qt::NoModifier)
    {
        return;
    }

    // 处理shift修饰的快捷键组合，按键不经过shift转化，将原始按键keycode转化为对应的Qt::Key
    if (event->modifiers() & Qt::ShiftModifier)
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 2)
        qtkey = KeycodeTranslator::keycode2QtKey(event);
#else
        qtkey = KeycodeTranslator::keycode2QtKey(event->nativeScanCode());
#endif
        KLOG_INFO(qLcKeybinding) << "convert KeyCode:" << event->nativeScanCode() << "to Qt::Key:" << qtkey;
    }

    // 定义修饰键
    static QList<Qt::KeyboardModifier> modifierOrder = {Qt::ControlModifier,
                                                        Qt::AltModifier,
                                                        Qt::ShiftModifier,
                                                        Qt::MetaModifier};

    // 定义修饰键与按键的映射关系
    static QHash<Qt::KeyboardModifier, int> modifierToKey = {{Qt::ControlModifier, Qt::Key_Control},
                                                             {Qt::AltModifier, Qt::Key_Alt},
                                                             {Qt::ShiftModifier, Qt::Key_Shift},
                                                             {Qt::MetaModifier, Qt::Key_Meta}};

    // 特殊处理super键为主键的情况：
    /** NOTE:
     * 1.单独按下super键：qt会识别Qt::Key_Super_L为主键，修饰键为Qt::MetaModifier
     * 2.先按下其他修饰键，再按下super键：qt会识别Qt::Key_Super_L为主键，修饰键为Qt::MetaModifier+其他修饰键
     * super作为主键的情况下，修饰键都会包含Qt::MetaModifier，为了防止后续转化为可读字串时出现两个Super，将Qt::MetaModifier去除
     */
    if (event->key() == Qt::Key_Super_L || event->key() == Qt::Key_Super_R)
    {
        // 添加除Qt::MetaModifier之外的修饰键
        auto modifiersWithoutMeta = event->modifiers();
        modifiersWithoutMeta &= ~Qt::MetaModifier;
        for (auto mod : modifierOrder)
        {
            if (modifiersWithoutMeta & mod)
            {
                keycodes.append(modifierToKey.value(mod));
            }
        }

        // 添加主键
        keycodes.append(event->key());
        emit inputKeyCodes(keycodes);
        return;
    }

    // 添加修饰键
    for (auto mod : modifierOrder)
    {
        if (event->modifiers() & mod)
        {
            keycodes.append(modifierToKey.value(mod));
        }
    }

    // 添加主键
    /** NOTE: 去重
     * 若按下shift+Alt（Alt作为主键）：
     * keyrelease信息为：event->key()=Qt::Key_Meta，经过上面keycode2QtKey()函数转换后，主键为Qt::Key_Alt;
     *                  event->modifiers()=ShiftModifier|AltModifier|MetaModifier
     * 其中，修饰键包含MetaModifier原因：通过查看xmodmap -pm， Alt 键会被映射为Meta键，
     * 因此，若修饰键传入Qt::Key_Alt，主键也会传入Qt::Key_Alt，导致误解
     */
    auto key = qtkey ? qtkey : event->key();
    if (!keycodes.contains(key))
    {
        keycodes.append(key);
    }

    if (keycodes.size() > 0)
    {
        emit inputKeyCodes(keycodes);
    }
}

void CustomLineEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        update();
        clear();
    }
    QWidget::mousePressEvent(event);
}

void CustomLineEdit::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    update();
    grabKeyboard();
    m_flag = true;
}
void CustomLineEdit::focusOutEvent(QFocusEvent *e)
{
    releaseKeyboard();
    update();
}
