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
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <Qt>
#include <math.h>
#include<sstream>
#include <iomanip>
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
}

HardwareInformationWidget::~HardwareInformationWidget()
{
    delete ui;
}

void HardwareInformationWidget::initUI(void)
{
    ui->widget_logo->hide();
}

/**
 * @brief HardwareInformationWidget::readHardwareInfo 调用Dbus接口，读取系统信息，并解析，显示在界面上
 * @param infoType 读取的信息类型，0：系统信息 1：硬件信息
 */
void HardwareInformationWidget::readHardwareInfo(int infoType)
{
    QString systemInfo;
    QLabel *labelDisk = new QLabel();
    labelDisk->setText(tr("Unknow"));
    labelDisk->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";font-size:12px;}");
    QLabel *labelGraphics = new QLabel();
    labelGraphics->setText(tr("Unknow"));
    labelGraphics->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";font-size:12px;}");
    QLabel *labelEths = new QLabel();
    labelEths->setText(tr("Unknow"));
    labelEths->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";font-size:12px;}");

    if(!InfoDbus::SystemInfo::getSystemInfo(infoType , systemInfo))
    {
        qDebug() << "get hardware information failed"<< endl;
        ui->label_CPU_info->setText(tr("Unknow"));
        ui->label_memory_info->setText(tr("Unknow"));

        ui->gridLayout_graphics_card->addWidget(labelGraphics,1,0,Qt::AlignRight);
        ui->gridLayout_hard_disk->addWidget(labelDisk,1,0,Qt::AlignRight);
        ui->gridLayout_network_card->addWidget(labelEths,1,0,Qt::AlignRight);
        return;
    }
    else
    {
        qInfo() << systemInfo << endl;
        ui->gridLayout_graphics_card->removeWidget(labelGraphics);
        ui->gridLayout_hard_disk->removeWidget(labelDisk);
        ui->gridLayout_network_card->removeWidget(labelEths);

        getJsonValueFromString(systemInfo);
        showListInfo();
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
                       double memory_size = value.toVariant().toDouble() /1024/1024/1024;
                       stringstream ss;
                       ss << setiosflags(ios::fixed) << setprecision(2) << memory_size;//保留两位小数
                       string str = ss.str();

                       QString size = QString("%1G").arg(QString::fromStdString(str));
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
               setCpuLogo(model);

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
                       double size ;
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
                               double disk_size = value.toVariant().toDouble();
                               size = disk_size/1024/1024/1024;
                           }
                       }
                       stringstream ss;
                       ss << setiosflags(ios::fixed) << setprecision(2) << size;//保留两位小数
                       string str = ss.str();
                       ///FIXME:后续将界面要显示disk值，做成数组
                       QString diskInfo = QString("%1 (%2G)").arg(disk_model).arg(QString::fromStdString(str));
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
    //根据数量设置各行的行高及左侧标签的布局
    int diskNum = diskList.size();
    if(diskNum > 0)
    {
        ui->label_hard_disk->setFixedHeight(40*diskNum);
    }

    int graphicsNum = graphicsList.size();
    if(graphicsNum > 0 )
    {
        ui->label_graphics_card->setFixedHeight(40*graphicsNum);
    }

    int ethsNum = ethsList.size();
    if(ethsNum > 0)
    {
        ui->label_network_card->setFixedHeight(40*ethsNum);
    }

    if(diskNum >1 )
    {
        qInfo() << "set disk label style" << endl;
        ui->label_hard_disk->setAlignment(Qt::AlignTop);
        ui->label_hard_disk->setStyleSheet("QLabel{padding-top:10px}");

    }
    if(graphicsNum > 1)
    {
        ui->label_graphics_card->setAlignment(Qt::AlignTop);
        ui->label_graphics_card->setStyleSheet("QLabel{padding-top:10px}");
    }
    if(ethsNum > 1)
    {
        ui->label_network_card->setAlignment(Qt::AlignTop);
        ui->label_network_card->setStyleSheet("QLabel{padding-top:10px}");
    }

    //依次将各个信息插入至布局
    for(i=0; i<diskNum; i++)
    {
        qInfo() << diskList.at(i);
        QLabel* labelDisk = new QLabel(diskList.at(i));
        //设置QLabel的样式
        labelDisk->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";font-size:12px;}");
        QFont font = labelDisk->font();
        int fontSize = font.pixelSize();
        qInfo() << "fontSize= " << fontSize;
        ui->gridLayout_hard_disk->addWidget(labelDisk,i,0,Qt::AlignRight);
    }

    for(i=0; i<graphicsNum; i++)
    {
        qInfo() << graphicsList.at(i);
        QLabel* labelGraphics = new QLabel(graphicsList.at(i));
        //设置QLabel的样式
        labelGraphics->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";font-size:12px;}");
        ui->gridLayout_graphics_card->addWidget(labelGraphics,i,0,Qt::AlignRight);
    }
    for(i=0; i<ethsNum; i++)
    {
        qInfo() << ethsList.at(i);
        QLabel* labelEths = new QLabel(ethsList.at(i));
        //设置QLabel的样式
        labelEths->setStyleSheet("QLabel{color:#7e7e7e;font-family: \"Noto Sans CJK SC regular\";font-size:12px;}");
        ui->gridLayout_network_card->addWidget(labelEths,i,0,Qt::AlignRight);
    }
}

