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
#ifndef KIRAN_CPANEL_AUDIO_INPUT_PAGE_H
#define KIRAN_CPANEL_AUDIO_INPUT_PAGE_H

#include <QMap>
#include <QWidget>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>

class AudioInfo : public QIODevice
{
    Q_OBJECT

public:
    AudioInfo(const QAudioFormat &format, QObject *parent);
    ~AudioInfo();

    void start();
    void stop();

    qreal level() const { return m_level; }

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    const QAudioFormat m_format;
    quint32 m_maxAmplitude;
    qreal m_level;  // 0.0 <= m_level <= 1.0

signals:
    void update();
};

QT_BEGIN_NAMESPACE
namespace Ui
{
class InputPage;
}
QT_END_NAMESPACE

class AudioInterface;
class AudioDeviceInterface;

class InputPage : public QWidget
{
    Q_OBJECT

public:
    explicit InputPage(QWidget *parent = nullptr);
    ~InputPage() override;
    QSize sizeHint() const override;

private:
    void init();
    void initSettings();
    void initCardOptions();
    void initVolume();
    void initActivedPort();
    void initConnet();
    void disableSettings(); 

    void initVoulumeFeedBack();
    void initAudioFormat();
    void initAudioInput();

    void reload();
    void clear();
    void clearFeedBack();

    void setDefaultSource(int sourceIndex);

private slots:
    void changeDefaultInputCard(int index);
    void setVolume(int value);
    void setActivePort(int index);
    void addSource(int index);
    void deleteSource(int index);
    
    void onActivePortChanged(const QString &value);
    void onDefaultSourceChanged(int index);
    void onVolumeChanged(double value);

    void refreshFeedBack();

private:
    Ui::InputPage *ui;
    AudioInterface *m_audioInterface;
    AudioDeviceInterface *m_defaultSource;
    bool m_isValidPort = false;

    QAudioInput *m_audioInput = nullptr;
    QAudioFormat m_format;
    QAudioDeviceInfo m_device;
    AudioInfo *m_audioInfo = nullptr;
};

#endif  //KIRAN_CPANEL_AUDIO_INPUT_PAGE_H
