/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 * PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
 * Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "system-information.h"
#include "dbus-wrapper/system-info-dbus.h"
#include "license-agreement.h"
#include "ui_system-information.h"

#include <kiranwidgets-qt5/kiran-message-box.h>
#include <kiranwidgets-qt5/kiran-style-public-define.h>
#include <kiranwidgets-qt5/widget-property-helper.h>

#include <kiran-log/qt5-log-i.h>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPainter>

#define HOST_NAME "host_name"
#define ARCH "arch"
#define KERNEL_VERSION "kernel_version"
#define KERNEL_NAME "kernal_name"
#define KERNEL_RELEASE "kernel_release"
#define PRODUCT_RELEASE "product_release"

using namespace Kiran::WidgetPropertyHelper;

SystemInformation::SystemInformation(QWidget *parent)
    : QWidget(parent), ui(new Ui::SystemInformation), hostNameWidget(nullptr), licenseAgreement(nullptr)
{
    ui->setupUi(this);
    init();
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

void SystemInformation::init()
{
    setMinimumHeight(400);
    initUI();
    // clang-format off
    connect(ui->btn_EULA, &QPushButton::clicked, [this]
    {
        if (licenseAgreement == nullptr)
        {
            licenseAgreement = new LicenseAgreement(this);
        }
        licenseAgreement->setEULA();
        licenseAgreement->show();
    });
    connect(ui->btn_version_license, &QPushButton::clicked, [this]
    {
        if (licenseAgreement == nullptr) {
            licenseAgreement = new LicenseAgreement(this);
        }
        licenseAgreement->setVersionLicnese();
        licenseAgreement->show();
    });
    // clang-format on
    connect(ui->btn_change_name, &QPushButton::clicked, this, &SystemInformation::handleChangeHostName);
}

bool SystemInformation::initUI()
{
    QString systemInfoJson;
    bool bRes = SystemInfoDBus::getSystemInfo(SYSTEMINFO_TYPE_SOFTWARE, systemInfoJson);
    if (!bRes)
    {
        ui->lab_name_info->setText(tr("Unknow"));
        ui->lab_core_version_info->setText(tr("Unknow"));
        ui->lab_system_arch_info->setText(tr("Unknow"));
        ui->lab_system_version_info->setText(tr("Unknow"));
        ui->btn_change_name->hide();
    }
    else
    {
        QString hostname,arch,systemVersion,kernelVersion;
        parseSoftwareInfoJson(systemInfoJson,
                              hostname,
                              arch,
                              systemVersion,
                              kernelVersion);
        qInfo() << hostname << arch << systemVersion << kernelVersion;
        ui->lab_name_info->setText(hostname);
        ui->lab_system_arch_info->setText(arch);
        ui->lab_system_version_info->setText(systemVersion);
        ui->lab_core_version_info->setText(kernelVersion);

    }

    QList<QLabel *> labels = {ui->lab_name_info,ui->lab_core_version_info, ui->lab_system_arch_info,ui->lab_system_version_info};
    for (auto label : labels)
    {
        label->setStyleSheet("color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";");
    }

    auto kiranFrames = findChildren<KiranFrame *>();
    for (auto frame : kiranFrames)
    {
        frame->setRadius(6);
        frame->setDrawBroder(false);
    }


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

void SystemInformation::parseSoftwareInfoJson(QString jsonString,
                                              QString& hostName,
                                              QString& arch,
                                              QString& systemVersion,
                                              QString& kernelVersion)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError || !jsonDocument.isObject())
    {
        KLOG_ERROR() << " please check the activation information string " << jsonString.toLocal8Bit().data();
        return;
    }
    qInfo() <<jsonString;
    QJsonObject rootObject = jsonDocument.object();
    if( rootObject.contains("host_name") && rootObject["host_name"].isString() )
    {
        hostName = rootObject["host_name"].toString();
    }
    if( rootObject.contains("arch") && rootObject["arch"].isString() )
    {
        arch = rootObject["arch"].toString();
    }
    if( rootObject.contains("product_release") && rootObject["product_release"].isString() )
    {
        systemVersion = rootObject["product_release"].toString();
    }
    if( rootObject.contains("kernal_name") && rootObject["kernal_name"].isString() &&
        rootObject.contains("kernel_release") && rootObject["kernel_release"].isString() )
    {
        kernelVersion = rootObject["kernal_name"].toString() + " " + rootObject["kernel_release"].toString();
    }
}

/**
 * @brief SystemInformation::onBtnchangeHostName: 当点击更改用户名后的槽函数
 */
void SystemInformation::handleChangeHostName()
{
    if (hostNameWidget == nullptr)
    {
        hostNameWidget = new ChangeHostNameWidget;
    }
    hostNameWidget->setAttribute(Qt::WA_QuitOnClose, false);
    hostNameWidget->installEventFilter(this);
    connect(hostNameWidget, SIGNAL(sigChangeNameSuccessful(bool, QString)), this,
            SLOT(updateHostName(bool, QString)));
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
 * @brief 事件监听，当收到激活向导窗口或者授权信息窗口的关闭事件时，释放窗口内存
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

QSize SystemInformation::sizeHint() const
{
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);

    QSize size;
    if (screenGeometry.height() >= 815 &&
        screenGeometry.width() >= 800)  // 能显示全
    {
        size = QSize(800, 815);
    }
    else if (screenGeometry.height() >= this->height() &&
             screenGeometry.width() >= this->width())
    {
        size = QSize(this->width(), this->height());
    }
    else
    {
        size = QSize(screenGeometry.width(), screenGeometry.height());
    }

    return size;
}
