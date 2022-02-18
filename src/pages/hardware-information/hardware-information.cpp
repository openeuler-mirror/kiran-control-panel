/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
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

#include "hardware-information.h"
#include "dbus-wrapper/system-info-dbus.h"
#include "ui_hardware-information.h"

#include <kiran-log/qt5-log-i.h>
#include <kiran-system-daemon/systeminfo-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <math.h>
#include <QDate>
#include <QFontMetrics>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <Qt>
#include <iomanip>
#include <iostream>
#include <sstream>

#define SYSTEM_LOGO "KylinSec OS"
#define MEMORY "mem"
#define TOTAL_SIZE "total_size"
#define CPU "cpu"
#define DISKS "disks"
#define DISK_SIZE "size"
#define GRAPHICS "graphics"
#define MODEL "model"
#define VENDOR "vendor"
#define CORES_NUMBER "cores_number"
#define ETHS "eths"

using namespace std;

HardwareInformation::HardwareInformation(QWidget *parent) : QWidget(parent),
                                                            ui(new Ui::HardwareInformation)
{
    ui->setupUi(this);
    initUI();
}

HardwareInformation::~HardwareInformation()
{
    delete ui;
}

bool HardwareInformation::initUI(void)
{
    if (!readHardwareInfo(1))
    {
        return false;
    }
    return true;
}

void HardwareInformation::paintEvent(QPaintEvent *painEvent)
{
    QDate currentDate = QDate::currentDate();
    QString date = currentDate.toString("yyyy-MM-dd");
    QString year = date.left(4);
    QString copyright = QString(tr("Copyright ©")) + QString("%1 ").arg(year) + QString(tr("KylinSec. All rights reserved."));

    QPainter painter(this);
    QFont font = QFont("Noto Sans CJK SC regular", 46);
    QRect drawRecLogo = QRect(this->geometry().x() + 24, this->geometry().y() + 16, this->width(), ui->widget_logo->height() - 16);

    painter.setPen(QColor(46, 179, 255));  //#2eb3FF
    painter.setFont(font);
    painter.drawText(drawRecLogo, SYSTEM_LOGO);

    QFontMetrics fm = painter.fontMetrics();
    int heightText = fm.height();

    int offsetHeight = heightText + 5 + 16;
    QRect drawRecCopyright = QRect(24, this->geometry().y() + offsetHeight, this->width(), ui->widget_logo->height() - offsetHeight);
    font.setPointSize(10);
    font.setWeight(QFont::Normal);
    painter.setPen(QColor(145, 145, 145));
    painter.setFont(font);
    painter.drawText(drawRecCopyright, copyright);
}

/**
 * @brief HardwareInformation::readHardwareInfo 调用Dbus接口，读取系统信息，并解析，显示在界面上
 * @param infoType 读取的信息类型，0：系统信息 1：硬件信息
 */
bool HardwareInformation::readHardwareInfo(int infoType)
{
    QString systemInfo;
    QLabel *labelDisk = new QLabel();
    ;
    labelDisk->setText(tr("Unknow"));
    labelDisk->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";}");
    QLabel *labelGraphics = new QLabel();
    labelGraphics->setText(tr("Unknow"));
    labelGraphics->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";}");
    QLabel *labelEths = new QLabel();
    labelEths->setText(tr("Unknow"));
    labelEths->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";}");

    if (!InfoDbus::SystemInfo::getSystemInfo(infoType, systemInfo))
    {
        KLOG_ERROR() << "get hardware information failed";
        ui->label_CPU_info->setText(tr("Unknow"));
        ui->label_memory_info->setText(tr("Unknow"));

        ui->gridLayout_graphics_card->addWidget(labelGraphics, 1, 0, Qt::AlignRight);
        ui->gridLayout_hard_disk->addWidget(labelDisk, 1, 0, Qt::AlignRight);
        ui->gridLayout_network_card->addWidget(labelEths, 1, 0, Qt::AlignRight);
        return false;
    }
    else
    {
        KLOG_INFO() << systemInfo;
        ui->gridLayout_graphics_card->removeWidget(labelGraphics);
        ui->gridLayout_hard_disk->removeWidget(labelDisk);
        ui->gridLayout_network_card->removeWidget(labelEths);

        getJsonValueFromString(systemInfo);
        showListInfo();
        return true;
    }
}

