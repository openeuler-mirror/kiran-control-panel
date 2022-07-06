/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-display is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 */

#include "kiran-display-configuration-panel.h"
#include "ui_kiran-display-configuration-panel.h"
#include "kiran-display-config-global.h"
#include "kiran-display-config-identifying.h"


#include <QToolTip>
#include <QPainter>
#include <QPainterPath>
#include <style-property.h>
#include <style-palette.h>

using namespace Kiran;
using namespace KiranDisplayConfigGlobal;

KiranDisplayConfigurationPanel::KiranDisplayConfigurationPanel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::KiranDisplayConfigurationPanel)
{
    ui->setupUi(this);

    ui->pushButton_left->setThemeIcon(QPixmap(":/kcp-display/images/rotation-left.svg"),PALETTE_DARK);
    ui->pushButton_right->setThemeIcon(QPixmap(":/kcp-display/images/rotation-right.svg"),PALETTE_DARK);
    ui->pushButton_horizontal->setThemeIcon(QPixmap(":/kcp-display/images/flip-h.svg"),PALETTE_DARK);
    ui->pushButton_vertical->setThemeIcon(QPixmap(":/kcp-display/images/flip-v.svg"),PALETTE_DARK);
    ui->pushButton_identifying->setThemeIcon(QPixmap(":/kcp-display/images/identification.svg"),PALETTE_DARK);
    ui->btns_widget->setAttribute(Qt::WA_TranslucentBackground, true);

    connect(ui->contain, &KiranDisplayConfigItemContain::sigButtonChecked, this, [=](QString monitorPath){
        ui->pushButton_horizontal->setChecked(ui->contain->getHorizontalDisplayReflectType());
        ui->pushButton_vertical->setChecked(ui->contain->getVerticalDisplayReflectType());
        emit buttonChecked(monitorPath);
    });
    connect(ui->contain, &KiranDisplayConfigItemContain::sigItemEnableChanged, this, [=](const bool &enabled){ui->btns_widget->setEnabled(enabled);});
}

KiranDisplayConfigurationPanel::~KiranDisplayConfigurationPanel()
{
    delete ui;
}

QVariantMap KiranDisplayConfigurationPanel::getData()
{
    return ui->contain->getData();
}

void KiranDisplayConfigurationPanel::setData(const QVariantList &list, const bool &clearChecked)
{
    ui->pushButton_identifying->setVisible(list.count() > 1);
    ui->contain->setData(list, clearChecked);
   // m_textList = list;
   // m_textSize.clear();
}

void KiranDisplayConfigurationPanel::changeItemDisabled(const bool &disabled)
{
    ui->contain->changeItemEnabled(disabled);
}

//void KiranDisplayConfigurationPanel::paintEvent(QPaintEvent *event)
//{
////    if(m_textSize.isEmpty())
////    {
////        QPainter painter(this);
////        painter.save();
////        QFont pen = painter.font();
////        pen.setPixelSize(64);
////        QFontMetrics fm = painter.fontMetrics();
////        foreach (QVariant var, m_textList) {
////            QString text = var.toMap().value("text").toString();
////            m_textSize.insert(text, QSize(fm.width(text), fm.height()));
////        }
////        painter.restore();
////    }

//    QFrame::paintEvent(event);
//}

void KiranDisplayConfigurationPanel::on_pushButton_left_clicked()
{
    ui->contain->setRotateDrect(1);
}

void KiranDisplayConfigurationPanel::on_pushButton_horizontal_clicked(bool checked)
{
    ui->contain->setHorizontalDisplayReflectType(checked);
}

void KiranDisplayConfigurationPanel::on_pushButton_vertical_clicked(bool checked)
{
    ui->contain->setVerticalDisplayReflectType(checked);
}

void KiranDisplayConfigurationPanel::on_pushButton_right_clicked()
{
    ui->contain->setRotateDrect(-1);
}
//标注屏幕
void KiranDisplayConfigurationPanel::on_pushButton_identifying_clicked()
{
    QString monitorPath = ui->contain->getCurMonitorPath();
    if(monitorPath.isEmpty()) return;
    QString text = ui->contain->getCurMonitorText();
    QPoint pos = QPoint(MonitorProperty(monitorPath, "x").toInt(), MonitorProperty(monitorPath, "y").toInt());

    KiranDisplayConfigIdentifying identify(this);
    identify.setText(text);
    identify.exec(pos);
}

void KiranDisplayConfigurationPanel::paintEvent(QPaintEvent *event)
{
    auto kiranPalette = StylePalette::instance();
    QColor backgroundColor = kiranPalette->color(StylePalette::Normal,StylePalette::Widget,StylePalette::Background);

    QPainterPath painterPath;
    painterPath.addRoundedRect(this->rect(),6,6);

    QPainter painter(this);
    painter.fillPath(painterPath,backgroundColor);

    QFrame::paintEvent(event);
}
