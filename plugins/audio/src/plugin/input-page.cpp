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
#include "input-page.h"
#include "dbus/audio-device-interface.h"
#include "dbus/audio-interface.h"
#include "ui_input-page.h"

#include <kiran-session-daemon/audio-i.h>
#include <qt5-log-i.h>

AudioInfo::AudioInfo(const QAudioFormat &format, QObject *parent)
    : QIODevice(parent), m_format(format), m_maxAmplitude(0), m_level(0.0)
{
    switch (m_format.sampleSize())
    {
    case 8:
        switch (m_format.sampleType())
        {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 255;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 127;
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (m_format.sampleType())
        {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 65535;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;
    case 32:
        switch (m_format.sampleType())
        {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 0xffffffff;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 0x7fffffff;
            break;
        case QAudioFormat::Float:
            m_maxAmplitude = 0x7fffffff;
            break;
        default:
            break;
        }
    default:
        break;
    }
}

AudioInfo::~AudioInfo()
{
}

void AudioInfo::start()
{
    open(QIODevice::WriteOnly);
}

void AudioInfo::stop()
{
    close();
}

qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    return 0;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
    if (m_maxAmplitude)
    {
        Q_ASSERT(m_format.sampleSize() % 8 == 0);
        const int channelBytes = m_format.sampleSize() / 8;
        const int sampleBytes = m_format.channelCount() * channelBytes;
        Q_ASSERT(len % sampleBytes == 0);
        const int numSamples = len / sampleBytes;

        quint32 maxValue = 0;
        const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

        for (int i = 0; i < numSamples; ++i)
        {
            for (int j = 0; j < m_format.channelCount(); ++j)
            {
                quint32 value = 0;

                if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt)
                {
                    value = *reinterpret_cast<const quint8 *>(ptr);
                }
                else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt)
                {
                    value = qAbs(*reinterpret_cast<const qint8 *>(ptr));
                }
                else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint16>(ptr);
                    else
                        value = qFromBigEndian<quint16>(ptr);
                }
                else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint16>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint16>(ptr));
                }
                else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint32>(ptr);
                    else
                        value = qFromBigEndian<quint32>(ptr);
                }
                else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint32>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint32>(ptr));
                }
                else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float)
                {
                    value = qAbs(*reinterpret_cast<const float *>(ptr) * 0x7fffffff);  // assumes 0-1.0
                }

                maxValue = qMax(value, maxValue);
                ptr += channelBytes;
            }
        }

        maxValue = qMin(maxValue, m_maxAmplitude);
        m_level = qreal(maxValue) / m_maxAmplitude;
    }

    emit update();
    return len;
}

InputPage::InputPage(QWidget *parent) : QWidget(parent), ui(new Ui::InputPage)
{
    ui->setupUi(this);
    m_audioInterface = AudioInterface::instance();
    ui->inputVolume->setStyleSheet("color:#2eb3ff;");
    initInputDevice();
    initInputSettins();
    if(isValidPort)
        initVoulumeFeedBack();
    initConnet();
}

InputPage::~InputPage()
{
    delete ui;
}

void InputPage::initInputDevice()
{
    QDBusPendingReply<QStringList> getSources = m_audioInterface->GetSources();
    QStringList sourcesList = getSources.value();

    QDBusPendingReply<QString> defaultSourcePath = m_audioInterface->GetDefaultSource();
    KLOG_DEBUG() << "defaultSourcePath" << defaultSourcePath;

    m_defaultSource = new AudioDeviceInterface(AUDIO_DBUS_NAME, defaultSourcePath, QDBusConnection::sessionBus(), this);
    m_defaultSourceIndex = m_defaultSource->index();
    initActivedPort();
}

void InputPage::initActivedPort()
{
    QDBusPendingReply<QString> getPorts = m_defaultSource->GetPorts();
    KLOG_DEBUG() << "getPorts:" << getPorts;
    //解析默认source的端口信息
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

                ui->inputDevices->insertItem(i, description);
                ui->inputDevices->setItemData(i, name, Qt::UserRole);  //激活端口所需信息
                //获取已激活的端口在comobox中的index
                if (m_defaultSource->active_port() == name)
                {
                    m_defaultDeviceIndex = i;
                    KLOG_DEBUG() << "m_defaultDeviceIndex" << m_defaultDeviceIndex;
                }
            }
        }
        //默认选中已激活的端口
        isValidPort = true;
        ui->inputDevices->setCurrentIndex(m_defaultDeviceIndex);
        ui->inputDevices->setEnabled(true);
        ui->volumeSetting->setEnabled(true);
    }
    else
    {
        KLOG_DEBUG() << "ports is null";
        isValidPort = false;
        ui->inputDevices->insertItem(0, tr("No input device detected"));
        ui->inputDevices->setEnabled(false);
        ui->volumeSetting->setValue(0);
        ui->volumeSetting->setEnabled(false);
    }
}

