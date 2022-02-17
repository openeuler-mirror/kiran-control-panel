/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "system-information.h"
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <kiranwidgets-qt5/kiran-style-public-define.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <kylin-license/license_i.h>
#include "change-host-name-widget.h"
#include "license/license-agreement.h"
#include "system-info-dbus.h"
#include "ui_system-information.h"

#include <kiran-log/qt5-log-i.h>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPainter>

#define SYSTEM_LOGO "KylinSec OS"

#define HOST_NAME "host_name"
#define ARCH "arch"
#define KERNEL_VERSION "kernel_version"
#define KERNEL_NAME "kernal_name"
#define KERNEL_RELEASE "kernel_release"
#define PRODUCT_RELEASE "product_release"

using namespace Kiran::WidgetPropertyHelper;

SystemInformation::SystemInformation(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::SystemInformation),
                                                        hostNameWidget(nullptr),
                                                        licenseAgreement(nullptr)
{
    ui->setupUi(this);
    initUI();

    connect(ui->btn_change_name, SIGNAL(clicked()), this, SLOT(onBtnchangeHostName()));
}

SystemInformation::~SystemInformation()
{
    delete ui;
    if (hostNameWidget != nullptr)
    {
        delete hostNameWidget;
    }
    if (licenseAgreement != nullptr)
    {
        delete licenseAgreement;
    }
}

bool SystemInformation::initUI()
{
    setMinimumWidth(400);
    readSystemInfo(0);

    ui->btn_change_name->setText(tr("Change"));
    ui->btn_EULA->setText(tr("Show"));
    ui->btn_version_license->setText(tr("Show"));

    connect(ui->btn_EULA, &QPushButton::clicked,
            [this] {
                if (licenseAgreement == nullptr)
                {
                    licenseAgreement = new LicenseAgreement();
                }
                licenseAgreement->setEULA();
                int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
                QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
                licenseAgreement->move(screenGeometry.x() + (screenGeometry.width() - this->width()) / 2,
                                       screenGeometry.y() + (screenGeometry.height() - this->height()) / 2);
                licenseAgreement->show();
            });

    connect(ui->btn_version_license, &QPushButton::clicked,
            [this] {
                if (licenseAgreement == nullptr)
                {
                    licenseAgreement = new LicenseAgreement();
                }
                licenseAgreement->setVersionLicnese();
                int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
                QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
                licenseAgreement->move(screenGeometry.x() + (screenGeometry.width() - this->width()) / 2,
                                       screenGeometry.y() + (screenGeometry.height() - this->height()) / 2);
                licenseAgreement->show();
            });

    return true;
}

bool SystemInformation::hasUnsavedOptions()
{
    if (hostNameWidget != nullptr && hostNameWidget->getLineEditStatus())
    {
        return true;
    }
    else
        return false;
}

/**
 * @brief SystemInformation::readSystemInfo:读取系统信息
 * @param infoType: 传入DBUS接口的参数，0：获取系统信息，1：获取硬件信息
 */
bool SystemInformation::readSystemInfo(int infoType)
{
    QString systemInfo;
    if (!InfoDbus::SystemInfo::getSystemInfo(infoType, systemInfo))
    {
        KLOG_DEBUG() << "get system information failed";
        ui->lab_name_info->setText(tr("Unknow"));
        ui->lab_core_version_info->setText(tr("Unknow"));
        ui->lab_system_arch_info->setText(tr("Unknow"));
        ui->lab_system_version_info->setText(tr("Unknow"));
        ui->btn_change_name->hide();
        return false;
    }
    else
    {
        KLOG_INFO() << systemInfo;
        getJsonValueFromString(systemInfo);
    }
    return true;
}

/**
 * @brief SystemInformation::getJsonValueFromString: 解析从DBUS后端获取的Json字符串
 * @param jsonString: DBUS后端获取的Json字符串
 */
