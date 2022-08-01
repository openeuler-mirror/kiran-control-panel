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

#ifndef KIRAN_CPANEL_AUDIO_MIXED_SETTING_PAGE_H
#define KIRAN_CPANEL_AUDIO_MIXED_SETTING_PAGE_H

#include <QMap>
#include <QWidget>
#include <QScrollArea>

class AudioInterface;
class QVBoxLayout;
class VolumeSettingPage;
class MixedSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit MixedSettingPage(QWidget *parent = nullptr);
    ~MixedSettingPage();

    int getHeight();
public slots:
    void initSinkInput();
    void initSink();
    void handleSinkInputAdded(int index);
    void handleSinkInputDelete(int index);

protected:
    // void paintEvent(QPaintEvent* event) override; 
signals:
    void adjustedMixedSettingPageSize();

private:
    AudioInterface *m_audioInterface;
    
    QVBoxLayout *m_vboxLayout;
    QScrollArea *m_scrollArea;
    QWidget * m_scrollAreaWidgetContents;
    QVBoxLayout *m_vboxScrollAreaLayout;

    QMap<int, VolumeSettingPage *> m_sinkInputsMap;
};

#endif  //KIRAN_CPANEL_AUDIO_MIXED_SETTING_PAGE_H
