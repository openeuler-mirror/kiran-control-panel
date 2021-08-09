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

#include "system-information-widget.h"
#include "ui_system-information-widget.h"
#include "system-info-dbus.h"
#include "license/active-guide-widget.h"
#include "change-host-name-widget.h"
#include <kylin-license/license_i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <kiranwidgets-qt5/kiran-style-public-define.h>
#include "license/license-agreement.h"

#include <kiran-log/qt5-log-i.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFont>
#include <QPainter>

#define SYSTEM_LOGO     "KylinSec OS"

#define HOST_NAME        "host_name"
#define ARCH             "arch"
#define KERNEL_VERSION   "kernel_version"
#define KERNEL_NAME      "kernal_name"
#define KERNEL_RELEASE   "kernel_release"
#define PRODUCT_RELEASE  "product_release"
#define EXPIRED_TIME     "expired_time"
#define START_TIME       "start_time"
#define LICENSE_STATUS   "license_status"
#define LICENSE_CODE     "license_code"
#define MACHINE_CODE     "machine_code"
#define REGISTER_TIME    "register_time"
#define REGISTER_TYPE    "register_type"
#define INSTALL_TYPE     "install_type"
using namespace Kiran::WidgetPropertyHelper;

SystemInformationWidget::SystemInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemInformationWidget),
    activeGuide(nullptr),
    licenseInfoWidget(nullptr),
    hostNameWidget(nullptr),
    licenseAgreement(nullptr)
{
    ui->setupUi(this);
    initUI();

    connect(ui->btn_change_name, SIGNAL(clicked()), this, SLOT(onBtnchangeHostName()));
    connect(ui->btn_status, SIGNAL(clicked()), this, SLOT(onBtnStatusClicked()));
}

SystemInformationWidget::~SystemInformationWidget()
{
    delete ui;
    if(activeGuide!=nullptr)
    {
        delete activeGuide;
    }
    if(licenseInfoWidget!=nullptr)
    {
        delete licenseInfoWidget;
    }
    if(hostNameWidget != nullptr)
    {
        delete hostNameWidget;
    }
    if(licenseAgreement != nullptr)
    {
        delete licenseAgreement;
    }
}

bool SystemInformationWidget::initUI()
{
    setMinimumWidth(400);
    readSystemInfo(0);
    readLicenseInfo();

    ui->btn_change_name->setText(tr("Change"));
    ui->lab_contact_info->setText("400-625-6606");
    ui->btn_EULA->setText(tr("Show"));
    ui->btn_version_license->setText(tr("Show"));

    connect(ui->btn_EULA,&QPushButton::clicked,
            [this]{
        if(licenseAgreement == nullptr)
        {
            licenseAgreement = new LicenseAgreement();
        }
        licenseAgreement->setEULA();
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        licenseAgreement->move(screenGeometry.x()+(screenGeometry.width()-this->width())/2,
               screenGeometry.y()+(screenGeometry.height()-this->height())/2);
        licenseAgreement->show();
    });

    connect(ui->btn_version_license,&QPushButton::clicked,
            [this]{
        if(licenseAgreement == nullptr)
        {
            licenseAgreement = new LicenseAgreement();
        }
        licenseAgreement->setVersionLicnese();
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        licenseAgreement->move(screenGeometry.x()+(screenGeometry.width()-this->width())/2,
               screenGeometry.y()+(screenGeometry.height()-this->height())/2);
        licenseAgreement->show();
    });

    return true;
}

bool SystemInformationWidget::hasUnsavedOptions()
{
    if((activeGuide!= nullptr && activeGuide->getLineEditStatus()) ||
        (hostNameWidget != nullptr && hostNameWidget->getLineEditStatus()))
    {
        return true;
    }
    else
        return false;
}

/**
 * @brief SystemInformationWidget::readSystemInfo:读取系统信息
 * @param infoType: 传入DBUS接口的参数，0：获取系统信息，1：获取硬件信息
 */
