#include "license-information.h"
#include <kiran-log/qt5-log-i.h>
#include <kylin-license/license_i.h>
#include <QDateTime>
#include <QDesktopWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "system-info-dbus.h"
#include "ui_license-information.h"

#define EXPIRED_TIME "expired_time"
#define START_TIME "start_time"
#define LICENSE_STATUS "license_status"
#define LICENSE_CODE "license_code"
#define MACHINE_CODE "machine_code"
#define REGISTER_TIME "register_time"
#define REGISTER_TYPE "register_type"
#define INSTALL_TYPE "install_type"

LicenseInformation::LicenseInformation(QWidget *parent) : QWidget(parent),
                                                          m_activeGuide(nullptr),
                                                          m_licenseInfoWidget(nullptr),
                                                          ui(new Ui::LicenseInformation)
{
    ui->setupUi(this);
    readLicenseInfo();
    ui->lab_contact_info->setText("400-625-6606");
    connect(ui->btn_status, SIGNAL(clicked()), this, SLOT(onBtnStatusClicked()));
}

LicenseInformation::~LicenseInformation()
{
    delete ui;
    if (m_activeGuide != nullptr)
        delete m_activeGuide;
    if (m_licenseInfoWidget != nullptr)
        delete m_licenseInfoWidget;
}

/**
 * @brief SystemInformationWidget::readLicenseInfo:读取系统授权信息
 */
