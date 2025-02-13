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
 * Author:     yangxiaoqing <yangxiaoqing@kylinsec.com.cn>
 */

#include "device-panel.h"
#include "display-config.h"
#include "screen-identifying.h"
#include "ui_device-panel.h"

#include <palette.h>
#include <QPainter>
#include <QPainterPath>
#include <QToolTip>

using namespace Kiran::Theme;

DevicePanel::DevicePanel(QWidget *parent) : QFrame(parent),
                                            ui(new Ui::DevicePanel)
{
    ui->setupUi(this);
    setAccessibleName("DevicePanel");

    ui->pushButton_left->setIcon(QIcon::fromTheme("ksvg-rotation-left"));
    ui->pushButton_right->setIcon(QIcon::fromTheme("ksvg-rotation-right"));
    ui->pushButton_horizontal->setIcon(QIcon::fromTheme("ksvg-flip-h"));
    ui->pushButton_vertical->setIcon(QIcon::fromTheme("ksvg-flip-v"));
    ui->pushButton_identifying->setIcon(QIcon::fromTheme("ksvg-identification"));
    ui->btns_widget->setAttribute(Qt::WA_TranslucentBackground, true);

    connect(ui->contain, &DevicePanelWidget::screenItemChecked, this, [this](QString monitorPath)
            {
                ui->pushButton_horizontal->setChecked(ui->contain->getHorizontalDisplayReflectType());
                ui->pushButton_vertical->setChecked(ui->contain->getVerticalDisplayReflectType());
                emit screenItemChecked(monitorPath); });
    connect(ui->contain, &DevicePanelWidget::screenItemEnableChanged, this, [=](const bool &enabled)
            { ui->btns_widget->setEnabled(enabled); });

    connect(DisplayConfig::instance(), &DisplayConfig::configModeChanged, this, [this](ConfigMode configMode)
            {
                if (configMode == ConfigMode::CONFIG_MODE_EXTRA)
                    ui->pushButton_identifying->setVisible(true);
                else
                    ui->pushButton_identifying->setVisible(false); });
}

DevicePanel::~DevicePanel()
{
    delete ui;
}

void DevicePanel::changeItemDisabled(const bool &disabled)
{
    ui->contain->changeItemEnabled(disabled);
}

void DevicePanel::on_pushButton_left_clicked()
{
    ui->contain->setRotateDrect(1);
}

void DevicePanel::on_pushButton_horizontal_clicked(bool checked)
{
    ui->contain->setHorizontalDisplayReflectType(checked);
}

void DevicePanel::on_pushButton_vertical_clicked(bool checked)
{
    ui->contain->setVerticalDisplayReflectType(checked);
}

void DevicePanel::on_pushButton_right_clicked()
{
    ui->contain->setRotateDrect(-1);
}
// 标注屏幕
void DevicePanel::on_pushButton_identifying_clicked()
{
    QString monitorPath = ui->contain->getCurMonitorPath();
    if (monitorPath.isEmpty()) return;
    QString text = ui->contain->getCurMonitorText();
    QPoint pos = QPoint(DBusInterface::MonitorProperty(monitorPath, "x").toInt(), DBusInterface::MonitorProperty(monitorPath, "y").toInt());

    ScreenIdentifying identify(this);
    identify.setText(text);
    identify.exec(pos);
}

void DevicePanel::paintEvent(QPaintEvent *event)
{
    QColor backgroundColor = DEFAULT_PALETTE()->getColor(Palette::ColorGroup::ACTIVE, Palette::ColorRole::WIDGET);

    QPainterPath painterPath;
    painterPath.addRoundedRect(this->rect(), 6, 6);

    QPainter painter(this);
    painter.fillPath(painterPath, backgroundColor);

    QFrame::paintEvent(event);
}
