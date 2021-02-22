/**
 * @file system-information-widget.cpp
 * @brief  获取系统信息，包括授权信息，并显示在界面中，提供用户授权的接口
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "system-information-widget.h"
#include "ui_system-information-widget.h"
#include "system-info-dbus.h"
#include "license/active-guide-widget.h"
#include <kylin-license/license_i.h>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QDesktopWidget>

#define HOST_NAME        "host_name"
#define ARCH             "arch"
#define KERNEL_VERSION   "kernel_version"
#define PRODUCT_RELEASE  "product_release"
#define EXPIRED_TIME     "expired_time"
#define START_TIME       "start_time"
#define LICENSE_STATUS   "license_status"
#define LICENSE_CODE     "license_code"
#define MACHINE_CODE     "machine_code"
#define REGISTER_TIME    "register_time"
#define REGISTER_TYPE    "register_type"
#define INSTALL_TYPE     "install_type"

SystemInformationWidget::SystemInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemInformationWidget),
    activeGuide(nullptr),
    licenseInfoWidget(nullptr)
{
    ui->setupUi(this);
    initUI();
    readSystemInfo(0);
    readLicenseInfo();

    connect(ui->btn_change_name, SIGNAL(clicked()), this, SLOT(changeCurrentHostName()));
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
}

void SystemInformationWidget::initUI()
{
    ui->btn_change_name->setText(tr("Change"));
    ui->btn_status->setText(tr("Active"));
    ui->lab_contact_info->setText("800820820");
}

/**
 * @brief SystemInformationWidget::readSystemInfo:读取系统信息
 * @param infoType: 传入DBUS接口的参数，0：获取系统信息，1：获取硬件信息
 */
void SystemInformationWidget::readSystemInfo(int infoType)
{
    QString systemInfo;
    if(!InfoDbus::SystemInfo::getSystemInfo(infoType , systemInfo))
    {
        qDebug() << "get system information failed"<< endl;
        ui->lab_name_info->setText(tr("Unknow"));
        ui->lab_core_version_info->setText(tr("Unknow"));
        ui->lab_system_arch_info->setText(tr("Unknow"));
        ui->lab_system_version_info->setText(tr("Unknow"));
        ui->btn_change_name->hide();
        return;
    }
    else
    {
        qInfo() << systemInfo << endl;
        getJsonValueFromString(systemInfo);
    }

}

/**
 * @brief SystemInformationWidget::readLicenseInfo:读取系统授权信息
 */
