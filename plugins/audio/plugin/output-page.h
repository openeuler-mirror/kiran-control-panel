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
#ifndef KIRAN_CPANEL_AUDIO_OUTPUT_PAGE_H
#define KIRAN_CPANEL_AUDIO_OUTPUT_PAGE_H

#include <QMap>
#include <QWidget>
#include <QDBusServiceWatcher>

QT_BEGIN_NAMESPACE
namespace Ui
{
class OutputPage;
}
QT_END_NAMESPACE

class AudioInterface;
class AudioDeviceInterface;
class QMediaPlayer;
class OutputPage : public QWidget
{
    Q_OBJECT

public:
    explicit OutputPage(QWidget *parent = nullptr);
    ~OutputPage() override;
    QSize sizeHint() const override;

private slots:
    void defaultSinkChanged(int index);
    void addSink(int index);
    void deleteSink(uint index);

    void changeDefaultOutputCard(int index);
    void onActivePortChanged(const QString &value);
    void changeVolumeSlider(double value);
    void changeBalanceSlider(double value);

    void setActivePort(int index);
    void setVolume(int value);
    void setBalance(int value);

    void playSoundExample();

private:
    void init();
    void initConnect();
    void loadSoundExamples();

    void initSettins();
    void initCardOptions();
    void initActivedPort();
    void initVolumeAndBalance();
    void disableSettings();
    
    void setDefaultSink(int sinkIndex);

    void reload();
    void clear();

private:
    Ui::OutputPage *ui;
    AudioInterface *m_audioInterface;
    AudioDeviceInterface *m_defaultSink;
    QDBusServiceWatcher *m_dbusServiceWatcher;
    QMediaPlayer* m_mediaPlayer;
};


#endif  //KIRAN_CPANEL_AUDIO_OUTPUT_PAGE_H
