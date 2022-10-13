/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#include "connection-itemwidget.h"
#include <qt5-log-i.h>
#include <QApplication>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>
#include "animation-loading-label.h"
#include "utils.h"

// 使用默认析构函数，父对象被释放时，会释放子对象
ConnectionItemWidget::ConnectionItemWidget(QWidget* parent) : KiranFrame(parent)
{
    initUI();
}

void ConnectionItemWidget::initUI()
{
    setFixedHeight(36);
    initPluginItemWidget();
    setDrawBroder(false);
    setAttribute(Qt::WA_Hover);
    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &ConnectionItemWidget::handleThemeChanged);
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

    //    auto pixmap = QApplication::style()->standardPixmap(QStyle::SP_ArrowRight);
    m_editButton->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/details-info.svg"));
    m_editButton->setIconSize(QSize(16, 16));
    m_editButton->setFixedSize(30, 36);
    m_editButton->setFlat(true);

    m_horizonLayout->addWidget(m_connectionTypeIcon);
    m_horizonLayout->addWidget(m_connectionName);
    m_horizonLayout->addStretch();
    m_horizonLayout->addWidget(m_activatedLabel);
    m_horizonLayout->addWidget(m_editButton);
    m_horizonLayout->setMargin(0);

    this->setLayout(m_horizonLayout);
    this->setContentsMargins(10, 0, 3, 0);
}

// TODO:名称过长进行缩略
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
    m_editButton->setAccessibleName(QString("ButtonEditConnectionName::%1").arg(nameStr));
}

QString ConnectionItemWidget::getName()
{
    return m_connectionName->text();
}

// TODO:其他状态信息的显示，以及优化
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
    QPixmap pixmap = NetworkUtils::trayIconColorSwitch(svgPath);
    KLOG_DEBUG() << "svgPath:" << svgPath;
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

void ConnectionItemWidget::setWiredStatusIcon()
{
    // TODO:图标跟随网络状态改变
    QString svgPath = ":/kcp-network-images/wired-connection.svg";
    QPixmap pixmap = NetworkUtils::trayIconColorSwitch(svgPath);
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

void ConnectionItemWidget::setEditButtonVisible(bool isVisible)
{
    m_editButton->setVisible(isVisible);
}

void ConnectionItemWidget::setOtherNetworkIcon()
{
    QString svgPath = ":/kcp-network-images/wireless-other-network.svg";
    QPixmap pixmap = NetworkUtils::trayIconColorSwitch(svgPath);
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

void ConnectionItemWidget::handleThemeChanged(Kiran::PaletteType paletteType)
{
    QPixmap pixmap = NetworkUtils::trayIconColorSwitch(m_connectionTypeIcon->pixmap(Qt::ReturnByValue));
    if (!pixmap.isNull())
        m_connectionTypeIcon->setPixmap(pixmap);
    m_editButton->setIcon(NetworkUtils::trayIconColorSwitch(":/kcp-network-images/details-info.svg"));
}

void ConnectionItemWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}