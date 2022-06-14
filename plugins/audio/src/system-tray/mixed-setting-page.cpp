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

#include "mixed-setting-page.h"
#include "dbus/audio-device-interface.h"
#include "dbus/audio-interface.h"
#include "dbus/audio-stream-interface.h"
#include "volume-setting-page.h"

#include <kiran-session-daemon/audio-i.h>
#include <kiran-titlebar-window.h>
#include <qt5-log-i.h>

#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

MixedSettingPage::MixedSettingPage(QWidget *parent) : QWidget(parent)
{
    m_audioInterface = AudioInterface::instance();
    m_vboxLayout = new QVBoxLayout();
    m_vboxLayout->setMargin(0);
    m_vboxLayout->setSpacing(0);

    initSink();
    initSinkInput();
    setLayout(m_vboxLayout);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ensurePolished();

    connect(m_audioInterface, &AudioInterface::SinkInputAdded, [=](int index) {
        handleSinkInputAdded(index);
    });
    connect(m_audioInterface, &AudioInterface::SinkInputDelete, [=](int index) {
        handleSinkInputDelete(index);
    });
}

MixedSettingPage::~MixedSettingPage()
{
}

void MixedSettingPage::initSink()
{
    QDBusPendingReply<QString> defaultSinkPath = m_audioInterface->GetDefaultSink();
    VolumeSettingPage *sinkSetting = new VolumeSettingPage(AUDIO_DEVICE, defaultSinkPath);
    sinkSetting->hideLine();
    m_vboxLayout->addWidget(sinkSetting);
}

void MixedSettingPage::initSinkInput()
{
    QDBusPendingReply<QStringList> getSinkInputs = m_audioInterface->GetSinkInputs();
    KLOG_DEBUG() << "getSinkInputs:" << getSinkInputs.value();
    QStringList sinkInputsList = getSinkInputs.value();

    for (int i = 0; i < sinkInputsList.count(); ++i)
    {
        QString objectPath = sinkInputsList.at(i);
        KLOG_DEBUG() << "objectPath :" << objectPath << "count:" << objectPath.count();
        VolumeSettingPage *sinkInputSettings = new VolumeSettingPage(AUDIO_STREAM, objectPath);
        int index = objectPath.mid(49).toInt();  //获取SinkInput的index
        KLOG_DEBUG() << "index" << index;
        m_sinkInputsMap[index] = sinkInputSettings;
        m_vboxLayout->addWidget(sinkInputSettings);
    }
}

void MixedSettingPage::handleSinkInputAdded(int index)
{
    KLOG_DEBUG() << "SinkInputAdded index: " << index;
    QString objectPath = QString("/com/kylinsec/Kiran/SessionDaemon/Audio/SinkInput%1").arg(index);
    KLOG_DEBUG() << "SinkInputAdded objectPath:" << objectPath;
    VolumeSettingPage *sinkInputAdded = new VolumeSettingPage(AUDIO_STREAM, objectPath);
    m_sinkInputsMap[index] = sinkInputAdded;
    m_vboxLayout->addWidget(sinkInputAdded);
}

void MixedSettingPage::handleSinkInputDelete(int index)
{
    KLOG_DEBUG() << "SinkInputDelete index: " << index;
    KLOG_DEBUG() << "m_sinkInputsMap[index];" << m_sinkInputsMap[index];
    delete m_sinkInputsMap[index];
    m_sinkInputsMap[index] = nullptr;
    update();
}

void MixedSettingPage::paintEvent(QPaintEvent *Event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
