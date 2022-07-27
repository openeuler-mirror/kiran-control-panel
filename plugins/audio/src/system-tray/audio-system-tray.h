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

#ifndef KIRAN_CPANEL_AUDIO_AUDIOSYSTEMTRAY_H
#define KIRAN_CPANEL_AUDIO_AUDIOSYSTEMTRAY_H

#include <QSystemTrayIcon>
#include <QWidget>
// #include <QVBoxLayout>
// #include <QScrollArea>

class VolumeSettingPage;
class MixedSettingPage;
class StatusNotifierManagerInterface;
class AudioInterface;
class AudioDeviceInterface;
class KiranRoundedTrayPopup;
class AudioSystemTray : public QWidget
{
    Q_OBJECT
public:
    explicit AudioSystemTray(QWidget* parent = nullptr);
    ~AudioSystemTray();

    void initVolumeSettingPage(QString objectPath);
    void initMixedSettingPage();
    void initTrayIcon();
    void initMenu();
    void initConnect();
    void setVolumeSettingPos();
    void setMixedSettingPos();
    void trayIconColorSwitch(QString svgPath, QString color);
    void getTrayGeometry();
public slots:

    void handleAudioTrayClicked(QSystemTrayIcon::ActivationReason reason);
    void handleMixedSettingClicked();
    void handleVolumeSettingClicked();
    void getTrayIconStyle();
    void setTrayIcon(int value);
    void handleAdjustedMixedSettingPageSize();

private:
    QSystemTrayIcon* m_systemTray;
    QMenu* m_menu;
    QAction* m_volumeSetting;
    QAction* m_mixedSetting;

    KiranRoundedTrayPopup* m_volumenPopup;
    KiranRoundedTrayPopup* m_mixedPopup;

    // QScrollArea *m_scrollArea;
    // QVBoxLayout *m_vboxScrollAreaLayout;

    VolumeSettingPage* m_volumeSettingPage;
    MixedSettingPage* m_mixedSettingPage;

    StatusNotifierManagerInterface* m_statusNotifierManager;
    AudioInterface* m_audioInterface;
    AudioDeviceInterface* m_sink;

    QString m_colorTheme;
    int xTray, yTray, heightTray, widthTray;
};

#endif  //KIRAN_CPANEL_AUDIO_AUDIOSYSTEMTRAY_H
