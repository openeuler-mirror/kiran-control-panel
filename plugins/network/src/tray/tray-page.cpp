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

#include "tray-page.h"
#include <qt5-log-i.h>
#include "ui_tray-page.h"
#include "wired-tray-widget.h"
#include "wireless-tray-widget.h"
#include "logging-category.h"
using namespace NetworkManager;

TrayPage::TrayPage(Device::List deviceList, QWidget *parent) : QWidget(parent), ui(new Ui::TrayPage)
{
    ui->setupUi(this);
    m_deviceList = deviceList;
    init();
}

TrayPage::~TrayPage()
{
    delete ui;
}

void TrayPage::init()
{
    if (m_deviceList.count() != 0)
    {
        initUI();
        initConnection();
    }
}

// XXX:修改初始化
void TrayPage::initUI()
{
    setFixedWidth(240);
    setMaximumHeight(434);
    ui->stackedWidget->setContentsMargins(0, 0, 0, 0);

    if (m_deviceList.count() > 1)
        setMultiDeviceWidget();
    else if (m_deviceList.count() == 1)
    {
        setSingleDeviceWidget();
    }

    /**
     * NOTE:
     * 此处是修复当存在多个网卡时，stackwidget中当前未显示页面的widget的大小有问题
     * 推测原因在于，在初始化时，在页面未show的状态下，设置widget的尺寸不生效存在问题。
    */
    repolish(this);
}

void TrayPage::repolish(QWidget *w)
{
    QList<const QObject *> children;
    children.reserve(w->children().size() + 1);
    for (auto child: qAsConst(w->children()))
        children.append(child);
    children.append(w);
    updateObjects(children);
}

void TrayPage::updateObjects(const QList<const QObject *>& objects)
{
    QEvent event(QEvent::StyleChange);
    for (const QObject *object : objects) 
    {
        if (auto widget = qobject_cast<QWidget*>(const_cast<QObject*>(object))) 
        {
            widget->style()->polish(widget);
            QCoreApplication::sendEvent(widget, &event);
            QList<const QObject *> children;
            children.reserve(widget->children().size() + 1);
            for (auto child: qAsConst(widget->children()))
                children.append(child);
            updateObjects(children);
        }
    }
}

void TrayPage::initConnection()
{
}

void TrayPage::setMultiDeviceWidget()
{
    for (Device::Ptr dev : m_deviceList)
    {
        QString devicePath = dev->uni();
        QString deviceName = dev->interfaceName();
        Device::Type deviceType = dev->type();
        ui->deviceComboBox->addItem(deviceName, devicePath);

        if (deviceType == Device::Ethernet)
        {
            ui->deviceLabel->setText(tr("Select wired network card"));
            initWiredTrayWidget(devicePath);
        }
        else if (deviceType == Device::Wifi)
        {
            ui->deviceLabel->setText(tr("Select wireless network card"));
            initWirelessTrayWidget(devicePath);
        }
    }
    connect(ui->deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TrayPage::handleDeviceComboBoxChanged);
    ui->selectDevicewidget->setVisible(true);
}

// XXX:单设备时，指定了stackedWidget的高度，多设备时未指定，因为考虑到多个stacked页面高度不一致的情况
void TrayPage::setSingleDeviceWidget()
{
    ui->selectDevicewidget->setVisible(false);
    QString devicePath = m_deviceList.value(0)->uni();
    Device::Type deviceType = m_deviceList.value(0)->type();
    if (deviceType == Device::Ethernet)
    {
        initWiredTrayWidget(devicePath);
    }
    else if (deviceType == Device::Wifi)
    {
        initWirelessTrayWidget(devicePath);
    }
}

void TrayPage::initWiredTrayWidget(const QString &devicePath)
{
    WiredTrayWidget *wiredTrayWidget = new WiredTrayWidget(devicePath, this);
    ui->stackedWidget->addWidget(wiredTrayWidget);
    ui->stackedWidget->setFixedHeight(wiredTrayWidget->getHeight());
    connect(wiredTrayWidget, &WiredTrayWidget::sizeChanged, this, &TrayPage::handleAdjustedTraySize);
}

void TrayPage::initWirelessTrayWidget(const QString &devicePath)
{
    WirelessTrayWidget *wirelessTrayWidget = new WirelessTrayWidget(devicePath, this);
    ui->stackedWidget->addWidget(wirelessTrayWidget);
    ui->stackedWidget->setFixedHeight(wirelessTrayWidget->getHeight());
    connect(wirelessTrayWidget, &WirelessTrayWidget::sizeChanged, this, &TrayPage::handleAdjustedTraySize);
}

void TrayPage::handleDeviceComboBoxChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
    QWidget *widget = ui->stackedWidget->currentWidget();

    auto trayWidget = qobject_cast<TrayWidget *>(widget);
    int height = trayWidget->getHeight();
    ui->stackedWidget->setFixedHeight(height);

    KLOG_DEBUG(qLcNetwork) << "tray widget height:" << height;
    // KLOG_DEBUG(qLcNetwork) << "ui->stackedWidget->size():" << ui->stackedWidget->size();
    // KLOG_DEBUG(qLcNetwork) << " ui->selectDevicewidget->size():" << ui->selectDevicewidget->size();
    // KLOG_DEBUG(qLcNetwork) << "this->size():" << this->size();

    emit sizeChanged(QSize(this->sizeHint().width(), ui->selectDevicewidget->sizeHint().height() + height));
}

QStringList TrayPage::devicePathList()
{
    QStringList devicePathList;
    for (Device::Ptr device : m_deviceList)
    {
        if (device != nullptr)
            devicePathList << device->uni();
    }
    KLOG_DEBUG(qLcNetwork) << "devicePathList:" << devicePathList;
    return devicePathList;
}

void TrayPage::handleAdjustedTraySize(QSize sizeHint)
{
    int height = sizeHint.height();
    ui->stackedWidget->setFixedHeight(height);

    if (ui->selectDevicewidget->isVisible())
        m_size = QSize(this->sizeHint().width(), ui->selectDevicewidget->size().height() + height);
    else
        m_size = QSize(this->sizeHint().width(), height);
       
     emit sizeChanged(m_size);
}

QSize TrayPage::trayPageSize()
{
    return m_size;
}