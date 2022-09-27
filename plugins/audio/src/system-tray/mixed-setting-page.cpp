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
    m_vboxLayout = new QVBoxLayout(this);
    m_vboxLayout->setMargin(0);
    m_vboxLayout->setSpacing(0);

    // TODO:增加scrollArea

    // m_scrollArea = new QScrollArea(this);
    // m_scrollArea->setWidgetResizable(true);

    // m_scrollAreaWidgetContents = new QWidget();
    // m_vboxScrollAreaLayout = new QVBoxLayout(m_scrollAreaWidgetContents);
    // m_vboxScrollAreaLayout->setMargin(0);
    // m_vboxScrollAreaLayout->setSpacing(0);
    // m_scrollAreaWidgetContents->setStyleSheet("QWidget{"
    //                                     "border: 1px solid red;"
    //                                     "border-radius:6px;"
    //                                     "}");

    initSink();
    initSinkInput();

    // m_scrollArea->setWidget(m_scrollAreaWidgetContents);
    // m_scrollArea->setFixedWidth(300);
    // m_scrollArea->setFrameShape(QFrame::NoFrame);
    // m_scrollArea->setStyleSheet("QScrollArea{"
    //                                     "border: 1px solid red;"
    //                                     "border-radius:6px;"
    //                                     "}");

    // m_vboxLayout->addWidget(m_scrollArea);
    // setStyleSheet("background:red;border-radius:6px;");

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ensurePolished();

    connect(m_audioInterface, &AudioInterface::SinkInputAdded, this, &MixedSettingPage::handleSinkInputAdded);
    connect(m_audioInterface, &AudioInterface::SinkInputDelete, this, &MixedSettingPage::handleSinkInputDelete);
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
    // m_vboxScrollAreaLayout->addWidget(sinkSetting);
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
        // m_vboxScrollAreaLayout->addWidget(sinkInputSettings);
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
    // m_vboxScrollAreaLayout->addWidget(sinkInputAdded);
    Q_EMIT adjustedMixedSettingPageSize();
}

void MixedSettingPage::handleSinkInputDelete(int index)
{
    KLOG_DEBUG() << "SinkInputDelete index: " << index;
    delete m_sinkInputsMap[index];
    m_sinkInputsMap[index] = nullptr;
    int removeNum = m_sinkInputsMap.remove(index);
    update();
    adjustSize();
    Q_EMIT adjustedMixedSettingPageSize();
}

int MixedSettingPage::getHeight()
{
    // KLOG_DEBUG() << "m_vboxLayout->sizeHint():" << m_vboxLayout->sizeHint();
    int height = 66 * (m_sinkInputsMap.count() + 1);

    if (height > 198)
        height = 198;

    return height;
}

/*
#include <style-palette.h>
#include <style-property.h>
#include <QPainterPath>
void MixedSettingPage::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    QStyle::State state;

    opt.initFrom(this);
    state = opt.state;
    QRectF frect = opt.rect;
    QPainterPath painterPath;
    painterPath.addRect(frect);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto kiranPalette = Kiran::StylePalette::instance();
    QColor backgroundColor;

    backgroundColor = kiranPalette->color(Kiran::StylePalette::Normal,
                                            Kiran::StylePalette::Window,
                                            Kiran::StylePalette::Background);

    painter.fillRect(frect, backgroundColor);

    QColor borderColor;
    borderColor = kiranPalette->color(Kiran::StylePalette::Normal,
                                      Kiran::StylePalette::Widget,
                                      Kiran::StylePalette::Border);

    auto pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(borderColor);
    pen.setStyle(Qt::SolidLine);
    painter.strokePath(painterPath, pen);


    QWidget::paintEvent(event);
}
*/