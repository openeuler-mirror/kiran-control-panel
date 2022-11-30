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

#include "hardware-information.h"
#include "dbus-wrapper/system-info-dbus.h"
#include "ui_hardware-information.h"

#include <kiran-log/qt5-log-i.h>
#include <kiran-system-daemon/systeminfo-i.h>

#include <QDate>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPainter>
#include <QEvent>
#include <QResizeEvent>

HardwareInformation::HardwareInformation(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::HardwareInformation)
{
    ui->setupUi(this);
    initUI();

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->viewport()->installEventFilter(this);
}

HardwareInformation::~HardwareInformation()
{
    delete ui;
}

bool HardwareInformation::eventFilter(QObject *watched, QEvent *event)
{
    if( watched==ui->scrollArea->viewport() && event->type()==QEvent::Resize )
    {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent *>(event);
        QWidget *contentWidget = ui->scrollArea->widget();
        if (contentWidget)
        {
            contentWidget->setFixedWidth(resizeEvent->size().width());
        }
    }
    return false;
}

void HardwareInformation::initUI(void)
{
    QString hardwareJson, memory, cpu;
    QStringList disks, graphics, eths;

    bool bRes = SystemInfoDBus::getSystemInfo(SYSTEMINFO_TYPE_HARDWARE, hardwareJson);
    if (bRes && parseHardwareInfoJson(hardwareJson, memory, cpu, disks, graphics, eths))
    {
        if (disks.count())
        {
            ui->label_hard_disk->setFixedHeight(36 * disks.count());
        }

        if (graphics.count())
        {
            ui->label_graphics_card->setFixedHeight(36 * graphics.count());
        }

        if (eths.count())
        {
            ui->label_network_card->setFixedHeight(36 * eths.count());
        }

        ui->label_memory_info->setText(memory);
        ui->label_memory_info->setToolTip(memory);

        ui->label_CPU_info->setText(cpu);
        ui->label_CPU_info->setToolTip(cpu);

        QList<std::tuple<QStringList, QGridLayout *> > hardwareMap = {
            {disks, ui->gridLayout_hard_disk},
            {graphics, ui->gridLayout_graphics_card},
            {eths, ui->gridLayout_network_card}};
        for (auto hardwareInitTuple : hardwareMap)
        {
            QStringList infos = std::get<0>(hardwareInitTuple);
            QGridLayout *layout = std::get<1>(hardwareInitTuple);
            for (const QString &hardwareItem : infos)
            {
                auto label = new KiranLabel(hardwareItem);
                label->setToolTip(hardwareItem);
                label->setElideMode(Qt::ElideRight);
                layout->addWidget(label, layout->count(), 0, Qt::AlignRight);
            }
        }
    }
    else
    {
        ui->label_CPU_info->setText(tr("Unknow"));
        ui->label_memory_info->setText(tr("Unknow"));
        QSet<QGridLayout *> gridLayoutSet = {
            ui->gridLayout_hard_disk,
            ui->gridLayout_graphics_card,
            ui->gridLayout_network_card};
        for (auto layout : gridLayoutSet)
        {
            auto label = new QLabel();
            label->setText(tr("Unknow"));
            layout->addWidget(label, 1, 0, Qt::AlignRight);
        }
    }

    auto kiranFrames = findChildren<KiranFrame *>();
    for (auto frame : kiranFrames)
    {
        frame->setRadius(6);
        frame->setDrawBroder(false);
    }

    QList<QLabel *> labels = {ui->label_CPU_info, ui->label_memory_info};
    for (auto label : labels)
    {
        label->setStyleSheet("color:#919191;font-family: \"Noto Sans CJK SC regular\";");
    }

    QList<QLayout *> layouts = {ui->gridLayout_hard_disk, ui->gridLayout_graphics_card, ui->gridLayout_network_card};
    for (auto layout : layouts)
    {
        for (int i = 0; i < layout->count(); i++)
        {
            auto item = layout->itemAt(i);
            auto label = item->widget();
            label->setStyleSheet("color:#919191;font-family: \"Noto Sans CJK SC regular\";");
        }
    }
}

bool HardwareInformation::parseHardwareInfoJson(const QString &json,
                                                QString &memory,
                                                QString &cpu,
                                                QStringList &disks,
                                                QStringList &graphics,
                                                QStringList &eths)
{
    QJsonParseError jsonError;

    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError || !jsonDocument.isObject())
    {
        KLOG_ERROR() << " please check the activation information string " << json.toLocal8Bit().data();
        return false;
    }

    QJsonObject rootObj = jsonDocument.object();

    //memory
    if (rootObj.contains("mem") && rootObj["mem"].isObject())
    {
        QJsonObject memObj = rootObj["mem"].toObject();
        if (memObj.contains("total_size"))
        {
            double totalSize = memObj["total_size"].toDouble();
            double memorySize = totalSize / 1024 / 1024 / 1024;
            memory = QString("%1G").arg(QString::number(memorySize, 'f', 2));
        }
    }

    //cpu
    if (rootObj.contains("cpu") && rootObj["cpu"].isObject())
    {
        QJsonObject cpuObj = rootObj["cpu"].toObject();
        if (cpuObj.contains("cores_number") && cpuObj.contains("model"))
        {
            int coresNumber = cpuObj["cores_number"].toInt();
            QString model = cpuObj["model"].toString();
            cpu = QString("%1 X %2").arg(model).arg(coresNumber);
        }
    }

    //disk
    if (rootObj.contains("disks") && rootObj["disks"].isArray())
    {
        QJsonArray disksArray = rootObj["disks"].toArray();
        for (auto &&diskValue : disksArray)
        {
            QJsonObject diskObj = diskValue.toObject();
            if (diskObj.contains("model") && diskObj.contains("size"))
            {
                QString model = diskObj["model"].toString();
                double size = diskObj["size"].toDouble();
                double diskSize = size / 1024 / 1024 / 1024;
                QString diskInfo = QString("%1 (%2G)").arg(model).arg(QString::number(diskSize, 'f', 2));
                disks << diskInfo;
            }
        }
    }

    //graphics
    if (rootObj.contains("graphics") && rootObj["graphics"].isArray())
    {
        QJsonArray graphicsArray = rootObj["graphics"].toArray();
        for(auto &&graphicsValue:graphicsArray)
        {
            QJsonObject graphicsObject = graphicsValue.toObject();
            if( graphicsObject.contains("model") && graphicsObject.contains("vendor") )
            {
                QString model = graphicsObject["model"].toString();
                QString vendor = graphicsObject["vendor"].toString();
                QString graphicsInfo = QString("%1 (%2)").arg(model).arg(vendor);
                graphics << graphicsInfo;
            }
        }
    }

    //eth
    if (rootObj.contains("eths") && rootObj["eths"].isArray())
    {
        QJsonArray ethsArray = rootObj["eths"].toArray();
        for(auto &&ethValue:ethsArray)
        {
            QJsonObject ethObject = ethValue.toObject();
            if( ethObject.contains("vendor") && ethObject.contains("model") )
            {
                QString model = ethObject["model"].toString();
                QString vendor = ethObject["vendor"].toString();
                QString ethInfo = QString("%1 (%2)").arg(model).arg(vendor);
                eths << ethInfo;
            }
        }
    }

    return true;
}

QString HardwareInformation::elideText(const QString &src)
{
    QFontMetrics fontMetrics(QFont("Noto Sans CJK SC regular"));
    return fontMetrics.elidedText(src, Qt::ElideRight, 300);
}
