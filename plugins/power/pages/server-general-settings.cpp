/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#include "server-general-settings.h"
#include "logging-category.h"
#include "ui_server-general-settings.h"

#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <style-palette.h>
#include <QGSettings>

using namespace Kiran;

#define MAX_IDLE_TIME 120

#define MATE_SESSION_SCHEMA_ID "org.mate.session"
#define KIRAN_SESSION_SCHEMA_ID "com.kylinsec.kiran.session-manager"
#define KEY_IDLE_DELAY "idleDelay"
#define DEFAULT_IDLE_DELAY 5

#define SCHEMA_KIRAN_SCREENSAVER "com.kylinsec.kiran.screensaver"
#define KEY_IDLE_ACTIVATION_LOCK "idleActivationLock"
ServerGeneralSettings::ServerGeneralSettings(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::ServerGeneralSettings)
{
    ui->setupUi(this);
    init();
}

ServerGeneralSettings::~ServerGeneralSettings()
{
    delete ui;
}

QSize ServerGeneralSettings::sizeHint() const
{
    return {500, 657};
}

void ServerGeneralSettings::init()
{
    initSessionSettings();
    initScreensaverSettings();
    initUI();
}

void ServerGeneralSettings::initSessionSettings()
{
    if (QGSettings::isSchemaInstalled(KIRAN_SESSION_SCHEMA_ID))
    {
        m_sessionSettings = new QGSettings(KIRAN_SESSION_SCHEMA_ID, QByteArray(), this);
    }
    else if (QGSettings::isSchemaInstalled(MATE_SESSION_SCHEMA_ID))
    {
        m_sessionSettings = new QGSettings(MATE_SESSION_SCHEMA_ID, QByteArray(), this);
    }

    if (!m_sessionSettings)
    {
        KLOG_ERROR(qLcPower) << "can't find session gsettings";
        return;
    }

    connect(m_sessionSettings, &QGSettings::changed,
            this, [this](const QString& key)
            {
            if (key != KEY_IDLE_DELAY)
            {
                return;
            }

            int idleTime = m_sessionSettings->get(KEY_IDLE_DELAY).toInt();
            updateIdleTimeLabel(idleTime);
            QSignalBlocker blocker(ui->slider_idleTime);
            ui->slider_idleTime->setValue(idleTime); });
}

void ServerGeneralSettings::initScreensaverSettings()
{
    if (!QGSettings::isSchemaInstalled(SCHEMA_KIRAN_SCREENSAVER))
    {
        KLOG_ERROR(qLcPower) << SCHEMA_KIRAN_SCREENSAVER << "is not install!";
        return;
    }

    m_screensaverSettings = new QGSettings(SCHEMA_KIRAN_SCREENSAVER, QByteArray(), this);

    connect(m_screensaverSettings, &QGSettings::changed, [this](const QString& key)
            {
            if(key != KEY_IDLE_ACTIVATION_LOCK)
            {
                return ;
            }

            bool value = m_screensaverSettings->get(KEY_IDLE_ACTIVATION_LOCK).toBool();
            m_switchIdlelockScreen->setChecked(value); });
}

void ServerGeneralSettings::initUI()
{
    // 由于服务器只提供该单个子功能项，需提供个二级分类列表
    ui->siderbar->viewport()->setAutoFillBackground(false);
    ui->siderbar->setFrameShape(QFrame::NoFrame);
    auto generalSettingItem = new QListWidgetItem(tr("General Settings"), ui->siderbar);

    auto subItemIcon = QIcon::fromTheme("kcp-power-general-settings");
    if (!subItemIcon.isNull())
    {
        QPixmap pixmap = subItemIcon.pixmap(40, 40);
        if (StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK)
        {
            QImage image = pixmap.toImage();
            image.invertPixels(QImage::InvertRgb);
            pixmap = QPixmap::fromImage(image);
        }
        generalSettingItem->setIcon(pixmap);
    }
    ui->siderbar->addItem(generalSettingItem);
    ui->siderbar->setCurrentRow(0);

    /// 初始化QSlider,和延迟设置的Timer
    ui->slider_idleTime->setMaximum(MAX_IDLE_TIME);
    ui->slider_idleTime->setMinimum(1);
    m_idleTimeSettingTimer.setInterval(300);
    m_idleTimeSettingTimer.setSingleShot(true);

    // 空闲时是否锁定屏幕及屏保
    m_switchIdlelockScreen = new KiranSwitchButton(this);
    m_switchIdlelockScreen->setAccessibleName("SwitchIdleLockScreen");
    ui->layout_idleLock->addWidget(m_switchIdlelockScreen);

    if( m_sessionSettings )
    {
        int idleTime = m_sessionSettings->get(KEY_IDLE_DELAY).toInt();
        updateIdleTimeLabel(idleTime);
        ui->slider_idleTime->setValue(idleTime);
    }

    if( m_screensaverSettings )
    {
        bool idleActivationLock = m_screensaverSettings->get(KEY_IDLE_ACTIVATION_LOCK).toBool();
        m_switchIdlelockScreen->setChecked(idleActivationLock);
    }

    connect(ui->slider_idleTime,
            &QSlider::valueChanged,
            this, [this]()
            { m_idleTimeSettingTimer.start(); });

    connect(&m_idleTimeSettingTimer,
            &QTimer::timeout,
            this, [this]()
            {
                if(!m_sessionSettings)
                {
                    KLOG_WARNING(qLcPower) << "session settings is null!";
                    return;
                }
                int idleTime = ui->slider_idleTime->value();
                m_sessionSettings->set(KEY_IDLE_DELAY, idleTime); });

    connect(m_switchIdlelockScreen, &QAbstractButton::toggled,
            this, [this](bool enable)
            {
            if(!m_screensaverSettings)
            {
                KLOG_WARNING(qLcPower) << "screensaver settings is null!";
                return;
            }
            m_screensaverSettings->set(KEY_IDLE_ACTIVATION_LOCK, enable); });
}

void ServerGeneralSettings::updateIdleTimeLabel(int min)
{
    QString idleTime;

    int hour = min / 60;
    int minute = min % 60;
    QStringList temp;
    if (hour)
    {
        temp.append(tr("%1hour").arg(hour));
    }
    if (minute)
    {
        temp.append(tr("%1minute").arg(minute));
    }
    idleTime = temp.join(" ");
    ui->label_idleTime->setText(idleTime);
}