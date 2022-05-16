/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-keyboard is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "general-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QSharedPointer>
#include <iostream>
#include "dbus-wrapper/dbus-wrapper.h"
#include "keyboard_backEnd_proxy.h"
#include "ui_general-page.h"

#define TIMEOUT 100
#define PAGE_BASE_SIZE_WIDTH 636
#define PAGE_BASE_SIZE_HEIGHT 670

GeneralPage::GeneralPage(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::GeneralPage)
{
    ui->setupUi(this);

    DbusWrapper *dbusWrapper = new DbusWrapper;
    m_keyboardInterface = dbusWrapper->getKeyboardInterface();
    dbusWrapper->deleteLater();
    dbusWrapper = nullptr;

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            [this] {
                qint32 delay = ui->hslider_delay->value();
                qint32 interval = ui->hslider_interval->value();
                KLOG_INFO() << "delay = :" << delay;
                KLOG_INFO() << "interval = :" << interval;
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
                m_timer->stop();
            });

    initUI();
}

GeneralPage::~GeneralPage()
{
    delete ui;
}

QSize GeneralPage::sizeHint() const
{
    return QSize(PAGE_BASE_SIZE_WIDTH, PAGE_BASE_SIZE_HEIGHT);
}

void GeneralPage::initUI()
{
    ui->lineEdit_key->setPlaceholderText(tr("Enter characters to test the settings"));
    initComponentValue();
}

void GeneralPage::initComponentValue()
{
    //获取重复键设置
    m_repeateEnabled = m_keyboardInterface->repeat_enabled();
    ui->checkBox->setChecked(m_repeateEnabled);
    if (!m_repeateEnabled)
    {
        setWidgetsStatus(m_repeateEnabled);
    }
    connect(ui->checkBox, &KiranSwitchButton::toggled,
            [this](bool status) {
                setWidgetsStatus(status);
                if (m_repeateEnabled != status)
                {
                    m_repeateEnabled = status;
                    m_keyboardInterface->setRepeat_enabled(status);
                }
            });
    connect(m_keyboardInterface.data(), &KeyboardBackEndProxy::repeat_enabledChanged,
            [this](bool isEnabled) {
                //更新界面
                m_repeateEnabled = isEnabled;
                ui->checkBox->setChecked(isEnabled);
                KLOG_INFO() << "get repeat_enabledChanged signal:  " << isEnabled;
            });

    //延时
    m_delay = m_keyboardInterface->repeat_delay();
    KLOG_INFO() << m_delay;
    ui->hslider_delay->setValue(m_delay);
    connect(ui->hslider_delay, &QSlider::valueChanged,
            [this] {
                m_timer->start(TIMEOUT);
            });
    connect(m_keyboardInterface.data(), &KeyboardBackEndProxy::repeat_delayChanged,
            [this](int value) {
                //更新界面
                if (m_delay != value)
                {
                    m_delay = value;
                    ui->hslider_delay->setValue(value);
                }
                KLOG_INFO() << "get repeat_delayChanged signal: " << value;
            });

    //速度
    qint32 value = m_keyboardInterface->repeat_interval();
    m_interval = ui->hslider_interval->maximum() - value + 10;
    KLOG_INFO() << m_interval;
    ui->hslider_interval->setValue(m_interval);
    connect(ui->hslider_interval, &QSlider::valueChanged,
            [this] {
                m_timer->start(TIMEOUT);
            });
    connect(m_keyboardInterface.data(), &KeyboardBackEndProxy::repeat_intervalChanged,
            [this](qint32 value) {
                //更新界面
                if (m_interval != (ui->hslider_interval->maximum() - value + 10))
                {
                    m_interval = ui->hslider_interval->maximum() - value + 10;
                    ui->hslider_interval->setValue(m_interval);
                }
                KLOG_INFO() << "get repeat_intervalChanged signal: " << value;
            });
}

void GeneralPage::setWidgetsStatus(bool status)
{
    ui->hslider_delay->setDisabled(!status);
    ui->hslider_interval->setDisabled(!status);
}
