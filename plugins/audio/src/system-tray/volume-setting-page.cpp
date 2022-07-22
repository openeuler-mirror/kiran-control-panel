/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-audio is licensed under Mulan PSL v2.
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
#include "volume-setting-page.h"
#include "ui_volume-setting-page.h"
#include "dbus/audio-device-interface.h"
#include "dbus/audio-interface.h"
#include "dbus/audio-stream-interface.h"

#include <kiran-session-daemon/audio-i.h>
#include <qt5-log-i.h>

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QSvgRenderer>
#include <style-palette.h>

VolumeSettingPage::VolumeSettingPage(enum AudioNode audio, QString objectPath, QWidget *parent) : QWidget(parent), ui(new Ui::VolumeSettingPage)
{
    ui->setupUi(this);
    ui->volume->setStyleSheet("color:#2eb3ff;");
    QDBusConnection session = QDBusConnection::sessionBus();
    m_audioInterface = AudioInterface::instance();
    audioNode = audio;
    if (audioNode == AUDIO_DEVICE)
    {
        KLOG_DEBUG() << "AUDIO_DEVICE";
        m_sink = new AudioDeviceInterface(AUDIO_DBUS_NAME, objectPath, session, this);
        initAudioDevice();
    }
    else if (audioNode == AUDIO_STREAM)
    {
        KLOG_DEBUG() << "AUDIO_STREAM";
        m_sinkInput = new AudioStreamInterface(AUDIO_DBUS_NAME, objectPath, session, this);
        initAudioStream();
    }
    connect(ui->muteButton, &QPushButton::clicked, this, &VolumeSettingPage::handleMuteButtonClicked);
}

VolumeSettingPage::~VolumeSettingPage()
{
    delete ui;
}

void VolumeSettingPage::initAudioDevice()
{
    initSettings(m_sink);
    ui->volumeName->setText(tr("Volume"));
    connect(m_sink, &AudioDeviceInterface::volumeChanged, [=](double value) {
        handleVolumeChanged(value);
    });
    connect(m_sink, &AudioDeviceInterface::muteChanged, [=](bool value) {
        KLOG_DEBUG() << "m_sink  muteChanged:" << value;
    });
}

void VolumeSettingPage::initAudioStream()
{
    initSettings(m_sinkInput);
    ui->volumeName->setText(m_sinkInput->GetProperty("application.name"));
    connect(m_sinkInput, &AudioStreamInterface::volumeChanged, [=](double value) {
        handleVolumeChanged(value);
    });
    connect(m_sinkInput, &AudioStreamInterface::muteChanged, [=](bool value) {
        KLOG_DEBUG() << "m_sinkInput muteChanged:" << value;
    });
}

template <class Audio>
void VolumeSettingPage::initSettings(Audio *audio)
{
    ui->volumeSetting->setRange(0, 100);
    ui->volumeSetting->setSingleStep(1);
    ui->volumeSetting->setPageStep(1);

    KLOG_DEBUG() << "current volume:" << audio->volume();
    double currentVolumeDouble = audio->volume() * 100;
    int currentVolume = round(currentVolumeDouble);
    setVolumeIcon(currentVolume);
    ui->volumeSetting->setValue(currentVolume);
    ui->volume->setText(QString::number(currentVolume) + "%");

    connect(ui->volumeSetting, &QSlider::valueChanged, [=](int value) {
        double volumeValue = static_cast<double>(value) / static_cast<double>(100);
        audio->SetVolume(volumeValue);
    });
}

void VolumeSettingPage::handleVolumeChanged(double value)
{
    ui->volumeSetting->blockSignals(true);   //为了避免拖动的同时设置位置会出现问题
    int currentVolume = round(value * 100);  //表示数值的时候向上取整
    ui->volume->setText(QString::number(currentVolume) + "%");
    setVolumeIcon(currentVolume);
    ui->volumeSetting->setValue(currentVolume);
    ui->volumeSetting->blockSignals(false);
}

void VolumeSettingPage::handleMuteButtonClicked()
{
    if (audioNode == AUDIO_DEVICE)
        clickMuteButton(m_sink);
    else
        clickMuteButton(m_sinkInput);
}

template <class Audio>
void VolumeSettingPage::clickMuteButton(Audio *audio)
{
    double currentVolumeDouble = audio->volume() * 100;
    int currentVolume = round(currentVolumeDouble);

    if (currentVolume != 0)
    {
        KLOG_DEBUG() << "m_sink->mute() :" << audio->mute();
        if (!audio->mute())
        {
            volumeBeforeMute = currentVolume;
            KLOG_DEBUG() << "volumeBeforeMute:" << volumeBeforeMute;
            audio->SetMute(true);
        }
    }
    else
    {
        if (volumeBeforeMute != 0)
        {
            KLOG_DEBUG() << "SetVolume volumeBeforeMute:" << volumeBeforeMute;
            audio->SetVolume(static_cast<double>(volumeBeforeMute) / static_cast<double>(100));
            volumeBeforeMute = 0;
            KLOG_DEBUG() << "volumeBeforeMute = 0";
        }
    }
}

//XXX:频繁调用函数,需要优化
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
    //icon原本为浅色
    QIcon icon(iconPath);
    QPixmap pixmap = icon.pixmap(16,16);
    if( Kiran::StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK )
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }
    return pixmap;
}

void VolumeSettingPage::hideLine()
{
    ui->line->hide();
}
