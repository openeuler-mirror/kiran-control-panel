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
* Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#include "tray-itemwidget.h"
#include "ui_tray-itemwidget.h"
#include <QSvgRenderer>
#include <QPainter>
#include <qt5-log-i.h>

TrayItemWidget::TrayItemWidget(QWidget *parent) : QWidget(parent), ui(new Ui::TrayItemWidget)
{
    ui->setupUi(this);
    initUI();
    initConnection();
}

TrayItemWidget::~TrayItemWidget()
{
    delete ui;
}

void TrayItemWidget::initUI()
{
    ui->activeStatusWidget->setVisible(false);
    ui->inputTextWidget->setVisible(false);

    setFixedWidth(240);
    setContentsMargins(10,10,10,10);
}

void TrayItemWidget::initConnection()
{
    connect(ui->inputTextConnectButton,&QPushButton::clicked,this, &TrayItemWidget::handleInputText);
    connect(ui->inputTextCancelButton,&QPushButton::clicked,this,&TrayItemWidget::cancelButtonClicked);
    connect(ui->connectButton,&QPushButton::clicked,this,&TrayItemWidget::connectButtonClicked);
    connect(ui->disconnectButton,&QPushButton::clicked,this,&TrayItemWidget::disconnectButttonClicked);
    connect(ui->ignoreButton,&QPushButton::clicked,this,&TrayItemWidget::ignoreButtonClicked);
    connect(ui->cancelButton,&QPushButton::clicked,this,&TrayItemWidget::cancelButtonClicked);
}

void TrayItemWidget::setWidgetsInDifferentStatus(TrayItemWidgetStatus WidgetStatus)
{
}

void TrayItemWidget::setName(const QString &name)
{
    ui->connectionName->setText(name);
}

QString TrayItemWidget::getName()
{
    return ui->connectionName->text();
}

void TrayItemWidget::setWirelessStatusIcon(bool security, int signal)
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
    ui->connectionTypeIcon->setPixmap(pixmap);
    ui->connectionTypeIcon->setAlignment(Qt::AlignCenter);
    ui->connectionTypeIcon->setVisible(true);
}

void TrayItemWidget::setWiredStatusIcon()
{
    // TODO:图标跟随网络状态改变
    QString svgPath = ":/kcp-network-images/wired-connection.svg";
    QPixmap pixmap = getPixmapFromSvg(svgPath);
    ui->connectionTypeIcon->setPixmap(pixmap);
    ui->connectionTypeIcon->setAlignment(Qt::AlignCenter);
    ui->connectionTypeIcon->setVisible(true);
}

void TrayItemWidget::setOtherNetworkIcon()
{
    QString svgPath = ":/kcp-network-images/wireless-other-network.svg";
    QPixmap pixmap = getPixmapFromSvg(svgPath);
    ui->connectionTypeIcon->setPixmap(pixmap);
    ui->connectionTypeIcon->setAlignment(Qt::AlignCenter);
    ui->connectionTypeIcon->setVisible(true);
}

void TrayItemWidget::activatedStatus()
{
    ui->inputTextWidget->setVisible(false);
    ui->activeStatusWidget->setVisible(true);
    ui->cancelAndConnect->setVisible(false);

    ui->ignoreAnddisconnect->setVisible(true);
    ui->ignoreAnddisconnect->layout()->setAlignment(Qt::AlignRight);
    ui->disconnectButton->setVisible(true);
    ui->ignoreButton->setVisible(false);
    ui->connectionStatus->setText(tr("Connected"));
}

void TrayItemWidget::deactivateStatus()
{
    simpleStatus();
    setLoadingStatus(false);
}

void TrayItemWidget::prepareConnectStatus()
{
    ui->connectionStatus->setText(tr("Unconnected"));
    ui->activeStatusWidget->setVisible(true);
    ui->cancelAndConnect->setVisible(true);
    ui->ignoreAnddisconnect->setVisible(false);
    ui->inputTextWidget->setVisible(false);
}

void TrayItemWidget::activatingStatus()
{
    prepareConnectStatus();
    setLoadingStatus(true);
//    setLoadingLabelVisible(true);
    setEnableConnectButton(false);
}

void TrayItemWidget::setLoadingStatus(bool isLoading)
{
//    ui->loadingLabel->setLoadingStatus(isLoading);
    ui->connectionStatus->setLoadingStatus(isLoading);
}

//void TrayItemWidget::setLoadingLabelVisible(bool isVisible)
//{
//    ui->loadingLabel->setVisible(isVisible);
//}

void TrayItemWidget::setConnectionStatusLabel()
{

}

void TrayItemWidget::setIgnoreButtonVisible(bool isVisible)
{
    ui->ignoreButton->setVisible(isVisible);
}

QPixmap TrayItemWidget::getPixmapFromSvg(const QString &svgPath)
{
    QSvgRenderer svgRenderer(QString(svgPath), this);
    QPixmap pixmap(QSize(16, 16));
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    svgRenderer.render(&painter);
    return pixmap;
}

void TrayItemWidget::showInputPasswordWidget()
{
    ui->activeStatusWidget->setVisible(false);
    ui->inputTextWidget->setVisible(true);
    ui->inputTextEdit->setEchoMode(QLineEdit::Password);
    ui->inputTextEdit->setPlaceholderText(tr("Please input password"));
    ui->inputTextConnectButton->setEnabled(true);
}

void TrayItemWidget::showInputSsidWidget()
{
    ui->activeStatusWidget->setVisible(false);
    ui->inputTextWidget->setVisible(true);
    ui->inputTextEdit->setEchoMode(QLineEdit::Normal);
    ui->inputTextEdit->setPlaceholderText(tr("Please input a network name"));
    ui->inputTextConnectButton->setEnabled(true);
}
void TrayItemWidget::handleInputText()
{
    QString text = ui->inputTextEdit->text();
    if(!text.isEmpty())
    {
        ui->inputTextConnectButton->setEnabled(false);
        if(ui->inputTextEdit->echoMode() == QLineEdit::Password)
            emit sendPassword(text);
        else
            emit sendSsid(text);
        ui->inputTextEdit->clear();
    }
}

QString TrayItemWidget::getPassword()
{
    return ui->inputTextEdit->text();
}

void TrayItemWidget::setIgnoreAndDisconnectButtonVisible(bool isVisible)
{
    ui->ignoreAnddisconnect->setVisible(isVisible);
}

void TrayItemWidget::setCancelAndConnectButtonVisible(bool isVisible)
{
    ui->cancelAndConnect->setVisible(isVisible);
}

void TrayItemWidget::simpleStatus()
{
    ui->activeStatusWidget->setVisible(false);
    ui->inputTextWidget->setVisible(false);
    ui->inputTextEdit->clear();
}

void TrayItemWidget::setEnableConnectButton(bool enable)
{
    ui->connectButton->setEnabled(enable);
}

TrayItemWidgetStatus TrayItemWidget::itemWidgetStatus()
{
    return m_currentItemWidgetStatus;
}

