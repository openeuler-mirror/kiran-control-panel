/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#include "connection-itemwidget.h"
#include <qt5-log-i.h>
#include <QPainter>
#include <QSvgRenderer>
#include <QLineEdit>
#include "animation-loading-label.h"
// 使用默认析构函数，父对象被释放时，会释放子对象
ConnectionItemWidget::ConnectionItemWidget( QWidget* parent) : QWidget(parent)
{
    initUI();
}

void ConnectionItemWidget::initUI()
{
    initPluginItemWidget();
    connect(m_editButton, &QPushButton::clicked, this, &ConnectionItemWidget::editButtonClicked);
}

void ConnectionItemWidget::initPluginItemWidget()
{
    m_connectionTypeIcon = new QLabel(this);
    m_connectionName = new QLabel(this);
    m_horizonLayout = new QHBoxLayout(this);
    m_editButton = new QPushButton(this);
    m_activatedLabel = new AnimationLoadingLabel(this);

    m_connectionTypeIcon->setVisible(false);
    m_activatedLabel->setVisible(false);

    m_horizonLayout->addWidget(m_connectionTypeIcon);
    m_horizonLayout->addWidget(m_connectionName);
    m_horizonLayout->addStretch();
    m_horizonLayout->addWidget(m_activatedLabel);
    m_horizonLayout->addWidget(m_editButton);
    m_horizonLayout->setMargin(0);

    this->setLayout(m_horizonLayout);
}

//TODO:名称过长进行缩略
void ConnectionItemWidget::setName(const QString& name)
{
    QString nameStr = name;
    //    QFontMetricsF fontMetricsF(m_connectionName->font());
    //    if(fontMetricsF.width(nameStr) > m_connectionName->width())
    //    {
    //        nameStr = fontMetricsF.elidedText(nameStr,Qt::ElideRight,m_connectionName->width());
    //    }
    //    KLOG_DEBUG() << "elidedText:" << nameStr;
    m_connectionName->setText(nameStr);
}

QString ConnectionItemWidget::getName()
{
    return m_connectionName->text();
}

//TODO:其他状态信息的显示，以及优化
void ConnectionItemWidget::activatedStatus()
{
    QPixmap pixmap(":/kcp-network-images/correct.png");
    m_activatedLabel->setPixmap(pixmap);
    m_activatedLabel->setAlignment(Qt::AlignCenter);
    m_activatedLabel->setVisible(true);
}

void ConnectionItemWidget::deactivateStatus()
{
    m_activatedLabel->setVisible(false);
}

void ConnectionItemWidget::setLoadingStatus(bool isLoading)
{
    m_activatedLabel->setLoadingStatus(isLoading);
}

void ConnectionItemWidget::setLabelVisible(bool isVisible)
{
    m_activatedLabel->setVisible(isVisible);
}

void ConnectionItemWidget::setWirelessStatusIcon(bool security, int signal)
{
    QString svgPath;
    if (security)
    {
        if (0 <= signal && signal < 25)
            svgPath = ":/kcp-network-images/wireless-1-security.svg";
        else if (25 <= signal && signal < 50)
            svgPath = ":/kcp-network-images/wireless-2-security.svg";
        else if (50 <= signal && signal < 75)
            svgPath = ":/kcp-network-images/wireless-3-security.svg";
        else if (75 <= signal && signal <= 100)
            svgPath = ":/kcp-network-images/wireless-4-security.svg";
    }
    else
    {
        if (0 <= signal && signal < 25)
            svgPath = ":/kcp-network-images/wireless-1.svg";
        else if (25 <= signal && signal < 50)
            svgPath = ":/kcp-network-images/wireless-2.svg";
        else if (50 <= signal && signal < 75)
            svgPath = ":/kcp-network-images/wireless-3.svg";
        else if (75 <= signal && signal <= 100)
            svgPath = ":/kcp-network-images/wireless-4.svg";
    }
    QPixmap pixmap = getPixmapFromSvg(svgPath);
    KLOG_DEBUG() << "svgPath:" << svgPath;
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

void ConnectionItemWidget::setWiredStatusIcon()
{
    // TODO:图标跟随网络状态改变
    QString svgPath = ":/kcp-network-images/wired-connection.svg";
    QPixmap pixmap = getPixmapFromSvg(svgPath);
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

QPixmap ConnectionItemWidget::getPixmapFromSvg(const QString& svgPath)
{
    QSvgRenderer svgRenderer(QString(svgPath), this);
    QPixmap pixmap(QSize(16, 16));
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    svgRenderer.render(&painter);
    return pixmap;
}

void ConnectionItemWidget::setEditButtonVisible(bool isVisible)
{
    m_editButton->setVisible(isVisible);
}

void ConnectionItemWidget::setOtherNetworkIcon()
{
    QString svgPath = ":/kcp-network-images/wireless-other-network.svg";
    QPixmap pixmap = getPixmapFromSvg(svgPath);
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

