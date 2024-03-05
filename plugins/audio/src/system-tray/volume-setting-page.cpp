/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
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
#include "volume-setting-page.h"
#include "dbus/audio-device-interface.h"
#include "dbus/audio-interface.h"
#include "dbus/audio-stream-interface.h"
#include "ui_volume-setting-page.h"

#include <kiran-session-daemon/audio-i.h>
#include <qt5-log-i.h>

#include <style-palette.h>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QSvgRenderer>

VolumeSettingPage::VolumeSettingPage(enum AudioNode audio, const QString objectPath, QWidget *parent) : QWidget(parent), ui(new Ui::VolumeSettingPage)
{
    ui->setupUi(this);
    ui->volume->setStyleSheet("color:#2eb3ff;");
    QDBusConnection session = QDBusConnection::sessionBus();
    m_audioInterface = AudioInterface::instance();
    m_audioNode = audio;

    if (m_audioNode == AUDIO_DEVICE)
    {
        QDBusPendingReply<QString> defaultSinkPath = m_audioInterface->GetDefaultSink();
        m_sink = new AudioDeviceInterface(AUDIO_DBUS_NAME, defaultSinkPath, QDBusConnection::sessionBus(), this);
        initAudioDevice();

        connect(m_sink, &AudioDeviceInterface::volumeChanged, this, &VolumeSettingPage::handleVolumeChanged, Qt::UniqueConnection);
        connect(m_sink, &AudioDeviceInterface::muteChanged, this, &VolumeSettingPage::changeSinkMute, Qt::UniqueConnection);
        connect(m_sink, &AudioDeviceInterface::active_portChanged, this, &VolumeSettingPage::changeSinkActivePort, Qt::UniqueConnection);

        connect(ui->volumeSetting, &QSlider::valueChanged, [this](int value)
                {
                    double volumeValue = static_cast<double>(value) / static_cast<double>(100);
                    m_sink->SetVolume(volumeValue); });

        connect(m_audioInterface, &AudioInterface::SinkAdded, this, &VolumeSettingPage::handleSinkAdded);
        connect(m_audioInterface, &AudioInterface::SinkDelete, this, &VolumeSettingPage::handleSinkDelete);
        connect(m_audioInterface, &AudioInterface::DefaultSinkChange, this, &VolumeSettingPage::handleDefaultSinkChanged, Qt::QueuedConnection);
    }
    else if (m_audioNode == AUDIO_STREAM)
    {
        m_sinkInput = new AudioStreamInterface(AUDIO_DBUS_NAME, objectPath, session, this);
        initAudioStream();
    }
    connect(ui->muteButton, &QPushButton::clicked, this, &VolumeSettingPage::handleMuteButtonClicked);

    initDbusServiceWatcher();
}

VolumeSettingPage::~VolumeSettingPage()
{
    delete ui;
}

void VolumeSettingPage::initDbusServiceWatcher()
{
    m_dbusServiceWatcher = new QDBusServiceWatcher();
    m_dbusServiceWatcher->setConnection(QDBusConnection::sessionBus());
    m_dbusServiceWatcher->addWatchedService(AUDIO_DBUS_NAME);
    m_dbusServiceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    connect(m_dbusServiceWatcher, &QDBusServiceWatcher::serviceUnregistered, [this](const QString &service)
            {
        KLOG_DEBUG() << "serviceUnregistered:" << service;
        disableSettings(); });
}

void VolumeSettingPage::initAudioDevice()
{
    KLOG_DEBUG() << "AUDIO_DEVICE";
    QDBusPendingReply<QString> getPorts = m_sink->GetPorts();
    // 解析默认sink的端口信息
    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(getPorts.value().toLatin1(), &jsonParseError);
    if (!doc.isNull() && jsonParseError.error == QJsonParseError::NoError)
    {
        initSettings(m_sink);
    }
    else
    {
        // 无激活端口则禁用音量设置
        disableSettings();
    }
    ui->volumeName->setText(tr("Volume"));
}

void VolumeSettingPage::initAudioStream()
{
    KLOG_DEBUG() << "AUDIO_STREAM";
    initSettings(m_sinkInput);
    ui->volumeName->setText(m_sinkInput->GetProperty("application.name"));
    connect(m_sinkInput, &AudioStreamInterface::volumeChanged, this, &VolumeSettingPage::handleVolumeChanged);
    connect(m_sinkInput, &AudioStreamInterface::muteChanged, this, &VolumeSettingPage::changeSinkInputMute);

    connect(ui->volumeSetting, &QSlider::valueChanged, [=](int value)
            {
                double volumeValue = static_cast<double>(value) / static_cast<double>(100);
                m_sinkInput->SetVolume(volumeValue); });
}

template <class Audio>
void VolumeSettingPage::initSettings(Audio *audio)
{
    ui->volumeSetting->setRange(0, 100);
    ui->volumeSetting->setSingleStep(1);
    ui->volumeSetting->setPageStep(1);
    ui->volumeSetting->setEnabled(true);

    KLOG_DEBUG() << "current volume:" << audio->volume();
    double currentVolumeDouble = audio->volume() * 100;
    int currentVolume = round(currentVolumeDouble);
    setVolumeIcon(currentVolume);
    ui->volumeSetting->setValue(currentVolume);
    ui->volume->setText(QString::number(currentVolume) + "%");
}

