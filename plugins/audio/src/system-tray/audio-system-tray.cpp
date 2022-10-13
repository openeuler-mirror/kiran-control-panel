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

#include "audio-system-tray.h"
#include "dbus/audio-device-interface.h"
#include "dbus/audio-interface.h"
#include "dbus/status-notifier-manager.h"
#include "kiran-rounded-tray-popup/kiran-rounded-tray-popup.h"
#include "system-tray/mixed-setting-page.h"
#include "system-tray/volume-setting-page.h"

#include <kiran-session-daemon/audio-i.h>
#include <qt5-log-i.h>
#include <QGuiApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QSvgRenderer>

#include <style-palette.h>

#define STATUS_NOTIFIER_MANAGER "org.kde.StatusNotifierManager"
#define STATUS_NOTIFIER_MANAGER_OBJECT_NAME "/StatusNotifierManager"

AudioSystemTray::AudioSystemTray(QWidget *parent) : QWidget(parent)
{
    m_audioInterface = AudioInterface::instance();
    QDBusPendingReply<QString> defaultSinkPath = m_audioInterface->GetDefaultSink();
    initVolumeSettingPage(defaultSinkPath);
    initMixedSettingPage();

    m_sink = new AudioDeviceInterface(AUDIO_DBUS_NAME, defaultSinkPath, QDBusConnection::sessionBus());
    m_statusNotifierManager = new StatusNotifierManagerInterface(STATUS_NOTIFIER_MANAGER, STATUS_NOTIFIER_MANAGER_OBJECT_NAME, QDBusConnection::sessionBus(), this);
    m_systemTray = new QSystemTrayIcon();

    initTrayIcon();
    initMenu();
    initConnect();
}
AudioSystemTray::~AudioSystemTray()
{
}

void AudioSystemTray::initVolumeSettingPage(QString objectPath)
{
    m_volumeSettingPage = new VolumeSettingPage(AUDIO_DEVICE, objectPath);
    m_volumeSettingPage->setObjectName("volumeSettingPage");
    m_volumeSettingPage->setWindowFlags(Qt::Popup | Qt::BypassWindowManagerHint);
    m_volumeSettingPage->hideLine();
    m_volumeSettingPage->setAttribute(Qt::WA_TranslucentBackground);

    m_volumenPopup = new KiranRoundedTrayPopup();
    m_volumenPopup->setContentWidget(m_volumeSettingPage);
}

void AudioSystemTray::initMixedSettingPage()
{
    m_mixedSettingPage = new MixedSettingPage();
    m_mixedSettingPage->setObjectName("mixedSettingPage");
    m_mixedSettingPage->setWindowFlags(Qt::Popup | Qt::BypassWindowManagerHint);
    m_mixedSettingPage->setAttribute(Qt::WA_TranslucentBackground);

    connect(m_mixedSettingPage, &MixedSettingPage::adjustedMixedSettingPageSize, this, &AudioSystemTray::handleAdjustedMixedSettingPageSize);
    m_mixedPopup = new KiranRoundedTrayPopup();
    m_mixedPopup->setContentWidget(m_mixedSettingPage);
}

void AudioSystemTray::initTrayIcon()
{
    getTrayIconStyle();
    double currentVolumeDouble = m_sink->volume() * 100;
    KLOG_INFO() << "currentVolumeDouble" << round(currentVolumeDouble);
    setTrayIcon(round(currentVolumeDouble));
}

void AudioSystemTray::initMenu()
{
    m_menu = new QMenu(this);
    m_volumeSetting = new QAction(tr("Volume Setting"));
    m_mixedSetting = new QAction(tr("Mixed Setting"));

    m_menu->addAction(m_mixedSetting);
    m_menu->addAction(m_volumeSetting);
    m_systemTray->setContextMenu(m_menu);

    connect(m_volumeSetting, &QAction::triggered, this, &AudioSystemTray::handleVolumeSettingClicked);
    connect(m_mixedSetting, &QAction::triggered, this, &AudioSystemTray::handleMixedSettingClicked);
}

void AudioSystemTray::initConnect()
{
    connect(m_systemTray, &QSystemTrayIcon::activated, this, &AudioSystemTray::handleAudioTrayClicked);

    connect(m_sink, &AudioDeviceInterface::volumeChanged, [=](double value)
            {
                int currentVolume = round(value * 100);  //表示数值的时候向上取整
                KLOG_DEBUG() << "m_sink volumeChanged :" << currentVolume;
                setTrayIcon(currentVolume); });

    connect(m_statusNotifierManager, &StatusNotifierManagerInterface::StyleChanged, [=](const QString &style)
            {
                KLOG_DEBUG() << "StyleChanged";
                //重新获取style
                getTrayIconStyle();
                //获取当前音量值重新设置TrayIcon
                m_sink->volume();
                double currentVolumeDouble = m_sink->volume() * 100;
                setTrayIcon(round(currentVolumeDouble)); });
}