/**
 * @brief GeneralFunctionsClass::getJsonValueFromString 解析后端传入的授权信息Json字符串
 * @param jsonString 需要转化的Json字符串
 */
void HardwareInformation::getJsonValueFromString(QString jsonString)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        KLOG_DEBUG() << " please check the activation information string " << jsonString.toLocal8Bit().data();
        return;
    }
    if (jsonDocument.isObject())
    {
        QJsonObject obj = jsonDocument.object();

        if (obj.contains(MEMORY))
        {
            QJsonValue value = obj.take(MEMORY);
            if (value.isObject())
            {
                QJsonObject object = value.toObject();
                if (object.contains(TOTAL_SIZE))
                {
                    QJsonValue value = object.value(TOTAL_SIZE);
                    if (value.isDouble())
                    {
                        double memory_size = value.toVariant().toDouble() / 1024 / 1024 / 1024;
                        stringstream ss;
                        ss << setiosflags(ios::fixed) << setprecision(2) << memory_size;  //保留两位小数
                        string str = ss.str();

                        QString size = QString("%1G").arg(QString::fromStdString(str));
                        ui->label_memory_info->setText(size);
                    }
                }
            }
        }
        if (obj.contains(CPU))
        {
            QJsonValue value = obj.take(CPU);
            if (value.isObject())
            {
                QString model;
                int coresNumber;
                QJsonObject object = value.toObject();
                if (object.contains(MODEL))
                {
                    QJsonValue value = object.value(MODEL);
                    if (value.isString())
                    {
                        model = value.toString();
                    }
                }
                if (object.contains(CORES_NUMBER))
                {
                    QJsonValue value = object.value(CORES_NUMBER);
                    if (value.isDouble())
                    {
                        coresNumber = value.toVariant().toInt();
                    }
                }
                QString cpuInfo = QString("%1 X %2").arg(model).arg(coresNumber);
                ui->label_CPU_info->setText(cpuInfo);
            }
        }
        if (obj.contains(DISKS))
        {
            QJsonValue value = obj.take(DISKS);
            if (value.isArray())
            {
                QJsonArray array = value.toArray();
                int nSize = array.size();
                for (int i = 0; i < nSize; ++i)
                {
                    QJsonValue value = array.at(i);
                    if (value.isObject())
                    {
                        QJsonObject object = value.toObject();
                        QString disk_model;
                        double size;
                        if (object.contains(MODEL))
                        {
                            QJsonValue value = object.value(MODEL);
                            if (value.isString())
                            {
                                disk_model = value.toString();
                            }
                        }
                        if (object.contains(DISK_SIZE))
                        {
                            QJsonValue value = object.value(DISK_SIZE);
                            if (value.isDouble())
                            {
                                double disk_size = value.toVariant().toDouble();
                                size = disk_size / 1024 / 1024 / 1024;
                            }
                        }
                        stringstream ss;
                        ss << setiosflags(ios::fixed) << setprecision(2) << size;  //保留两位小数
                        string str = ss.str();
                        ///FIXME:后续将界面要显示disk值，做成数组
                        QString diskInfo = QString("%1 (%2G)").arg(disk_model).arg(QString::fromStdString(str));
                        diskList << diskInfo;
                    }
                }
            }
        }
        if (obj.contains(GRAPHICS))
        {
            QJsonValue value = obj.take(GRAPHICS);
            if (value.isArray())
            {
                QJsonArray array = value.toArray();
                int nSize = array.size();
                for (int i = 0; i < nSize; ++i)
                {
                    QJsonValue value = array.at(i);
                    if (value.isObject())
                    {
                        QJsonObject object = value.toObject();
                        QString vendor;
                        QString model;
                        if (object.contains(VENDOR))
                        {
                            QJsonValue value = object.value(VENDOR);
                            if (value.isString())
                            {
                                vendor = value.toString();
                            }
                        }
                        if (object.contains(MODEL))
                        {
                            QJsonValue value = object.value(MODEL);
                            if (value.isString())
                            {
                                model = value.toString();
                            }
                        }
                        ///FIXME:后续将界面要显示graphics值，做成数组
                        QString graphicsInfo = QString("%1 (%2)").arg(model).arg(vendor);
                        graphicsList << graphicsInfo;
                    }
                }
            }
        }
        if (obj.contains(ETHS))
        {
            QJsonValue value = obj.take(ETHS);
            if (value.isArray())
            {
                QJsonArray array = value.toArray();
                int nSize = array.size();
                for (int i = 0; i < nSize; ++i)
                {
                    QJsonValue value = array.at(i);
                    if (value.isObject())
                    {
                        QJsonObject object = value.toObject();
                        QString vendor;
                        QString model;
                        if (object.contains(VENDOR))
                        {
                            QJsonValue value = object.value(VENDOR);
                            if (value.isString())
                            {
                                vendor = value.toString();
                            }
                        }
                        if (object.contains(MODEL))
                        {
                            QJsonValue value = object.value(MODEL);
                            if (value.isString())
                            {
                                model = value.toString();
                            }
                        }
                        QString ethsInfo = QString("%1 (%2)").arg(model).arg(vendor);
                        ethsList << ethsInfo;
                    }
                }
            }
        }
    }
}

