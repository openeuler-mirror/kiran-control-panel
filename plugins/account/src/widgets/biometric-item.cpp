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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "biometric-item.h"
#include "ui_biometric-item.h"

#include <QPainter>
#include <QStyleOption>
#include <QPainter>
#include <QPainterPath>

#include <style-palette.h>

 using namespace Kiran;

BiometricItem::BiometricItem(const QString &text,
                             const QString &biometricID,
                             BiometricItemType type,
                             QWidget *parent)
    : QWidget(parent),
      ui(new Ui::BiometricItem),
      m_itemType(type),
      m_biometricItemDataID(biometricID)
{
    ui->setupUi(this);
    initUI(text, biometricID, type);
}

BiometricItem::~BiometricItem()
{
    delete ui;
}

void BiometricItem::initUI(const QString &text, const QString &biometricID, BiometricItemType type)
{
    setFixedHeight(36);
    ui->label_text->setText(text);

    ui->btn_delete->setStyleSheet( " QToolButton{border-image:url(:/kcp-account/images/trash.svg);} "
                                   " QToolButton:hover{border-image:url(:/kcp-account/images/trash-hover.svg);}"
                                   " QToolButton:pressed{border-image:url(:/kcp-account/images/trash-pressed.svg);" );

    switch (type)
    {
    case BIOMETRIC_ITEM_ADD:
        ui->btn_add->setVisible(true);
        ui->btn_delete->setVisible(false);
        break;
    case BIOMETRIC_ITEM_NORMAL:
        ui->btn_delete->setVisible(true);
        ui->btn_add->setVisible(false);
        break;
    }

    connect(ui->btn_add, &QPushButton::clicked, [this]() {
        Q_EMIT sigAddBiometricItem();
    });

    connect(ui->btn_delete, &QPushButton::clicked, [=]() {
        Q_EMIT sigDeleteBiometricItem(text);
    });
}

void BiometricItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing,true);
    p.setPen(Qt::NoPen);

    auto backgroundColor = StylePalette::instance()->color(isEnabled()?StylePalette::Normal:StylePalette::Disabled,
                                                           StylePalette::Widget,
                                                           StylePalette::Background);

    QPainterPath painterPath;
    painterPath.addRoundedRect(opt.rect,6,6);

    p.fillPath(painterPath,backgroundColor);
}

void BiometricItem::setItemAddEnabled(bool enabled)
{
    ui->btn_add->setEnabled(enabled);
}

BiometricItem::BiometricItemType BiometricItem::getItemType()
{
    return m_itemType;
}

QString BiometricItem::getBiometricItemName()
{
    return ui->label_text->text();
}

QString BiometricItem::getBiometricItemDataID()
{
    return m_biometricItemDataID;
}
