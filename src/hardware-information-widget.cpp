/**
 * @file hardware-information-widget.cpp
 * @brief 显示硬件信息控件
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "hardware-information-widget.h"
#include "ui_hardware-information-widget.h"
#include "system-info-dbus.h"

#include <iostream>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <kiran-cc-daemon/kiran-system-daemon/systeminfo_i.h>

#define MEMORY          "mem"
#define TOTAL_SIZE      "total_size"
#define CPU             "cpu"
#define DISKS           "disks"
#define DISK_SIZE       "size"
#define GRAPHICS        "graphics"
#define MODEL           "model"
#define VENDOR          "vendor"
#define CORES_NUMBER    "cores_number"
#define ETHS            "eths"

using namespace std;

HardwareInformationWidget::HardwareInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HardwareInformationWidget)
{
    ui->setupUi(this);
    initUI();
    readHardwareInfo(1);
    showListInfo();

}

HardwareInformationWidget::~HardwareInformationWidget()
{
    delete ui;
}

void HardwareInformationWidget::initUI(void)
{

}

/**
 * @brief HardwareInformationWidget::readHardwareInfo 调用Dbus接口，读取系统信息，并解析，显示在界面上
 * @param infoType 读取的信息类型，0：系统信息 1：硬件信息
 */
void HardwareInformationWidget::readHardwareInfo(int infoType)
{
    QString systemInfo;
    if(!InfoDbus::SystemInfo::getSystemInfo(infoType , systemInfo))
    {
        qDebug() << "get hardware information failed"<< endl;
        ui->label_CPU_info->setText(tr("Unknow"));
        ui->label_memory_info->setText(tr("Unknow"));
        ui->label_hard_disk_info->setText(tr("Unknow"));
        ui->label_graphics_card_info->setText(tr("Unknow"));
        ui->label_network_card_info->setText(tr("Unknow"));
        return;
    }
    else
    {
        qInfo() << systemInfo << endl;
        getJsonValueFromString(systemInfo);
    }
}

/**
 * @brief GeneralFunctionsClass::getJsonValueFromString 解析后端传入的授权信息Json字符串
 * @param jsonString 需要转化的Json字符串
 */
void HardwareInformationWidget::getJsonValueFromString(QString jsonString)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(),&jsonError);
    if( jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError ){
        qDebug()<< " please check the license information string "<< jsonString.toLocal8Bit().data();
        return;
    }
    if(jsonDocument.isObject())
    {
       QJsonObject obj = jsonDocument.object();

       if(obj.contains(MEMORY))
       {
           QJsonValue value = obj.take(MEMORY);
           if(value.isObject())
           {
               QJsonObject object = value.toObject();
               if(object.contains(TOTAL_SIZE))
               {
                   QJsonValue value = object.value(TOTAL_SIZE);
                   if(value.isDouble())
                   {
                       unsigned int memory_size = value.toVariant().toUInt();
                       QString size = QString::number(memory_size);
                       ui->label_memory_info->setText(size);
                   }
               }
           }
       }
       if(obj.contains(CPU))
       {
           QJsonValue value = obj.take(CPU);
           if(value.isObject())
           {
               QString model;
               int coresNumber;
               QJsonObject object = value.toObject();
               if(object.contains(MODEL))
               {
                   QJsonValue value = object.value(MODEL);
                   if(value.isString())
                   {
                       model = value.toString();
                   }
               }
               if(object.contains(CORES_NUMBER))
               {
                   QJsonValue value = object.value(CORES_NUMBER);
                   if(value.isDouble())
                   {
                       coresNumber = value.toVariant().toInt();
                   }
               }
               QString cpuInfo = QString("%1 X %2").arg(model).arg(coresNumber);
               ui->label_CPU_info->setText(cpuInfo);

           }
       }
       if(obj.contains(DISKS))
       {
           QJsonValue value = obj.take(DISKS);
           if(value.isArray())
           {
               QJsonArray array = value.toArray();
               int nSize = array.size();
               for (int i = 0; i < nSize; ++i)
               {
                   QJsonValue value = array.at(i);
                   if(value.isObject())
                   {
                       QJsonObject object = value.toObject();
                       QString disk_model;
                       QString size ;
                       if(object.contains(MODEL))
                       {
                           QJsonValue value = object.value(MODEL);
                           if(value.isString())
                           {
                               disk_model = value.toString();
                           }
                       }
                       if(object.contains(DISK_SIZE))
                       {
                           QJsonValue value = object.value(DISK_SIZE);
                           if(value.isDouble())
                           {
                               unsigned int disk_size = value.toVariant().toUInt();
                               size = QString::number(disk_size);
                           }
                       }
                       ///FIXME:后续将界面要显示disk值，做成数组
                       QString diskInfo = QString("%1 (%2)").arg(disk_model).arg(size);
                       diskList << diskInfo;
                   }
               }
           }
       }
       if(obj.contains(GRAPHICS))
       {
           QJsonValue value = obj.take(GRAPHICS);
           if(value.isArray())
           {
               QJsonArray array = value.toArray();
               int nSize = array.size();
               for (int i = 0; i < nSize; ++i)
               {
                   QJsonValue value = array.at(i);
                   if(value.isObject())
                   {
                       QJsonObject object = value.toObject();
                       QString vendor;
                       QString model;
                       if(object.contains(VENDOR))
                       {
                           QJsonValue value = object.value(VENDOR);
                           if(value.isString())
                           {
                               vendor = value.toString();
                           }
                       }
                       if(object.contains(MODEL))
                       {
                           QJsonValue value = object.value(MODEL);
                           if(value.isString())
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
       if(obj.contains(ETHS))
       {
           QJsonValue value = obj.take(ETHS);
           if(value.isArray())
           {
               QJsonArray array = value.toArray();
               int nSize = array.size();
               for (int i = 0; i < nSize; ++i)
               {
                   QJsonValue value = array.at(i);
                   if(value.isObject())
                   {
                       QJsonObject object = value.toObject();
                       QString vendor;
                       QString model;
                       if(object.contains(VENDOR))
                       {
                           QJsonValue value = object.value(VENDOR);
                           if(value.isString())
                           {
                               vendor = value.toString();
                           }
                       }
                       if(object.contains(MODEL))
                       {
                           QJsonValue value = object.value(MODEL);
                           if(value.isString())
                           {
                               model = value.toString();
                           }
                       }

                       ///FIXME:后续将界面要显示graphics值，做成数组
                       QString ethsInfo = QString("%1 (%2)").arg(model).arg(vendor);
                       ethsList << ethsInfo;
                   }
               }
           }
       }
    }
}

/**
 * @brief HardwareInformationWidget::showListInfo: 在硬件信息界面中显示所有的硬盘、网卡、显卡信息列表
 */
void HardwareInformationWidget::showListInfo()
{
    int i;
    int diskNum = diskList.size();
    int graphicsNum = graphicsList.size();
    int ethsNum = ethsList.size();


    for(i=0; i<diskNum; i++)
    {
        qInfo() << diskList.at(i);
    }
    for(i=0; i<graphicsNum; i++)
    {
        qInfo() << graphicsList.at(i);
    }
    for(i=0; i<ethsNum; i++)
    {
        qInfo() << ethsList.at(i);
    }
}
