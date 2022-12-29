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

class OutputPage : public QWidget
{
    Q_OBJECT

public:
    explicit OutputPage(QWidget *parent = nullptr);
    ~OutputPage() override;
    QSize sizeHint() const override;

public slots:
    void handleDefaultSinkChanged(int index);
    void handleSinkAdded(int index);
    void handleSinkDelete(int index);
    void handleActivePortChanged(const QString &value);
    void handleVolumeChanged(double value);
    void handleBalanceChanged(double value);

private:
    void init();
    void initOutputDevice();
    void initOutputSettins();
    void initConnect();
    void initActivedPort();
    void initVolumeValue();
    void initBalanceValue();

    void disableSettings();

private:
    Ui::OutputPage *ui;
    AudioInterface *m_audioInterface;
    QMap<int, AudioDeviceInterface *> m_outputDevicesMap;
    AudioDeviceInterface *m_defaultSink;
    int m_defaultDeviceIndex;
    QDBusServiceWatcher *m_dbusServiceWatcher;
};


#endif  //KIRAN_CPANEL_AUDIO_OUTPUT_PAGE_H