void InputPage::initInputSettins()
{
    ui->volumeSetting->setRange(0, 100);
    ui->volumeSetting->setSingleStep(1);
    ui->volumeSetting->setPageStep(1);

    double currentVolumeDouble = m_defaultSource->volume() * 100;
    int currentVolume = round(currentVolumeDouble);
    ui->volumeSetting->setValue(currentVolume);
    ui->inputVolume->setText(QString::number(currentVolume) + "%");

    connect(ui->volumeSetting, &QSlider::valueChanged, [=](int value) {
        double volumeValue = static_cast<double>(value) / static_cast<double>(100);
        m_defaultSource->SetVolume(volumeValue);
    });
}

void InputPage::initConnet()
{
    connect(ui->inputDevices, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), [=](int index) {
        KLOG_DEBUG() << "initConnet activated index:" << index;
        QString namePort = ui->inputDevices->itemData(index, Qt::UserRole).toString();
        if (!namePort.isNull())
        {
            m_defaultSource->SetActivePort(namePort);
            KLOG_DEBUG() << "SetActivePort:" << namePort;
        }
        else
            KLOG_DEBUG() << "namePort is null";
    });
    connect(m_defaultSource, &AudioDeviceInterface::volumeChanged, [=](double value) {
        handleVolumeChanged(value);
    });
    connect(m_defaultSource, &AudioDeviceInterface::active_portChanged, [=](const QString &value) {
        handleActivePortChanged(value);
    });

    //Fix:SourceAdded 和 SourceDelete没有被激发
    connect(m_audioInterface, &AudioInterface::SourceAdded, [this](uint index) {
        handleSourceAdded(index);
    });
    connect(m_audioInterface, &AudioInterface::SourceDelete, [this](uint index) {
        handleSourceDelete(index);
    });
    connect(m_audioInterface, &AudioInterface::DefaultSourceChange, this,&InputPage::handleDefaultSourceChanged,Qt::QueuedConnection);
}

void InputPage::handleActivePortChanged(const QString &value)
{
    KLOG_DEBUG() << "handleActivePortChanged :" << value;
    for (int i = 0; i < ui->inputDevices->count(); ++i)
    {
        QString name = ui->inputDevices->itemData(i, Qt::UserRole).toString();
        if (name == value)
        {
            ui->inputDevices->setCurrentIndex(i);
            KLOG_DEBUG() << "handleActivePortChanged  setCurrentIndex " << i;
        }
    }
}

void InputPage::handleVolumeChanged(double value)
{
    ui->volumeSetting->blockSignals(true);
    int currentVolume = round(value * 100);
    KLOG_DEBUG() << "input-page:" << currentVolume;
    ui->inputVolume->setText(QString::number(currentVolume) + "%");
    ui->volumeSetting->setValue(currentVolume);
    ui->volumeSetting->blockSignals(false);
}

/*
 * TODO:
 * 1、处理快速拔插设备
 * 2、设备插入后是否需要等待设备准备好
 * */

void InputPage::handleDefaultSourceChanged(int index)
{
    KLOG_DEBUG() << "DefaultSourceChanged:" << index;
    //delete and restart init defaultSource
    m_defaultSource->deleteLater();
    m_defaultSource = nullptr;

    ui->inputDevices->clear();
    initInputDevice();
    initInputSettins();

    ui->volumeScale->setPercent(0);

    if(m_audioInfo != nullptr)
    {
        m_audioInfo->stop();
        m_audioInfo->deleteLater();
        m_audioInfo = nullptr;
    }
    if(m_audioInput != nullptr)
    {
        m_audioInput->deleteLater();
        m_audioInput = nullptr;
    }
    if(isValidPort)
    {
        initVoulumeFeedBack();
    }
    initConnet();
}

//暂时没有处理Source增加减少的需求
void InputPage::handleSourceAdded(int index)
{
    KLOG_DEBUG() << "Source Added:" << index;
}

void InputPage::handleSourceDelete(int index)
{
    KLOG_DEBUG() << "Source Delete:" << index;
}

QSize InputPage::sizeHint() const
{
    return {500, 657};
}

//通过QMultimedia模块获取反馈音量
//首先使用QAudioFormat类设置音频流参数信息，然后从QIODevice中读取PCM数据
//最后使用QAudioInput类接收从输入设备来的音频数据
void InputPage::initVoulumeFeedBack()
{
    initAudioFormat();
    m_audioInfo = new AudioInfo(m_format, this);
    connect(m_audioInfo, &AudioInfo::update, this, &InputPage::refreshFeedBack);
    initAudioInput();
}

void InputPage::initAudioFormat()
{
    m_format.setSampleRate(8000);
    m_format.setChannelCount(2);
    m_format.setSampleSize(16);
    m_format.setSampleType(QAudioFormat::SignedInt);
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setCodec("audio/pcm");

    m_device = QAudioDeviceInfo::defaultInputDevice();
    //Constructs a copy of other.
    QAudioDeviceInfo info(m_device);
    if (!info.isFormatSupported(m_format))
    {
        KLOG_WARNING() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }
}

void InputPage::initAudioInput()
{
    m_audioInput = new QAudioInput(m_device, m_format, this);
    m_audioInfo->start();
    m_audioInput->start(m_audioInfo);
}

//XXX: QIODevice一直在监听PCM数据，可以优化一下,或许100ms获取一次数据
void InputPage::refreshFeedBack()
{
    ui->volumeScale->setPercent(m_audioInfo->level());
}
