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

#ifndef KIRAN_CPANEL_AUDIO_VOLUME_SETTING_PAGE_H
#define KIRAN_CPANEL_AUDIO_VOLUME_SETTING_PAGE_H

#include <QWidget>
#include "common/audio-node.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class VolumeSettingPage;
}
QT_END_NAMESPACE

class AudioInterface;
class AudioDeviceInterface;
class AudioStreamInterface;

class VolumeSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeSettingPage(enum AudioNode audio, const QString objectPath, QWidget *parent = nullptr);
    ~VolumeSettingPage() override;

    void initAudioDevice();
    void initAudioStream();
    template <class Audio>
    void initSettings(Audio *audio);
    template <class Audio>
    void clickMuteButton(Audio *audio);
    void volumeIconColorSwitch(QString svgPath, QString color);

    void paintEvent(QPaintEvent *event) override;
public slots:
    void handleVolumeChanged(double value);
    void handleMuteButtonClicked();
    void setVolumeIcon(int value);
    void hideLine();

private:
    Ui::VolumeSettingPage *ui;
    AudioInterface *m_audioInterface;
    AudioDeviceInterface *m_sink;
    AudioStreamInterface *m_sinkInput;

    int volumeBeforeMute;
    AudioNode audioNode;
};

#endif  //KIRAN_CPANEL_AUDIO_VOLUME_SETTING_PAGE_H