void SystemInformation::getJsonValueFromString(QString jsonString)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        KLOG_DEBUG() << " please check the activation information string " << jsonString.toLocal8Bit().data();
        return;
    }
    if (jsonDocument.isObject())
    {
        QJsonObject obj = jsonDocument.object();
        if (obj.contains(HOST_NAME))
        {
            QJsonValue value = obj.take(HOST_NAME);
            if (value.isString())
            {
                QString hostName = value.toString();
                ui->lab_name_info->setText(hostName);
            }
        }
        if (obj.contains(ARCH))
        {
            QJsonValue value = obj.take(ARCH);
            if (value.isString())
            {
                QString arch = value.toString();
                ui->lab_system_arch_info->setText(arch);
            }
        }
        if (obj.contains(PRODUCT_RELEASE))
        {
            QJsonValue value = obj.take(PRODUCT_RELEASE);
            if (value.isString())
            {
                QString system_vresion = value.toString();
                ui->lab_system_version_info->setText(system_vresion);
            }
        }
        if (obj.contains(KERNEL_NAME) && obj.contains(KERNEL_RELEASE))
        {
            QJsonValue value_name = obj.take(KERNEL_NAME);
            QJsonValue value_release = obj.take(KERNEL_RELEASE);
            if (value_name.isString() && value_release.isString())
            {
                QString kernel_version = value_name.toString() + " " + value_release.toString();
                ui->lab_core_version_info->setText(kernel_version);
            }
        }
    }
}

/**
 * @brief SystemInformation::onBtnchangeHostName: 当点击更改用户名后的槽函数
 */
void SystemInformation::onBtnchangeHostName()
{
    if (hostNameWidget == nullptr)
    {
        hostNameWidget = new ChangeHostNameWidget;
    }
    hostNameWidget->setAttribute(Qt::WA_QuitOnClose, false);
    hostNameWidget->installEventFilter(this);
    connect(hostNameWidget, SIGNAL(sigChangeNameSuccessful(bool, QString)), this, SLOT(updateHostName(bool, QString)));
    hostNameWidget->raise();
    hostNameWidget->show();
}

void SystemInformation::updateHostName(bool isChanged, QString name)
{
    if (isChanged)
    {
        KLOG_INFO() << "new host name is" << name;
        ui->lab_name_info->setText(name);
    }
    else
    {
        return;
    }
}

/**
 * @brief  事件监听，当收到激活向导窗口或者授权信息窗口的关闭事件时，释放窗口内存
 * @param  obj  事件对象
 * @param  obj  事件
 * @return 是否过滤
 */
bool SystemInformation::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == hostNameWidget && event->type() == QEvent::Close)
    {
        hostNameWidget->deleteLater();
        hostNameWidget = nullptr;
    }
    return false;
}

void SystemInformation::paintEvent(QPaintEvent *painEvent)
{
    QDate currentDate = QDate::currentDate();
    QString date = currentDate.toString("yyyy-MM-dd");
    QString year = date.left(4);
    QString copyright = QString(tr("Copyright ©")) + QString("%1 ").arg(year) + QString(tr("KylinSec. All rights reserved."));

    QPainter painter(this);
    QFont font = QFont("Noto Sans CJK SC regular", 46);
    QRect drawRecLogo = QRect(this->geometry().x() + 24, this->geometry().y() + 16, this->width(), ui->widget_logo->height() - 16);

    painter.setPen(QColor(46, 179, 255));  //#2eb3FF
    painter.setFont(font);
    painter.drawText(drawRecLogo, SYSTEM_LOGO);

    QFontMetrics fm = painter.fontMetrics();
    int heightText = fm.height();

    int offsetHeight = heightText + 5 + 16;
    QRect drawRecCopyright = QRect(24, this->geometry().y() + offsetHeight, this->width(), ui->widget_logo->height() - offsetHeight);
    font.setPointSize(10);
    font.setWeight(QFont::Normal);
    painter.setPen(QColor(145, 145, 145));
    painter.setFont(font);
    painter.drawText(drawRecCopyright, copyright);
}

QSize SystemInformation::sizeHint() const
{
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);

    QSize size;
    if (screenGeometry.height() >= 815 && screenGeometry.width() >= 800)  //能显示全
    {
        size = QSize(800, 815);
    }
    else if (screenGeometry.height() >= this->height() && screenGeometry.width() >= this->width())
    {
        size = QSize(this->width(), this->height());
    }
    else
    {
        size = QSize(screenGeometry.width(), screenGeometry.height());
    }

    return size;
}