void HardwareInformationWidget::setCpuLogo(QString cpuModel)
{
    qInfo() << "set logo " << endl;
    //根据cpu 型号名添加对应logo
    QImage imgLogo ;
    if(cpuModel.contains("Intel",Qt::CaseInsensitive))
    {
        if(!imgLogo.load(":/logos/logos/intel.jpg"))
        {
            qDebug() << "can't load logo image" << endl;
            return;
        }
    }
    else if(cpuModel.contains("ZHAOXIN", Qt::CaseInsensitive))
    {
        if(!imgLogo.load(":/logos/logos/ZHAOXIN.png"))
        {
            qDebug() << "can't load logo image" << endl;
            return;
        }
    }
    else if(cpuModel.contains("HUAWEI Kunpeng", Qt::CaseInsensitive))
    {
        if(!imgLogo.load(":/logos/logos/HUAWEI Kunpeng.png"))
        {
            qDebug() << "can't load logo image" << endl;
            return;
        }
    }
    else if(cpuModel.contains("LOONGSON", Qt::CaseInsensitive))
    {
        if(!imgLogo.load(":/logos/logos/Loongson.gif"))
        {
            qDebug() << "can't load logo image" << endl;
            return;
        }
    }
    else if(cpuModel.contains("PHYTIUM", Qt::CaseInsensitive))
    {
        if(!imgLogo.load(":/logos/logos/PHYTIUM.png"))
        {
            qDebug() << "can't load logo image" << endl;
            return;
        }
    }
    else if(cpuModel.contains("AMD", Qt::CaseInsensitive))
    {
        if(!imgLogo.load(":/logos/logos/amd.svg"))
        {
            qDebug() << "can't load logo image" << endl;
            return;
        }
    }
    else if(cpuModel.contains("sw", Qt::CaseInsensitive))
    {
        if(!imgLogo.load(":/logos/logos/sw_64.png"))
        {
            qDebug() << "can't load logo image" << endl;
            return;
        }
    }
    scaledPixmap(imgLogo);
}

void HardwareInformationWidget::scaledPixmap(QImage img)
{
    float labelLogoWidth = this->width();
    float labelLogoHeight = ui->widget_logo->height() - 80;
    float newWidth, newHeight;  //新的宽和高

    QPixmap pixmap;
    pixmap = QPixmap::fromImage(img);
    float scaledWidth = labelLogoWidth/pixmap.width();
    float scaledHeight = labelLogoHeight/pixmap.height();

    if(pixmap.width() < labelLogoWidth && pixmap.height() < labelLogoHeight)
    {
        ui->label_logo->setPixmap(pixmap);
        return;
    }
    else if(scaledHeight < scaledWidth)
    {
        newWidth = pixmap.width() * scaledHeight;
        newHeight = pixmap.height() * scaledHeight;
    }
    else if(scaledHeight >= scaledWidth)
    {
        newWidth = pixmap.width() * scaledWidth;
        newHeight = pixmap.height() * scaledWidth;
    }
    pixmap = pixmap.scaled(newWidth,newHeight,Qt::KeepAspectRatio);
    ui->label_logo->setPixmap(pixmap);
}
