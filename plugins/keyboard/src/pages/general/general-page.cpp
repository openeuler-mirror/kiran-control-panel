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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "ui_general-page.h"
#include "general-page.h"
#include "keyboard_backEnd_proxy.h"

#include <kiran-log/qt5-log-i.h>
#include <QSharedPointer>
#include <iostream>
#include <kiran-session-daemon/keyboard-i.h>

GeneralPage::GeneralPage(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::GeneralPage),
    m_keyboardInterface(new KeyboardBackEndProxy(KEYBOARD_DBUS_NAME,KEYBOARD_OBJECT_PATH,QDBusConnection::sessionBus(),this))
{
    ui->setupUi(this);

    init();
}

GeneralPage::~GeneralPage()
{
    delete ui;
}

QSize GeneralPage::sizeHint() const
{
    return {419, 595};
}

void GeneralPage::init()
{
    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout,this,&GeneralPage::handleSaverTimerTimeOut);

    ui->lineEdit_key->setPlaceholderText(tr("Enter characters to test the settings"));

    // 重复键开关
    m_repeateEnabled = m_keyboardInterface->repeat_enabled();
    ui->switch_repeatKey->setChecked(m_repeateEnabled);
    handleSwitchRepeatKeyToggled(m_repeateEnabled);
    connect(ui->switch_repeatKey,&KiranSwitchButton::toggled,
            this,&GeneralPage::handleSwitchRepeatKeyToggled);
    connect(m_keyboardInterface,&KeyboardBackEndProxy::repeat_enabledChanged,[this](bool enabled){
        KLOG_DEBUG() << "keyboard general setting repeat enable changed:" << enabled;
        m_repeateEnabled = enabled;
        ui->switch_repeatKey->setChecked(enabled);
    });

    // 重复键延时设置
    m_delay = m_keyboardInterface->repeat_delay();
    ui->hslider_delay->setValue(m_delay);
    connect(ui->hslider_delay,&QSlider::valueChanged,[this](int value){
        m_timer->start();
    });
    connect(m_keyboardInterface,&KeyboardBackEndProxy::repeat_delayChanged,[this](int delay){
        if( m_delay!=delay )
        {
            KLOG_DEBUG() << "keyboard general setting repeat delay changed:" << delay;
            m_delay=delay;
            ui->hslider_delay->setValue(m_delay);
        }
    });


    // 重复键间隔设置
    m_interval = m_keyboardInterface->repeat_interval();
    ui->hslider_interval->setValue(m_interval);
    connect(ui->hslider_interval,&QSlider::valueChanged,[this](int value){
        m_timer->start();
    });
    connect(m_keyboardInterface,&KeyboardBackEndProxy::repeat_intervalChanged,[this](int interval){
        if (m_interval != (ui->hslider_interval->maximum() - interval + 10))
        {
            KLOG_DEBUG() << "keyboard general setting repeat interval changed:" << interval;
            m_interval = ui->hslider_interval->maximum() - interval + 10;;
            ui->hslider_interval->setValue(m_interval);
        }
    });
}

void GeneralPage::handleSaverTimerTimeOut()
{
    qint32 delay = ui->hslider_delay->value();
    qint32 interval = ui->hslider_interval->value();

    if (m_delay != delay)
    {
        m_delay = delay;
        m_keyboardInterface->setRepeat_delay(m_delay);
    }

    if (m_interval != interval)
    {
        m_interval = interval;
        auto value = ui->hslider_interval->maximum() - interval + 10;
        m_keyboardInterface->setRepeat_interval(value);
    }

    KLOG_DEBUG() << "keyboard general setting save:";
    KLOG_DEBUG() << "repeat delay   :" << delay;
    KLOG_DEBUG() << "repeat interval:" << interval;
}

void GeneralPage::handleSwitchRepeatKeyToggled(bool checked)
{
    ui->hslider_delay->setEnabled(checked);
    ui->hslider_interval->setEnabled(checked);
    if (m_repeateEnabled != checked)
    {
        m_repeateEnabled = checked;
        m_keyboardInterface->setRepeat_enabled(checked);
    }
}