void AudioSystemTray::handleAudioTrayClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        setVolumeSettingPos();
        m_volumenPopup->show();
        break;
    }
}

void AudioSystemTray::setVolumeSettingPos()
{
    getTrayGeometry();
    int offset = 8;  // KiranRoundedTrayPopup 的margin为8
    int pageWidth = 300;
    int pageHeight = 66;

    m_volumenPopup->setGeometry(xTray - pageWidth / 2, yTray - pageHeight - offset, pageWidth, pageHeight);
}

void AudioSystemTray::handleMixedSettingClicked()
{
    setMixedSettingPos();
    m_mixedPopup->show();
}

// XXX:弹出MixedSetting界面调整
void AudioSystemTray::setMixedSettingPos()
{
    getTrayGeometry();
    int offset = 8;  // KiranRoundedTrayPopup 的margin为8
    int height = m_mixedSettingPage->getHeight();
    int width = m_mixedPopup->sizeHint().width();

    m_mixedPopup->setFixedHeight(height + offset * 2);
    m_mixedPopup->move(xTray - width / 2, yTray - height - offset);
}

void AudioSystemTray::handleAdjustedMixedSettingPageSize()
{
    if (m_mixedPopup->isVisible())
    {
        setMixedSettingPos();
    }
}

QPixmap AudioSystemTray::trayIconColorSwitch(const QString &iconPath, const int iconSize)
{
    // icon原本为浅色
    QIcon icon(iconPath);
    QPixmap pixmap = icon.pixmap(iconSize, iconSize);
    if (Kiran::StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK)
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }
    return pixmap;
}

void AudioSystemTray::getTrayGeometry()
{
    QDBusPendingReply<QString> getGeometry = m_statusNotifierManager->GetGeometry("~02-volume");
    KLOG_DEBUG() << "getGeometry.value():" << getGeometry.value();

    double height, width, x, y;
    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(getGeometry.value().toLatin1(), &jsonParseError);
    if (!doc.isNull() && jsonParseError.error == QJsonParseError::NoError)
    {
        if (doc.isObject() && jsonParseError.error == QJsonParseError::NoError)
        {
            if (doc.isObject())
            {
                QJsonObject object = doc.object();
                QStringList list = object.keys();
                height = object.value("height").toDouble();
                width = object.value("width").toDouble();
                x = object.value("x").toDouble();
                y = object.value("y").toDouble();
            }
        }
    }
    heightTray = static_cast<int>(height);
    widthTray = static_cast<int>(width);
    xTray = static_cast<int>(x);
    yTray = static_cast<int>(y);
    KLOG_DEBUG() << "getTrayGeometry ";
    KLOG_DEBUG() << "heightTray" << heightTray;
    KLOG_DEBUG() << "widthTray" << widthTray;
    KLOG_DEBUG() << "xTray" << xTray;
    KLOG_DEBUG() << "yTray" << yTray;
}

// XXX:频繁调用函数,需要优化
void AudioSystemTray::setTrayIcon(int value)
{
    QIcon icon;
    if (value == 0)
    {
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-mute.svg"));
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-mute.svg", 64));
    }
    else if (0 < value && value <= 33)
    {
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-low.svg"));
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-low.svg", 64));
    }
    else if (33 < value && value <= 66)
    {
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-medium.svg"));
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-medium.svg", 64));
    }
    else
    {
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-loud.svg"));
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-loud.svg", 64));
    }
    m_systemTray->setIcon(icon);
    m_systemTray->show();
}

void AudioSystemTray::handleVolumeSettingClicked()
{
    QProcess process(this);
    QStringList arguments;
    arguments << "-c"
              << "audio"
              << "-s"
              << "OutputPage";
    process.startDetached("kiran-control-panel", arguments);
}

//暂时不使用，改成从平台主题中获取颜色
void AudioSystemTray::getTrayIconStyle()
{
    QDBusPendingReply<QString> getStyle = m_statusNotifierManager->GetStyle();
    KLOG_DEBUG() << "getStyle.value()" << getStyle.value();
    double red, green, blue, alpha;
    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(getStyle.value().toLatin1(), &jsonParseError);
    if (!doc.isNull() && jsonParseError.error == QJsonParseError::NoError)
    {
        if (doc.isObject() && jsonParseError.error == QJsonParseError::NoError)
        {
            if (doc.isObject())
            {
                QJsonObject object = doc.object();
                QStringList list = object.keys();
                KLOG_DEBUG() << "fg_color" << object.value("fg_color");
                QJsonObject rgba = object.value("fg_color").toObject();
                red = rgba.value("red").toDouble();
                green = rgba.value("green").toDouble();
                blue = rgba.value("blue").toDouble();
                alpha = rgba.value("alpha").toDouble();
            }
        }
    }
    //暂时用rgb,rgba设置无效，需要完善
    m_colorTheme = QString("rgb(%1,%2,%3)").arg(red * 255).arg(green * 255).arg(blue * 255);
    KLOG_DEBUG() << "getTrayIconStyle:" << m_colorTheme;
}