bool LicenseInformation::readLicenseInfo()
{
    QString licenseInfo;
    if (!InfoDbus::KylinLicense::getLicenseJson(licenseInfo))
    {
        KLOG_DEBUG() << "get activation information failed";
        ui->lab_expire_date_info->setText(tr("Unknow"));
        ui->lab_status->setText(tr("Can't get activation information"));
        ui->lab_status->setStyleSheet("QLabel#lab_status {color:#ff3838;}");
        ui->btn_status->hide();
        ui->lab_install_time_info->setText(tr("Unknow"));
        ui->lab_expire_date_info->setText(tr("Unknow"));
        m_lcCode = "NULL";
        return false;
    }
    else
    {
        ///解析后端传入的授权信息Json字符串
        KLOG_INFO() << licenseInfo;
        getJsonValueFromString(licenseInfo);
        ///在界面上显示安装时间
        QDateTime Starttime = QDateTime::fromSecsSinceEpoch(m_startTime);
        m_installTime = Starttime.toString("yyyy-MM-dd");
        KLOG_INFO() << "install time = " << m_installTime;
        ui->lab_install_time_info->setText(m_installTime);

        KLOG_INFO() << "activation = " << m_licenseStatus;
        ///给激活状态赋值
        m_isActive = m_licenseStatus;
        switch (m_licenseStatus)
        {
        case KylinSec::LicenseStatus::LICENSE_STATUS_UNREGISTERD:
        {
            /*未激活*/
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#ff3838;}");
            ui->btn_status->setText(tr("Activate"));

            int status;
            if (InfoDbus::KylinLicense::getServiceStatus(status))
            {
                switch (status)
                {
                case KylinSec::LicenseServiceStatus::LICENSE_SERVICE_STATUS_INVALID:
                {
                    ui->lab_status->setText((QString("%1 (%2: %3)")).arg(tr("The current time is illegal")).arg(tr("Less than the installation time")).arg(m_installTime));
                    break;
                }
                case KylinSec::LicenseServiceStatus::LICENSE_SERVICE_STATUS_EXPIRED:
                case KylinSec::LicenseServiceStatus::LICENSE_SERVICE_STATUS_UNEXPIRED:
                {
                    QDateTime time = QDateTime::fromSecsSinceEpoch(m_expiredTime);
                    QString dueTime = time.toString("yyyy-MM-dd");
                    KLOG_INFO() << "due time = " << dueTime;

                    ui->lab_status->setText(QString("%1%2").arg(tr("Not activated. Trail expiration: ")).arg(dueTime));
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

            QDateTime time = QDateTime::fromSecsSinceEpoch(m_expiredTime);
            QString dueTime = time.toString("yyyy-MM-dd");
            QDate expiredTime = QDate::fromString(dueTime, "yyyy-MM-dd");
            KLOG_INFO() << "due time = " << dueTime << "year: " << expiredTime.year() << "mouth: " << expiredTime.month() << "day: " << expiredTime.day();

            ui->lab_expire_date_info->setText(dueTime);

            QDate currentDate = QDate::currentDate();
            KLOG_INFO() << "current time = "
                        << "year: " << currentDate.year() << "mouth: " << currentDate.month() << "day: " << currentDate.day();

            if (currentDate.daysTo(expiredTime) < 0)
            {
                ui->btn_status->setText(tr("Activate"));
                m_isActive = false;
                return true;
            }

            if (expiredTime.year() >= 2250)
                ui->lab_expire_date_info->setText(tr("Forever"));

            ui->btn_status->setFixedSize(QSize(16, 16));

            ui->btn_status->setText(NULL);
            ui->btn_status->setIcon(QIcon(":/images/license_info.svg"));
            ui->btn_status->setStyleSheet("QToolButton#btn_status{background: transparent;}");
            break;
        }
        default:
            break;
        }
    }
    return true;
}

void LicenseInformation::getJsonValueFromString(QString jsonString)
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
        if (obj.contains(LICENSE_STATUS))
        {
            QJsonValue value = obj.take(LICENSE_STATUS);
            if (value.isDouble())
            {
                m_licenseStatus = value.toVariant().toInt();
                KLOG_INFO() << "activation status :" << m_licenseStatus;
            }
        }
        if (obj.contains(LICENSE_CODE))
        {
            QJsonValue value = obj.take(LICENSE_CODE);
            if (value.isString())
            {
                m_lcCode = value.toVariant().toString();
                KLOG_INFO() << "lc_code :" << m_lcCode;
            }
        }
        if (obj.contains(MACHINE_CODE))
        {
            QJsonValue value = obj.take(MACHINE_CODE);
            if (value.isString())
            {
                m_mcCode = value.toVariant().toString();
                KLOG_INFO() << "mc_code :" << m_mcCode;
            }
        }
        if (obj.contains(EXPIRED_TIME))
        {
            QJsonValue value = obj.take(EXPIRED_TIME);
            if (value.isDouble())
            {
                m_expiredTime = value.toVariant().toInt();
                KLOG_INFO() << "expired_time :" << m_expiredTime;
            }
        }
        if (obj.contains(START_TIME))
        {
            QJsonValue value = obj.take(START_TIME);
            if (value.isDouble())
            {
                m_startTime = value.toVariant().toInt();
                KLOG_INFO() << "start time :" << m_startTime;
            }
        }
    }
}

/**
 * @brief  槽函数，用户点击授权状态按钮，
 *         未激活，则弹出激活向导窗口
 *         已激活，则弹出授权信息窗口
 */
void LicenseInformation::onBtnStatusClicked()
{
    if (!m_isActive)  //popup Active Guide
    {
        if (m_activeGuide == nullptr)
        {
            m_activeGuide = new ActGuideWidget(this);
        }
        m_activeGuide->setAttribute(Qt::WA_QuitOnClose, false);
        m_activeGuide->installEventFilter(this);
        connect(m_activeGuide, SIGNAL(systemIsActived(bool)), this, SLOT(updateLicenseInfo(bool)));
        m_activeGuide->raise();
        m_activeGuide->show();
    }
    else  //popup informations
    {
        if (m_licenseInfoWidget == nullptr)
        {
            m_licenseInfoWidget = new LicenseInfoWidget(m_mcCode, m_lcCode, this);
        }

        m_licenseInfoWidget->setAttribute(Qt::WA_QuitOnClose, false);
        m_licenseInfoWidget->installEventFilter(this);
        m_licenseInfoWidget->setLicenseCode(m_lcCode);
        m_licenseInfoWidget->setMachineCode(m_mcCode);
        m_licenseInfoWidget->raise();

        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_licenseInfoWidget->move(screenGeometry.x() + (screenGeometry.width() - m_licenseInfoWidget->width()) / 2,
                                  screenGeometry.y() + (screenGeometry.height() - m_licenseInfoWidget->height()) / 2);

        m_licenseInfoWidget->show();
    }
}

/**
 * @brief  接收到激活向导窗口发出的激活状态信号，更新当前窗口的激活状态信息
 * @param  isregister 是否激活成功
 */
void LicenseInformation::updateLicenseInfo(bool isregister)
{
    KLOG_INFO() << "updateActivationInformation\n";
    if (isregister)
    {
        QString licenseInfo;
        if (!InfoDbus::KylinLicense::getLicenseJson(licenseInfo))
        {
            KLOG_DEBUG() << "get activation information failed";
            ui->lab_expire_date_info->setText(tr("Unknow"));
            ui->lab_status->setText(tr("Can't get activation information"));
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#ff3838;}");
            ui->btn_status->hide();
            ui->lab_install_time_info->setText(tr("Unknow"));
            ui->lab_expire_date_info->setText(tr("Unknow"));

            m_lcCode = "NULL";
            return;
        }
        else
        {
            //解析后端传入的授权信息Json字符串
            getJsonValueFromString(licenseInfo);
            KLOG_INFO() << "activation = " << m_licenseStatus;

            //给激活状态赋值
            m_isActive = m_licenseStatus;

            ui->lab_status->setText(tr("Activated"));
            ui->lab_status->setStyleSheet("QLabel#lab_status {color:#5ab940;}");
            ui->btn_status->setText(NULL);
            ui->btn_status->setIcon(QIcon(":/images/license_info.svg"));
            ui->btn_status->setStyleSheet("QToolButton#btn_status{background: transparent;border: none;}");
            ui->btn_status->setFixedSize(QSize(16, 16));

            QDateTime time = QDateTime::fromSecsSinceEpoch(m_expiredTime);
            QString dueTime = time.toString("yyyy-MM-dd");
            KLOG_INFO() << "due time = " << dueTime;

            QString year = dueTime.split("-").first();
            if (year.toInt() >= 2250)
                ui->lab_expire_date_info->setText(tr("Forever"));
            else
                ui->lab_expire_date_info->setText(dueTime);
        }
    }
}

bool LicenseInformation::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_activeGuide && event->type() == QEvent::Close)
    {
        m_activeGuide->deleteLater();
        m_activeGuide = nullptr;
    }
    else if (obj == m_licenseInfoWidget && event->type() == QEvent::Close)
    {
        m_licenseInfoWidget->deleteLater();
        m_licenseInfoWidget = nullptr;
    }
    return false;
}