bool SystemInformationWidget::readSystemInfo(int infoType)
{
    QString systemInfo;
    if(!InfoDbus::SystemInfo::getSystemInfo(infoType , systemInfo))
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
 * @brief SystemInformationWidget::readLicenseInfo:读取系统授权信息
 */
bool SystemInformationWidget::readLicenseInfo()
{
    QString licenseInfo;
    if(!InfoDbus::KylinLicense::getLicenseJson(licenseInfo))
    {
        KLOG_DEBUG() << "get activation information failed";
        ui->lab_expire_date_info->setText(tr("Unknow"));
        ui->lab_status->setText(tr("Can't get activation information"));
        ui->lab_status->setStyleSheet("QLabel#lab_status {color:#ff3838;}");
        ui->btn_status->hide();
        ui->lab_install_time_info->setText(tr("Unknow"));
        ui->lab_expire_date_info->setText(tr("Unknow"));
        lc_code = "NULL";
        return false;
    }
    else
    {
        ///解析后端传入的授权信息Json字符串
        KLOG_INFO() << licenseInfo;
        getJsonValueFromString(licenseInfo);
        ///在界面上显示安装时间
        QDateTime Starttime = QDateTime::fromTime_t(start_time);
        installTime  = Starttime.toString("yyyy-MM-dd");
        KLOG_INFO() << "install time = " <<installTime;
        ui->lab_install_time_info->setText(installTime);

        KLOG_INFO() <<"activation = "<<license_status;
        ///给激活状态赋值
        isActive = license_status;
        switch (license_status) {
        case KylinSec::LicenseStatus::LICENSE_STATUS_UNREGISTERD:
        {
            /*未激活*/
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#ff3838;}");
            ui->btn_status->setText(tr("Activate"));

            int status;
            if(InfoDbus::KylinLicense::getServiceStatus(status))
            {
                switch (status) {
                case KylinSec::LicenseServiceStatus::LICENSE_SERVICE_STATUS_INVALID:
                {
                    ui->lab_status->setText((QString("%1 (%2: %3)")).arg(tr("The current time is illegal")).
                                            arg(tr("Less than the installation time")).
                                            arg(installTime));
                    break;
                }
                case KylinSec::LicenseServiceStatus::LICENSE_SERVICE_STATUS_EXPIRED:
                case KylinSec::LicenseServiceStatus::LICENSE_SERVICE_STATUS_UNEXPIRED:
                {
                    QDateTime time = QDateTime::fromTime_t(expired_time);
                    QString dueTime  = time.toString("yyyy-MM-dd");
                    KLOG_INFO() << "due time = " <<dueTime ;

                    ui->lab_status->setText(QString("%1%2").arg(tr("Not activated. Trail expiration: ")).
                                                            arg(dueTime));
                }
                default:
                    break;
                }
            }
            else
            {
                ui->lab_status->setText(tr("get service status failed"));
                ui->btn_status->hide();
            }
            ui->lab_expire_date_info->setText(tr("Not yet"));
            break;
        }
        case KylinSec::LicenseStatus::LICENSE_STATUS_REGISTERD:
        {
            /*已激活*/
            ui->btn_status->show();
            ui->lab_status->setText(tr("Activated"));
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#5ab940;}");

            QDateTime time = QDateTime::fromTime_t(expired_time);
            QString dueTime  = time.toString("yyyy-MM-dd");
	    QDate expiredTime = QDate::fromString(dueTime,"yyyy-MM-dd");
            KLOG_INFO() << "due time = " <<dueTime <<
                           "year: " << expiredTime.year() <<
                           "mouth: " << expiredTime.month() <<
                           "day: " << expiredTime.day();
            ui->lab_expire_date_info->setText(dueTime);

	    QDate currentDate =  QDate::currentDate();
            KLOG_INFO() << "current time = "<<
                           "year: " << currentDate.year() <<
                           "mouth: " << currentDate.month() <<
                           "day: " << currentDate.day();

	    if(currentDate.daysTo(expiredTime) < 0)
            {
                ui->btn_status->setText(tr("Activate"));
                isActive = false;
                return true;
            }
            ui->btn_status->setFixedSize(QSize(16,16));

            ui->btn_status->setText(NULL);
            ui->btn_status->setIcon(QIcon(":/images/license_info.svg"));
            ui->btn_status->setStyleSheet( "QToolButton#btn_status{background: transparent;}" );
            break;
        }
        default:
            break;
        }

    }
    return true;
}

/**
 * @brief SystemInformationWidget::getJsonValueFromString: 解析从DBUS后端获取的Json字符串
 * @param jsonString: DBUS后端获取的Json字符串
 */
void SystemInformationWidget::getJsonValueFromString(QString jsonString)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(),&jsonError);
    if( jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError ){
        KLOG_DEBUG()<< " please check the activation information string "<< jsonString.toLocal8Bit().data();
        return;
    }
    if(jsonDocument.isObject())
    {
       QJsonObject obj = jsonDocument.object();
       if(obj.contains(HOST_NAME))
       {
           QJsonValue value = obj.take(HOST_NAME);
           if(value.isString())
           {
               QString hostName = value.toString();
               ui->lab_name_info->setText(hostName);
           }
       }
       if(obj.contains(ARCH))
       {
           QJsonValue value = obj.take(ARCH);
           if(value.isString())
           {
               QString arch = value.toString();
               ui->lab_system_arch_info->setText(arch);
           }
       }
       if(obj.contains(PRODUCT_RELEASE))
       {
           QJsonValue value = obj.take(PRODUCT_RELEASE);
           if(value.isString())
           {
               QString system_vresion = value.toString();
               ui->lab_system_version_info->setText(system_vresion);
           }
       }
       if(obj.contains(KERNEL_NAME) && obj.contains(KERNEL_RELEASE))
       {
           QJsonValue value_name = obj.take(KERNEL_NAME);
           QJsonValue value_release = obj.take(KERNEL_RELEASE);
           if(value_name.isString() && value_release.isString())
           {
               QString kernel_version = value_name.toString() + " " + value_release.toString();
               ui->lab_core_version_info->setText(kernel_version);
           }
       }
       if(obj.contains(LICENSE_STATUS))
       {
           QJsonValue value = obj.take(LICENSE_STATUS);
           if(value.isDouble())
           {
               license_status = value.toVariant().toInt();
               KLOG_INFO() << "activation status :" <<license_status;
           }
       }
       if(obj.contains(LICENSE_CODE))
       {
           QJsonValue value = obj.take(LICENSE_CODE);
           if(value.isString())
           {
               lc_code = value.toVariant().toString();
               KLOG_INFO() << "lc_code :" << lc_code;
           }
       }
       if(obj.contains(MACHINE_CODE))
       {
           QJsonValue value = obj.take(MACHINE_CODE);
           if(value.isString())
           {
               mc_code = value.toVariant().toString();
               KLOG_INFO() << "mc_code :" << mc_code;
           }
       }
       if(obj.contains(EXPIRED_TIME))
       {
           QJsonValue value = obj.take(EXPIRED_TIME);
           if(value.isDouble())
           {
               expired_time = value.toVariant().toInt();
               KLOG_INFO() << "expired_time :" <<expired_time;
           }
       }
       if(obj.contains(START_TIME))
       {
           QJsonValue value = obj.take(START_TIME);
           if(value.isDouble())
           {
               start_time = value.toVariant().toInt();
               KLOG_INFO() << "start time :" << start_time;
           }
       }
    }
}

