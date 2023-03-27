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

#include "output-page.h"
#include "dbus/audio-device-interface.h"
#include "dbus/audio-interface.h"
#include "kiran-session-daemon/audio-i.h"
#include "ui_output-page.h"

#include <qt5-log-i.h>
#include <QComboBox>

OutputPage::OutputPage(QWidget *parent) : QWidget(parent), ui(new Ui::OutputPage)
{
    ui->setupUi(this);
    ui->outputVolume->setStyleSheet("color:#2eb3ff;");
    m_audioInterface = AudioInterface::instance();
    initOutputDevice();
    initOutputSettins();
    initConnect();
}

OutputPage::~OutputPage()
{
    delete ui;
}

void OutputPage::initOutputDevice()
{
    QDBusPendingReply<QStringList> getSinks = m_audioInterface->GetSinks();
    QStringList sinksList = getSinks.value();

    QDBusPendingReply<QString> defaultSinkPath = m_audioInterface->GetDefaultSink();
    KLOG_DEBUG() << "defaultSink" << defaultSinkPath;

    m_defaultSink = new AudioDeviceInterface(AUDIO_DBUS_NAME, defaultSinkPath, QDBusConnection::sessionBus(), this);
    m_defaultSinkIndex = m_defaultSink->index();
    initActivedPort();
}

void OutputPage::initActivedPort()
{
    QDBusPendingReply<QString> getPorts = m_defaultSink->GetPorts();
    KLOG_DEBUG() << "getPorts:" << getPorts;

    //解析默认sink的端口信息
    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(getPorts.value().toLatin1(), &jsonParseError);
    if (!doc.isNull() && jsonParseError.error == QJsonParseError::NoError)
    {
        if (doc.isArray() && jsonParseError.error == QJsonParseError::NoError)
        {
            QJsonArray array = doc.array();
            for (int i = 0; i < array.count(); ++i)
            {
                KLOG_DEBUG() << "array.at(i)" << array.at(i);
                QJsonObject object = array.at(i).toObject();
                QString description = object.value("description").toString();
                QString name = object.value("name").toString();
                double priority = object.value("priority").toDouble();
                KLOG_DEBUG() << "description" << description;
                KLOG_DEBUG() << "name" << name;
                KLOG_DEBUG() << "priority" << priority;
                ui->outputDevices->insertItem(i, description);
                ui->outputDevices->setItemData(i, name, Qt::UserRole);  //激活端口所需信息

                //获取已激活的端口在comobox中的index
                if (m_defaultSink->active_port() == name)
                {
                    m_defaultDeviceIndex = i;
                    KLOG_DEBUG() << "m_defaultDeviceIndex" << m_defaultDeviceIndex;
                }
            }
        }
        //默认选中已激活的端口
        ui->outputDevices->setCurrentIndex(m_defaultDeviceIndex);
        ui->outputDevices->setEnabled(true);
        ui->volumeSetting->setEnabled(true);
        ui->volumeBalance->setEnabled(true);
    }
    else
    {
        //无激活端口则禁用音量设置和平衡
        KLOG_DEBUG() << "default sink ports is null";
        ui->outputDevices->insertItem(0, tr("No output device detected"));
        ui->outputDevices->setEnabled(false);
        ui->volumeSetting->setValue(0);
        ui->volumeSetting->setEnabled(false);
        ui->volumeBalance->setValue(0);
        ui->volumeBalance->setEnabled(false);
    }
}

void OutputPage::initOutputSettins()
{
    initVolumeSetting();
    initBalanceSetting();
}

void OutputPage::initVolumeSetting()
{
    ui->volumeSetting->setRange(0, 100);
    ui->volumeSetting->setSingleStep(1);
    ui->volumeSetting->setPageStep(1);

    double currentVolumeDouble = m_defaultSink->volume() * 100;
    int currentVolume = round(currentVolumeDouble);
    ui->volumeSetting->setValue(currentVolume);
    ui->outputVolume->setText(QString::number(currentVolume) + "%");
}

void OutputPage::initBalanceSetting()
{
    ui->volumeBalance->setRange(-100, 100);
    ui->volumeBalance->setSingleStep(1);
    ui->volumeBalance->setPageStep(1);

    KLOG_DEBUG() << "current balance:" << m_defaultSink->balance();
    double currentBalanceDouble = m_defaultSink->balance() * 100;
    int currentBalance = round(currentBalanceDouble);
    ui->volumeBalance->setValue(currentBalance);
}

