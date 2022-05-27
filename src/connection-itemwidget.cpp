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
ConnectionItemWidget::ConnectionItemWidget(ItemWidgetType itemWidgetType, QWidget* parent) : QWidget(parent)
{
    m_itemWidgetType = itemWidgetType;
    initUI();
}

void ConnectionItemWidget::initUI()
{
    if (m_itemWidgetType == ITEM_WIDGET_TYPE_PLUGIN)
        initPluginItemWidget();
    else
        initTrayItemWidget();
    connect(m_actionButton, &QPushButton::clicked, this, &ConnectionItemWidget::actionButtonClicked);
}

void ConnectionItemWidget::initPluginItemWidget()
{
    m_connectionTypeIcon = new QLabel(this);
    m_connectionName = new QLabel(this);
    m_horizonLayout = new QHBoxLayout(this);
    m_actionButton = new QPushButton(this);
    m_activatedLabel = new AnimationLoadingLabel(this);

    m_connectionTypeIcon->setVisible(false);
    m_activatedLabel->setVisible(false);

    m_horizonLayout->addWidget(m_connectionTypeIcon);
    m_horizonLayout->addWidget(m_connectionName);
    m_horizonLayout->addStretch();
    m_horizonLayout->addWidget(m_activatedLabel);
    m_horizonLayout->addWidget(m_actionButton);
    m_horizonLayout->setMargin(0);

    this->setLayout(m_horizonLayout);
}

void ConnectionItemWidget::initTrayItemWidget()
{
    m_connectionTypeIcon = new QLabel(this);
    m_connectionTypeIcon->setVisible(true);
    m_connectionName = new QLabel(this);
    //    m_connectionName->setStyleSheet("background:yellow;");

    m_horizonIconAndNameLayout = new QHBoxLayout();
    m_horizonIconAndNameLayout->addWidget(m_connectionTypeIcon);
    m_horizonIconAndNameLayout->addWidget(m_connectionName);
    m_horizonIconAndNameLayout->setSpacing(10);
    m_horizonIconAndNameLayout->addStretch();

    m_verticalLayout = new QVBoxLayout();
    m_verticalLayout->addLayout(m_horizonIconAndNameLayout);

    m_activeStatusWidget = new QWidget(this);
    m_horizonActivateStatusLabelLayout = new QHBoxLayout(m_activeStatusWidget);
    // icon:16*16 ,spacing:10 -> Spacer:(26,16)
    horizontalSpacer = new QSpacerItem(26, 16, QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_horizonActivateStatusLabelLayout->addSpacerItem(horizontalSpacer);

    m_connectionStatus = new QLabel(m_activeStatusWidget);
    m_connectionStatus->setFixedWidth(80);
    m_connectionStatus->setVisible(false);
    //    m_connectionStatus->setStyleSheet("background:red;");
    m_connectionStatus->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    m_horizonActivateStatusLabelLayout->addWidget(m_connectionStatus);
    m_horizonActivateStatusLabelLayout->setAlignment(m_connectionStatus, Qt::AlignLeft);

    m_horizonActivateStatusLabelLayout->setMargin(0);
    m_horizonActivateStatusLabelLayout->setSpacing(10);
    m_horizonActivateStatusLabelLayout->addStretch();

    m_activeStatusWidget->setLayout(m_horizonActivateStatusLabelLayout);
    //    m_activeStatusWidget->setStyleSheet("background:green;");

    m_verticalLayout->addWidget(m_activeStatusWidget);

    m_verticalLayout->setSpacing(0);
    m_verticalLayout->setMargin(0);

    m_horizonLayout = new QHBoxLayout(this);
    m_actionButton = new QPushButton(this);
    m_actionButton->setText(tr("Disconnect"));
    m_actionButton->setVisible(false);

    m_horizonLayout->addLayout(m_verticalLayout);
    m_horizonLayout->addStretch();

    m_activatedLabel = new AnimationLoadingLabel(this);
    m_activatedLabel->setVisible(false);
    //    m_activatedLabel->setStyleSheet("background:pink;");

    m_horizonLayout->addWidget(m_activatedLabel);
    m_horizonLayout->addWidget(m_actionButton);
    m_horizonLayout->setMargin(0);

    m_horizonLayout->setContentsMargins(10, 0, 10, 0);
    m_activeStatusWidget->setVisible(false);
    //    this->setLayout(m_horizonLayout);

    setFixedHeight(50);
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
    if (m_itemWidgetType == ITEM_WIDGET_TYPE_PLUGIN)
    {
        QPixmap pixmap = getPixmapFromSvg(":/kcp-network-images/correct.png");
        m_activatedLabel->setPixmap(pixmap);
        m_activatedLabel->setAlignment(Qt::AlignCenter);
        m_activatedLabel->setVisible(true);
    }
    else
    {
        m_activatedLabel->setVisible(false);
        m_connectionStatus->setText(tr("Activated"));
        m_connectionStatus->setVisible(true);
        m_activeStatusWidget->setVisible(true);
        m_actionButton->setVisible(true);
        m_horizonIconAndNameLayout->setContentsMargins(0, 10, 0, 0);
        m_horizonActivateStatusLabelLayout->setContentsMargins(0, 0, 0, 10);
    }
}

void ConnectionItemWidget::deactivateStatus()
{
    if (m_itemWidgetType == ITEM_WIDGET_TYPE_PLUGIN)
    {
        m_activatedLabel->setVisible(false);
    }
    else
    {
        m_activatedLabel->setVisible(false);
        m_connectionStatus->setVisible(false);
        m_activeStatusWidget->setVisible(false);
        m_actionButton->setVisible(false);
        m_horizonIconAndNameLayout->setMargin(0);
    }
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

void ConnectionItemWidget::setActionButtonVisible(bool isVisible)
{
    m_actionButton->setVisible(isVisible);
}

void ConnectionItemWidget::setOtherNetworkIcon()
{
    QString svgPath = ":/kcp-network-images/wireless-other-network.svg";
    QPixmap pixmap = getPixmapFromSvg(svgPath);
    m_connectionTypeIcon->setPixmap(pixmap);
    m_connectionTypeIcon->setAlignment(Qt::AlignCenter);
    m_connectionTypeIcon->setVisible(true);
}

InputPasswordWidget::InputPasswordWidget(QWidget* parent) : QWidget(parent)
{
    intUI();
    initConnection();
}

void InputPasswordWidget::intUI()
{
//    setFixedHeight(50);
    setFixedSize(240,50);
    m_passwordEdit =  new QLineEdit(this);
    m_passwordEdit->setFixedSize(160,24);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_activateConnectionButton = new QPushButton(this);
    m_activateConnectionButton->setText(tr("Connect"));
    m_activateConnectionButton->setFixedSize(50,24);

    m_horizonLayout = new QHBoxLayout(this);

    m_horizonLayout->addWidget(m_passwordEdit);
    m_horizonLayout->addWidget(m_activateConnectionButton);
    m_horizonLayout->setSpacing(10);
    m_horizonLayout->setContentsMargins(10,13,10,13);
    this->setLayout(m_horizonLayout);
}

void InputPasswordWidget::initConnection()
{
    connect(m_activateConnectionButton,&QPushButton::clicked,this,&InputPasswordWidget::handleInputPassword);
}


void InputPasswordWidget::handleInputPassword()
{
    QString password = m_passwordEdit->text();
    emit sendPassword(password);
}

QString InputPasswordWidget::getPassword()
{
    return m_passwordEdit->text();
}