/**
 * @brief SystemInformationWidget::onBtnchangeHostName: 当点击更改用户名后的槽函数
 */
void SystemInformationWidget::onBtnchangeHostName()
{
    if(hostNameWidget == nullptr)
    {
        hostNameWidget = new ChangeHostNameWidget;
    }
    hostNameWidget->setAttribute(Qt::WA_QuitOnClose,false);
    hostNameWidget->installEventFilter(this);
    connect(hostNameWidget, SIGNAL(sigChangeNameSuccessful(bool,QString)), this, SLOT(updateHostName(bool,QString)));
    hostNameWidget->raise();
    hostNameWidget->show();
}

/**
 * @brief  槽函数，用户点击授权状态按钮，
 *         未激活，则弹出激活向导窗口
 *         已激活，则弹出授权信息窗口
 */
void SystemInformationWidget::onBtnStatusClicked()
{
    if(!isActive)  //popup Active Guide
    {
        if(activeGuide==nullptr)
        {
            activeGuide = new ActGuideWidget(this);
        }
        activeGuide->setAttribute(Qt::WA_QuitOnClose,false);
        activeGuide->installEventFilter(this);
        connect(activeGuide, SIGNAL(systemIsActived(bool)) , this ,SLOT(updateLicenseInfo(bool)));
        activeGuide->raise();
        activeGuide->show();
    }
    else //popup informations
    {
        if(licenseInfoWidget == nullptr)
        {
            licenseInfoWidget = new LicenseInfoWidget(mc_code ,lc_code,this);
        }

        licenseInfoWidget->setAttribute(Qt::WA_QuitOnClose,false);
        licenseInfoWidget->installEventFilter(this);
        licenseInfoWidget->setLicenseCode(lc_code);
        licenseInfoWidget->setMachineCode(mc_code);
        licenseInfoWidget->raise();

        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        licenseInfoWidget->move(screenGeometry.x()+(screenGeometry.width()-licenseInfoWidget->width())/2,
               screenGeometry.y()+(screenGeometry.height()-licenseInfoWidget->height())/2);

        licenseInfoWidget->show();
    }

}