void OutputPage::initConnect()
{
    connect(m_defaultSink, &AudioDeviceInterface::volumeChanged, this, &OutputPage::handleVolumeChanged);
    connect(m_defaultSink, &AudioDeviceInterface::balanceChanged, this, &OutputPage::handleBalanceChanged);
    connect(m_defaultSink, &AudioDeviceInterface::active_portChanged, this, &OutputPage::handleActivePortChanged);
    connect(m_audioInterface, &AudioInterface::SinkAdded, this, &OutputPage::handleSinkAdded);
    connect(m_audioInterface, &AudioInterface::SinkDelete, this, &OutputPage::handleSinkDelete);
    connect(m_audioInterface, &AudioInterface::DefaultSinkChange, this, &OutputPage::handleDefaultSinkChanged, Qt::QueuedConnection);

    connect(ui->outputDevices, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index)
            {
                QString namePort = ui->outputDevices->itemData(index, Qt::UserRole).toString();
                KLOG_DEBUG() << "SetActivePort:" << namePort;
                if(m_defaultSink != nullptr)
                    m_defaultSink->SetActivePort(namePort); 
                else
                    KLOG_DEBUG() << "m_defaultSink is null"; });

    connect(ui->volumeSetting, &QSlider::valueChanged, [this](int value)
            {
                double volumeValue = static_cast<double>(ui->volumeSetting->sliderPosition()) / static_cast<double>(100);
                if(m_defaultSink != nullptr)
                {
                    m_defaultSink->SetVolume(volumeValue); 
                    KLOG_DEBUG() << "SetVolume:" << volumeValue;
                }  
                else
                    KLOG_DEBUG() << "m_defaultSink is null"; });

    connect(ui->volumeBalance, &QSlider::valueChanged, [this](int value)
            {
                double balanceValue = static_cast<double>(value) / static_cast<double>(100);
                if (m_defaultSink != nullptr)
                {
                    m_defaultSink->SetBalance(balanceValue);
                    KLOG_DEBUG() << "balanceValue" << balanceValue;
                }
                else
                    KLOG_DEBUG() << "m_defaultSink is null"; });
}

void OutputPage::handleActivePortChanged(const QString &value)
{
    KLOG_DEBUG() << "handleActivePortChanged :" << value;
    for (int i = 0; i < ui->outputDevices->count(); ++i)
    {
        QString name = ui->outputDevices->itemData(i, Qt::UserRole).toString();
        if (name == value)
        {
            ui->outputDevices->setCurrentIndex(i);
            KLOG_DEBUG() << "handleActivePortChanged  setCurrentIndex " << i;
        }
    }
}

void OutputPage::handleVolumeChanged(double value)
{
    ui->volumeSetting->blockSignals(true);  //为了避免拖动的同时设置位置会出现问题
    int currentVolume = round(value * 100);
    ui->outputVolume->setText(QString::number(currentVolume) + "%");
    ui->volumeSetting->setValue(currentVolume);
    ui->volumeSetting->blockSignals(false);
}

void OutputPage::handleBalanceChanged(double value)
{
    ui->volumeBalance->blockSignals(true);
    int currentBalance = round(value * 100);
    ui->volumeBalance->setValue(currentBalance);
    ui->volumeBalance->blockSignals(false);
}

void OutputPage::handleSinkAdded(int index)
{
    KLOG_DEBUG() << "SinkAdded";
}

void OutputPage::handleDefaultSinkChanged(int index)
{
    // delete and restart init defaultSource
    m_defaultSink->deleteLater();
    m_defaultSink = nullptr;
    ui->outputDevices->blockSignals(true);
    ui->outputDevices->clear();
    ui->outputDevices->blockSignals(false);

    initOutputDevice();
    initOutputSettins();

    connect(m_defaultSink, &AudioDeviceInterface::volumeChanged, this, &OutputPage::handleVolumeChanged);
    connect(m_defaultSink, &AudioDeviceInterface::balanceChanged, this, &OutputPage::handleBalanceChanged);
    connect(m_defaultSink, &AudioDeviceInterface::active_portChanged, this, &OutputPage::handleActivePortChanged);
}

void OutputPage::handleSinkDelete(int index)
{
    KLOG_DEBUG() << "SinkDelete";
}

QSize OutputPage::sizeHint() const
{
    return {500, 657};
}