void SystemInformationWidget::readLicenseInfo()
{
    QString licenseInfo;
    if(!InfoDbus::KylinLicense::getLicenseJson(licenseInfo))
    {
        qDebug() << "get license information failed"<< endl;
        ui->lab_expire_date_info->setText(tr("Unknow"));
        ui->lab_status->setText(tr("Can't get license information"));
        ui->lab_status->setStyleSheet("QLabel#lab_status {color:#ff3838}");
        ui->btn_status->hide();
        ui->lab_install_time_info->setText(tr("Unknow"));
        ui->lab_expire_date_info->setText(tr("Unknow"));
        lc_code = "NULL";
        return;
    }
    else
    {
        ///解析后端传入的授权信息Json字符串
        qInfo() << licenseInfo << endl;
        getJsonValueFromString(licenseInfo);
        ///在界面上显示安装时间
        QDateTime Starttime = QDateTime::fromTime_t(start_time);
        installTime  = Starttime.toString("yyyy-MM-dd");
        qInfo() << "install time = " <<installTime <<endl;
        ui->lab_install_time_info->setText(installTime);

        qInfo() <<"licensed = "<<license_status <<endl;
        ///给激活状态赋值
        isActive = license_status;
        switch (license_status) {
        case KylinSec::LicenseStatus::LICENSE_STATUS_UNREGISTERD:
        {
            /*未激活*/
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#ff3838}");
            ui->btn_status->setText(tr("Active"));

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
                    qInfo() << "due time = " <<dueTime <<endl;

                    ui->lab_status->setText(QString("%1%2").arg(tr("Trial Version. Trial expire: ")).
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
            ui->lab_expire_date_info->setText(tr("Unknow"));
            break;
        }
        case KylinSec::LicenseStatus::LICENSE_STATUS_REGISTERD:
        {
            /*已激活*/
            ui->lab_status->setText(tr("Official Version"));
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#5ab940}");

            QDateTime time = QDateTime::fromTime_t(expired_time);
            QString dueTime  = time.toString("yyyy-MM-dd");
            qInfo() << "due time = " <<dueTime <<endl;
            ui->lab_expire_date_info->setText(dueTime);

            QDateTime currentTime = QDateTime::currentDateTime();   //获取当前时间
            uint currentTimeT = currentTime.toTime_t();   //将当前时间转为时间戳
            if(currentTimeT>expired_time)
            {
                ui->btn_status->setText(tr("Active"));
                isActive = false;
                return ;
            }
            ui->btn_status->setFixedSize(QSize(20,20));

            ui->btn_status->setText(NULL);
            ui->btn_status->setIcon(QIcon(":/data/license_info.svg"));
            ui->btn_status->setStyleSheet( "QPushButton#btn_status{background: transparent;}" );
            break;
        }
        default:
            break;
        }

    }
}

void SystemInformationWidget::resizeEvent(QResizeEvent *event)
{
     //qInfo() << "Page width: " << ui->widget_host_name->width() << " Page height: " << ui->widget_host_name->height() << endl;
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
        qDebug()<< " please check the license information string "<< jsonString.toLocal8Bit().data();
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
       if(obj.contains(KERNEL_VERSION))
       {
           QJsonValue value = obj.take(KERNEL_VERSION);
           if(value.isString())
           {
               QString kernel_version = value.toString();
               ui->lab_core_version_info->setText(kernel_version);
           }
       }
       if(obj.contains(LICENSE_STATUS))
       {
           QJsonValue value = obj.take(LICENSE_STATUS);
           if(value.isDouble())
           {
               license_status = value.toVariant().toInt();
               qInfo() << "license status :" <<license_status << endl;
           }
       }
       if(obj.contains(LICENSE_CODE))
       {
           QJsonValue value = obj.take(LICENSE_CODE);
           if(value.isString())
           {
               lc_code = value.toVariant().toString();
               qInfo() << "lc_code :" << lc_code<< endl;
           }
       }
       if(obj.contains(MACHINE_CODE))
       {
           QJsonValue value = obj.take(MACHINE_CODE);
           if(value.isString())
           {
               mc_code = value.toVariant().toString();
               qInfo() << "mc_code :" << mc_code <<endl;
           }
       }
       if(obj.contains(EXPIRED_TIME))
       {
           QJsonValue value = obj.take(EXPIRED_TIME);
           if(value.isDouble())
           {
               expired_time = value.toVariant().toInt();
               qInfo() << "expired_time :" <<expired_time <<endl;
           }
       }
       if(obj.contains(START_TIME))
       {
           QJsonValue value = obj.take(START_TIME);
           if(value.isDouble())
           {
               start_time = value.toVariant().toInt();
               qInfo() << "start time :" << start_time <<endl;
           }
       }
    }
}

/**
 * @brief SystemInformationWidget::changeCurrentHostName: 当点击更改用户名后的槽函数
 */
void SystemInformationWidget::changeCurrentHostName()
{

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
            activeGuide = new ActGuideWidget;
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
            licenseInfoWidget = new LicenseInfoWidget(mc_code ,lc_code);
        }

        licenseInfoWidget->setAttribute(Qt::WA_QuitOnClose,false);
        licenseInfoWidget->installEventFilter(this);
        licenseInfoWidget->setLicenseCode(lc_code);
        licenseInfoWidget->setMachineCode(mc_code);
        licenseInfoWidget->raise();

        licenseInfoWidget->show();
        QDesktopWidget *desktop = QApplication::desktop();
        licenseInfoWidget->move((desktop->width() - licenseInfoWidget->width())/2 , (desktop->height() - licenseInfoWidget->height())/2 );
    }

}

/**
 * @brief  接收到激活向导窗口发出的激活状态信号，更新当前窗口的激活状态信息
 * @param  isregister 是否激活成功
 */
void SystemInformationWidget::updateLicenseInfo(bool isregister)
{
    qInfo("updateLicenseInformation\n");
    if(isregister)
    {
        QString licenseInfo;
        if(!InfoDbus::KylinLicense::getLicenseJson(licenseInfo))
        {
            qDebug() << "get license information failed"<< endl;
            ui->lab_expire_date_info->setText(tr("Unknow"));
            ui->lab_status->setText(tr("Can't get license information"));
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#ff3838}");
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
            qInfo() <<"licensed = "<<license_status <<endl;

            //给激活状态赋值
            isActive = license_status;

            ui->lab_status->setText(tr("Official Version"));
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#5ab940}");
            ui->btn_status->setText(NULL);
            ui->btn_status->setIcon(QIcon(":/images/license_info.svg"));
            ui->btn_status->setStyleSheet( "QPushButton#btn_status{background: transparent;}" );
            ui->btn_status->setFixedSize(QSize(16,16));

            QDateTime time = QDateTime::fromTime_t(expired_time);
            QString dueTime  = time.toString("yyyy-MM-dd");
            qInfo() << "due time = " <<dueTime<<endl;
            ui->lab_expire_date_info->setText(dueTime);
        }
    }

}

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
    return false;
}