void VolumeSettingPage::changeSinkActivePort(const QString &value)
{
    KLOG_DEBUG() << "active port changed :" << value;
    if(value.isEmpty())
    {
        disableSettings();
        return;
    }

    double currentVolumeDouble = m_sink->volume() * 100;
    int currentVolume = round(currentVolumeDouble);
    setVolumeIcon(currentVolume);
    ui->volumeSetting->setEnabled(true);
    ui->volumeSetting->setValue(currentVolume);
    ui->volume->setText(QString::number(currentVolume) + "%");
}

void VolumeSettingPage::changeSinkMute(bool value)
{
    KLOG_DEBUG() << "change sink mute:" << value;
    double currentVolume = m_sink->volume() * 100;
    emit sinkMuteChanged(value, currentVolume);
    if (value)
    {
        setVolumeIcon(0);
    }
    else
    {
        setVolumeIcon(currentVolume);
    }
}
void VolumeSettingPage::changeSinkInputMute(bool value)
{
    KLOG_DEBUG() << "change sink input mute:" << value;
    double currentVolume = m_sinkInput->volume() * 100;
    if (value)
    {
        setVolumeIcon(0);
    }
    else
    {
        setVolumeIcon(currentVolume);
    }
}

void VolumeSettingPage::handleVolumeChanged(double value)
{
    if(!ui->volumeSetting->isEnabled())
    {
        return;
    }
    
    ui->volumeSetting->blockSignals(true);   // 为了避免拖动的同时设置位置会出现问题
    int currentVolume = round(value * 100);  // 表示数值的时候向上取整
    ui->volume->setText(QString::number(currentVolume) + "%");
    setVolumeIcon(currentVolume);
    ui->volumeSetting->setValue(currentVolume);
    ui->volumeSetting->blockSignals(false);
    emit volumeChanged(value);
}

void VolumeSettingPage::handleMuteButtonClicked()
{
    if (m_audioNode == AUDIO_DEVICE)
        switchMute(m_sink);
    else
        switchMute(m_sinkInput);
}

void VolumeSettingPage::handleDefaultSinkChanged(int index)
{
    KLOG_DEBUG() << "Default Sink Changed";
    // delete and restart init defaultSink
    if (m_sink != nullptr)
    {
        m_sink->deleteLater();
        m_sink = nullptr;
    }

    QDBusPendingReply<QString> defaultSinkPath = m_audioInterface->GetDefaultSink();
    m_sink = new AudioDeviceInterface(AUDIO_DBUS_NAME, defaultSinkPath, QDBusConnection::sessionBus(), this);
    initAudioDevice();
    connect(m_sink, &AudioDeviceInterface::volumeChanged, this, &VolumeSettingPage::handleVolumeChanged, Qt::UniqueConnection);
    connect(m_sink, &AudioDeviceInterface::muteChanged, this, &VolumeSettingPage::changeSinkMute, Qt::UniqueConnection);
}

void VolumeSettingPage::handleSinkAdded(int index)
{
    KLOG_DEBUG() << "SinkAdded";
    // 当已经存在defaultSink时，暂时不处理其他sink的添加
    if (m_sink != nullptr)
    {
        // 刷新界面
        initSettings(m_sink);
    }
    else
    {
        // defaultSink不存在，则重新初始化设备
        initAudioDevice();
    }
}

void VolumeSettingPage::handleSinkDelete(int index)
{
    // 当前存在Sink
    if (m_sink != nullptr)
    {
        // 删除的是defaultSink则进行处理，删除其他sink暂时不处理
        if (m_sink->index() == index)
        {
            disableSettings();
        }
    }
}

template <class Audio>
void VolumeSettingPage::switchMute(Audio *audio)
{
    if (!audio->mute())
    {
        audio->SetMute(true);
    }
    else
    {
        audio->SetMute(false);
    }
    KLOG_DEBUG() << "current defalut sink mute:" << audio->mute();
}

// XXX:频繁调用函数,需要优化
void VolumeSettingPage::setVolumeIcon(int value)
{
    if (value == 0)
    {
        ui->muteButton->setIcon(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-mute.svg"));
    }
    else if (0 < value && value <= 33)
    {
        ui->muteButton->setIcon(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-low.svg"));
    }
    else if (33 < value && value <= 66)
    {
        ui->muteButton->setIcon(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-medium.svg"));
    }
    else
    {
        ui->muteButton->setIcon(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-loud.svg"));
    }
}

QPixmap VolumeSettingPage::trayIconColorSwitch(const QString &iconPath)
{
    // icon原本为浅色
    QIcon icon(iconPath);
    QPixmap pixmap = icon.pixmap(16, 16);
    if (Kiran::StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK)
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }
    return pixmap;
}

void VolumeSettingPage::disableSettings()
{
    ui->volumeSetting->setValue(0);
    ui->volume->setText(QString::number(0) + "%");
    ui->volumeSetting->setEnabled(false);
    setVolumeIcon(0);
    emit volumeChanged(0);
}

void VolumeSettingPage::hideLine()
{
    ui->line->hide();
}
