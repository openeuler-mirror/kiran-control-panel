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

    m_statusNotifierManager = new StatusNotifierManagerInterface(STATUS_NOTIFIER_MANAGER, STATUS_NOTIFIER_MANAGER_OBJECT_NAME, QDBusConnection::sessionBus(), this);
    m_systemTray = new QSystemTrayIcon();

    initTrayIcon();
    initMenu();
    initDbusServiceWatcher();
    initConnect();
}

AudioSystemTray::~AudioSystemTray()
{
    delete m_volumeSettingPage;
    delete m_mixedSettingPage;
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
    double currentVolumeDouble = 0;
    if(!m_audioInterface->getCards().isEmpty())
    {
        QDBusPendingReply<QString> defaultSinkPath = m_audioInterface->GetDefaultSink();
        AudioDeviceInterface defaultSink (AUDIO_DBUS_NAME, defaultSinkPath, QDBusConnection::sessionBus());
        currentVolumeDouble = defaultSink.volume() * 100;
    }
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

void AudioSystemTray::initDbusServiceWatcher()
{
    m_dbusServiceWatcher = new QDBusServiceWatcher();
    m_dbusServiceWatcher->setConnection(QDBusConnection::sessionBus());
    m_dbusServiceWatcher->addWatchedService(AUDIO_DBUS_NAME);
    m_dbusServiceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    connect(m_dbusServiceWatcher, &QDBusServiceWatcher::serviceUnregistered, [this](const QString &service)
            {
                setTrayIcon(0);
            });
}

void AudioSystemTray::initConnect()
{
    connect(m_systemTray, &QSystemTrayIcon::activated, this, &AudioSystemTray::handleAudioTrayClicked);

    connect(m_volumeSettingPage,&VolumeSettingPage::volumeChanged,[this](double value)
    {
        int currentVolume = round(value * 100);  //表示数值的时候向上取整
        KLOG_DEBUG() << "sink volume changed :" << currentVolume;
        setTrayIcon(currentVolume);
    });

    connect(m_volumeSettingPage,&VolumeSettingPage::sinkMuteChanged,[this](bool mute,double currentVolume)
    {
        if(mute)
        {
            setTrayIcon(0);
        }
        else
        {
            setTrayIcon(currentVolume);
        }
    });

    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, [this](Kiran::PaletteType paletteType)
            {
                //获取当前音量值重新设置TrayIcon
                QDBusPendingReply<QString> defaultSinkPath = m_audioInterface->GetDefaultSink();
                AudioDeviceInterface defaultSink (AUDIO_DBUS_NAME, defaultSinkPath, QDBusConnection::sessionBus());
                double currentVolumeDouble = defaultSink.volume() * 100;
                setTrayIcon(round(currentVolumeDouble)); 
            });
}

void AudioSystemTray::handleAudioTrayClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        if(m_volumenPopup->isVisible())
            m_volumenPopup->hide();
        else
        {
            setVolumeSettingPos();
            m_volumenPopup->show();
        }
        break;
    default:
        break;
    }
}

void AudioSystemTray::setVolumeSettingPos()
{
    getTrayGeometry();
    int offset = 8;  // KiranRoundedTrayPopup 的margin为8
    int pageWidth = 300;
    int pageHeight = 66;

    int showPosY; 
    // 托盘程序在顶端
    if(m_yTray == 0)
    {
        showPosY  = m_heightTray - offset;
    }
    else
    {
        //托盘程序在底部
        showPosY = m_yTray - pageHeight - offset;
    }

    m_volumenPopup->setGeometry(m_xTray - pageWidth / 2, showPosY, pageWidth, pageHeight);
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
    m_mixedPopup->move(m_xTray - width / 2, m_yTray - height - offset);
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
    QIcon icon = QIcon::fromTheme(iconPath);
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

    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(getGeometry.value().toLatin1(), &jsonParseError);

    if(doc.isNull() || !doc.isObject() || (jsonParseError.error != QJsonParseError::NoError))
    {
        return;
    }
    
    double height, width, x, y = 0;
    QJsonObject object = doc.object();
    height = object.value("height").toDouble();
    width = object.value("width").toDouble();
    x = object.value("x").toDouble();
    y = object.value("y").toDouble();
    
    m_heightTray = static_cast<int>(height);
    m_widthTray = static_cast<int>(width);
    m_xTray = static_cast<int>(x);
    m_yTray = static_cast<int>(y);
    KLOG_DEBUG() << "tray geometry" 
                        << "height:" << m_heightTray 
                        << "width:" << m_widthTray
                        << "x:" << m_xTray
                        << "y:" << m_yTray;
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
    else if (0 < value && value <= 34)
    {
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-low.svg"));
        icon.addPixmap(trayIconColorSwitch(":/kcp-audio-images/kcp-audio-low.svg", 64));
    }
    else if (33 < value && value <= 67)
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

