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

#include "shortcut-item.h"
#include "ui_shortcut-item.h"
#include "keycode-translator.h"

#include <kiran-log/qt5-log-i.h>

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

ShortcutItem::ShortcutItem(int type, ShortcutInfoPtr shortcutInfo, QWidget *parent)
    : KiranFrame(parent),
      ui(new Ui::ShortcutItem)
{
    ui->setupUi(this);
    setAccessibleName("ShortcutItem");
    m_shortcutInfo = shortcutInfo;
    m_type = type;

    initUI();
}

ShortcutItem::~ShortcutItem()
{
    delete ui;
}

void ShortcutItem::initUI()
{
    setRadius(6);
    setAttribute(Qt::WA_Hover);
    setDrawBroder(false);

    ui->btn_delete->hide();
    ui->btn_delete->setIcon(QIcon(":/kiran-control-panel/images/trash.svg"));

    ui->label_keybination->setStyleSheet("QLabel#label_keybination{color:#919191}");

    ui->label_name->setText(m_shortcutInfo->name);

    QString readableString = KeycodeTranslator::backendKeyString2Readable(m_shortcutInfo->keyCombination);
    ui->label_keybination->setText(readableString);

    connect(ui->btn_delete, &QToolButton::clicked, [this]{
        sigDelete(m_shortcutInfo->uid);
    });
}

void ShortcutItem::setName(QString name)
{
    m_shortcutInfo->name = name;
    ui->label_name->setText(name);
}

void ShortcutItem::setKeyBinding(QString keyCombination)
{
    m_shortcutInfo->keyCombination = keyCombination;

    QString readableString = KeycodeTranslator::backendKeyString2Readable(keyCombination);
    ui->label_keybination->setText(readableString);
}

void ShortcutItem::setAction(QString action)
{
    m_shortcutInfo->action = action;
}

void ShortcutItem::setEditMode(bool isEditMode)
{
    if (m_type == SHORTCUT_TYPE_CUSTOM)
        ui->btn_delete->setVisible(isEditMode);
}

/*
void ShortcutItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}
*/

void ShortcutItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit sigClicked(m_type, m_shortcutInfo->uid, m_shortcutInfo->name, m_shortcutInfo->keyCombination, m_shortcutInfo->action);
    }
    QWidget::mousePressEvent(event);
}

QString ShortcutItem::getName()
{
    return m_shortcutInfo->name;
}

int ShortcutItem::getType()
{
    return m_shortcutInfo->type;
}

QString ShortcutItem::getUid()
{
    return m_shortcutInfo->uid;
}

QString ShortcutItem::getShowKeybinding()
{
    return ui->label_keybination->text();
}

ShortcutInfoPtr ShortcutItem::getShortcut()
{
    return m_shortcutInfo;
}
