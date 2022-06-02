 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */
 
#include "biometric-item.h"
#include "ui_biometric-item.h"

#include <QPainter>
#include <QStyleOption>
#include <QPainter>
#include <QPainterPath>

#include <kiran-palette.h>

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

    auto backgroundColor = KiranPalette::instance()->color(isEnabled()?KiranPalette::Normal:KiranPalette::Disabled,
                                                        KiranPalette::Widget,
                                                        KiranPalette::Background);

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