/**
 * @brief HardwareInformation::showListInfo: 在硬件信息界面中显示所有的硬盘、网卡、显卡信息列表
 */
void HardwareInformation::showListInfo()
{
    int i;
    //根据数量设置各行的行高及左侧标签的布局
    int diskNum = diskList.size();
    if (diskNum > 0)
    {
        ui->label_hard_disk->setFixedHeight(40 * diskNum);
    }

    int graphicsNum = graphicsList.size();
    if (graphicsNum > 0)
    {
        ui->label_graphics_card->setFixedHeight(40 * graphicsNum);
    }

    int ethsNum = ethsList.size();
    if (ethsNum > 0)
    {
        ui->label_network_card->setFixedHeight(40 * ethsNum);
    }

    if (diskNum > 1)
    {
        ui->label_hard_disk->setAlignment(Qt::AlignTop);
        ui->label_hard_disk->setStyleSheet("QLabel{padding-top:10px;}");
    }
    if (graphicsNum > 1)
    {
        ui->label_graphics_card->setAlignment(Qt::AlignTop);
        ui->label_graphics_card->setStyleSheet("QLabel{padding-top:10px;}");
    }
    if (ethsNum > 1)
    {
        ui->label_network_card->setAlignment(Qt::AlignTop);
        ui->label_network_card->setStyleSheet("QLabel{padding-top:10px;}");
    }

    //依次将各个信息插入至布局
    for (i = 0; i < diskNum; i++)
    {
        KLOG_INFO() << diskList.at(i);
        QLabel *labelDisk = new QLabel(diskList.at(i));
        //设置QLabel的样式
        labelDisk->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";}");
        ui->gridLayout_hard_disk->addWidget(labelDisk, i, 0, Qt::AlignRight);
    }

    for (i = 0; i < graphicsNum; i++)
    {
        KLOG_INFO() << graphicsList.at(i);
        QLabel *labelGraphics = new QLabel(graphicsList.at(i));
        //设置QLabel的样式
        labelGraphics->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";}");
        ui->gridLayout_graphics_card->addWidget(labelGraphics, i, 0, Qt::AlignRight);
    }
    for (i = 0; i < ethsNum; i++)
    {
        KLOG_INFO() << ethsList.at(i);
        QLabel *labelEths = new QLabel(ethsList.at(i));
        //设置QLabel的样式
        labelEths->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";}");
        ui->gridLayout_network_card->addWidget(labelEths, i, 0, Qt::AlignRight);
    }
}