/**
 * @brief  接收到激活向导窗口发出的激活状态信号，更新当前窗口的激活状态信息
 * @param  isregister 是否激活成功
 */
void SystemInformationWidget::updateLicenseInfo(bool isregister)
{
    KLOG_INFO() << "updateActivationInformation\n";
    if(isregister)
    {
        QString licenseInfo;
        if(!InfoDbus::KylinLicense::getLicenseJson(licenseInfo))
        {
            KLOG_DEBUG() << "get activation information failed";
            ui->lab_expire_date_info->setText(tr("Unknow"));
            ui->lab_status->setText(tr("Can't get activation information"));
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#ff3838;}");
            ui->btn_status->hide();
            ui->lab_install_time_info->setText(tr("Unknow"));
            ui->lab_expire_date_info->setText(tr("Unknow"));

            lc_code = "NULL";
            return;
        }
        else
        {
            //解析后端传入的授权信息Json字符串
            getJsonValueFromString(licenseInfo);
            KLOG_INFO() <<"activation = "<<license_status;

            //给激活状态赋值
            isActive = license_status;

            ui->lab_status->setText(tr("Activated"));
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#5ab940;}");
            ui->btn_status->setText(NULL);
            ui->btn_status->setIcon(QIcon(":/images/license_info.svg"));
            ui->btn_status->setStyleSheet( "QToolButton#btn_status{background: transparent;border: none;}" );
            ui->btn_status->setFixedSize(QSize(16,16));

            QDateTime time = QDateTime::fromTime_t(expired_time);
            QString dueTime  = time.toString("yyyy-MM-dd");
            KLOG_INFO() << "due time = " <<dueTime;
            ui->lab_expire_date_info->setText(dueTime);
        }
    }

}

void SystemInformationWidget::updateHostName(bool isChanged, QString name)
{
    if(isChanged)
    {
        KLOG_INFO() <<"new host name is" <<  name;
        ui->lab_name_info->setText(name);
    }
    else
    {
        return;
    }
}

#include <iostream>
/**
 * @brief  事件监听，当收到激活向导窗口或者授权信息窗口的关闭事件时，释放窗口内存
 * @param  obj  事件对象
 * @param  obj  事件
 * @return 是否过滤
 */
bool SystemInformationWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj==activeGuide&&event->type()==QEvent::Close)
    {
        activeGuide->deleteLater();
        activeGuide = nullptr;
    }
    else if(obj == licenseInfoWidget && event->type()==QEvent::Close)
    {
        licenseInfoWidget->deleteLater();
        licenseInfoWidget = nullptr;
    }
    else if(obj == hostNameWidget && event->type() == QEvent::Close)
    {
        hostNameWidget->deleteLater();
        hostNameWidget = nullptr;
    }
    return false;
}

void SystemInformationWidget::paintEvent(QPaintEvent *painEvent)
{
    QDate currentDate = QDate::currentDate();
    QString date = currentDate.toString("yyyy-MM-dd");
    QString year = date.left(4);
    QString copyright = QString(tr("Copyright ©"))+ QString("%1 ").arg(year)+QString(tr("KylinSec. All rights reserved."));

    QPainter painter(this);
    QFont font = QFont("Noto Sans CJK SC regular",46);
    QRect drawRecLogo = QRect(this->geometry().x()+24,this->geometry().y()+16,this->width(),ui->widget_logo->height()-16);

    painter.setPen(QColor(46,179,255));  //#2eb3FF
    painter.setFont(font);
    painter.drawText(drawRecLogo, SYSTEM_LOGO);

    QFontMetrics fm = painter.fontMetrics();
    int heightText = fm.height();

    int offsetHeight = heightText+5+16;
    QRect drawRecCopyright = QRect(24,this->geometry().y()+offsetHeight, this->width(),ui->widget_logo->height()-offsetHeight);
    font.setPointSize(10);
    font.setWeight(QFont::Normal);
    painter.setPen(QColor(145,145,145));
    painter.setFont(font);
    painter.drawText(drawRecCopyright,copyright);
}

QSize SystemInformationWidget::sizeHint() const
{
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);

    QSize size;
    if(screenGeometry.height() >= 815 && screenGeometry.width() >= 800 ) //能显示全
    {
        size = QSize(800,815);
    }
    else if (screenGeometry.height() >= this->height() && screenGeometry.width() >= this->width())
    {
        size = QSize(this->width(),this->height());
    }
    else
    {
        size = QSize(screenGeometry.width() , screenGeometry.height());
    }

    return size;
}

