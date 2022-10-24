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

#include "xml-management.h"
#include <kiran-log/qt5-log-i.h>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include "pages/wallpaper/wallpaper-def.h"

XmlManagement::XmlManagement(QWidget *parent) : QWidget(parent)
{
    loadXmlFiles();
}

XmlManagement::~XmlManagement()
{
}

/**
 * @brief XmlManagement::loadXmlFiles 加载xml文件
 */
void XmlManagement::loadXmlFiles()
{
    QString localFile = QString("%1/%2").arg(QDir::homePath()).arg(LOCAL_WALLPAPER_FILE);
    QFile file(localFile);
    if (!file.exists())
    {
        KLOG_DEBUG() << "local xml file no exist";
        xmlReader(SYSTEM_WALLPAPER_FILE);
        xmlWriter();
    }
    xmlReader(localFile);
}

/**
 * @brief XmlManagement::xmlReader 读取xml文件内容
 * @param filePath xml文件路径
 * @return true 读取成功
 *         false 读取失败
 */
bool XmlManagement::xmlReader(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        KLOG_DEBUG() << "open " << filePath << " failed!";
        return false;
    }
    m_wallpaperXmlHead.clear();
    m_mapList.clear();

    QXmlStreamReader reader;
    reader.setDevice(&file);
    int tokenType;
    while (!reader.atEnd())
    {
        tokenType = reader.readNext();
        switch (tokenType)
        {
        case QXmlStreamReader::StartDocument:
        {  // head
            // XML 声明
            QString strVersion = reader.documentVersion().toString();
            QString strEncoding = reader.documentEncoding().toString();
            m_wallpaperXmlHead.insert("version", strVersion);
            m_wallpaperXmlHead.insert("encoding", strEncoding);
            break;
        }
        case QXmlStreamReader::DTD:
        {  // DTD
            // DTD 声明
            QString strDTDName = reader.dtdName().toString();
            QString strDTDSystemId = reader.dtdSystemId().toString();  // DTD 系统标识符

            m_wallpaperXmlHead.insert("DTD name", strDTDName);
            m_wallpaperXmlHead.insert("DTD systemId", strDTDSystemId);
            break;
        }
        case QXmlStreamReader::StartElement:
        {
            if (reader.name() == "wallpapers")
            {
                // parse wallpapers
                parseWallpapers(reader);
            }
            break;
        }
        case QXmlStreamReader::EndDocument:
        {  // 结束文档
            break;
        }
        default:
            reader.skipCurrentElement();
        }
    }
    if (reader.hasError())
    {
        file.close();
        KLOG_ERROR() << "error: " << reader.error();
        return false;
    }
    file.close();
    return true;
}

/**
 * @brief XmlManagement::xmlWriter 将壁纸信息写入xml文件
 * @return true 写入成功
 *         false 写入失败
 */
bool XmlManagement::xmlWriter()
{
    QString localFile = QString("%1/%2").arg(QDir::homePath()).arg(LOCAL_WALLPAPER_FILE);
    QFile file(localFile);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        KLOG_DEBUG() << "open " << localFile << " failed!";
        return false;
    }

    QXmlStreamWriter writer;
    writer.setDevice(&file);
    writer.setAutoFormatting(true);  // 自动格式化

    //write head
    QString version = m_wallpaperXmlHead.find("version").value();
    writer.writeStartDocument(version, false);

    writer.writeDTD(QString::fromLocal8Bit("<!DOCTYPE %1 SYSTEM \"%2\">").arg(m_wallpaperXmlHead.find("DTD name").value()).arg(m_wallpaperXmlHead.find("DTD systemId").value()));

    //write body
    writer.writeStartElement("wallpapers");

    KLOG_INFO() << "m_mapList size " << m_mapList.size();
    for (QMap<QString, QString> map : m_mapList)
    {
        QMap<QString, QString>::const_iterator i = map.begin();
        writer.writeStartElement("wallpaper");
        if (map.contains(DELETED))
            writer.writeAttribute(DELETED, map.value(DELETED));
        else
            writer.writeAttribute(DELETED, "false");
        if (map.contains(NAME))
        {
            writer.writeTextElement(NAME, map.value(NAME));
        }
        while (i != map.end())
        {
            if (i.key() == DELETED || i.key() == "xml:lang" || i.key() == "langName" || i.key() == NAME)
            {
                i++;
                continue;
            }
            writer.writeTextElement(i.key(), QString(i.value()));
            i++;
        }

        if (map.contains(ARTIST))
        {
            writer.writeTextElement(ARTIST, map.value(ARTIST));
        }
        else
            writer.writeTextElement(ARTIST, "(none)");
        if (map.contains(PCOLOR))
        {
            writer.writeTextElement(PCOLOR, map.value(PCOLOR));
        }
        else
            writer.writeTextElement(PCOLOR, "#000000");
        if (map.contains(SCOLOR))
        {
            writer.writeTextElement(SCOLOR, map.value(SCOLOR));
        }
        else
            writer.writeTextElement(SCOLOR, "#000000");
        if (map.contains(SHADE_TYPE))
        {
            writer.writeTextElement(SHADE_TYPE, map.value(SHADE_TYPE));
        }
        else
            writer.writeTextElement(SHADE_TYPE, "vertical-gradient");
        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
    return true;
}

//add(one),delete
/**
 * @brief XmlManagement::xmlUpdate 更新xml文件内容
 * @param updateList 更新的壁纸信息列表
 */
void XmlManagement::xmlUpdate(QList<QMap<QString, QString>> updateList)
{
    QString localFile = QString("%1/%2").arg(QDir::homePath()).arg(LOCAL_WALLPAPER_FILE);
    QFile file(localFile);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        KLOG_DEBUG() << "open " << localFile << " failed!";
        return;
    }
    if (updateList.size() < 1)
    {
        KLOG_DEBUG() << "there has no element to update!";
        return;
    }

    QXmlStreamWriter writer;
    writer.setDevice(&file);
    writer.setAutoFormatting(true);  // 自动格式化

    //write head
    QString version = m_wallpaperXmlHead.find("version").value();
    writer.writeStartDocument(version, false);

    writer.writeDTD(QString::fromLocal8Bit("<!DOCTYPE %1 SYSTEM \"%2\">").arg(m_wallpaperXmlHead.find("DTD name").value()).arg(m_wallpaperXmlHead.find("DTD systemId").value()));

    //write body
    writer.writeStartElement("wallpapers");

    KLOG_INFO() << "updateList size " << updateList.size();
    for (QMap<QString, QString> map : updateList)
    {
        QMap<QString, QString>::const_iterator i = map.begin();
        writer.writeStartElement("wallpaper");
        if (map.contains(DELETED))
            writer.writeAttribute(DELETED, map.value(DELETED));
        if (map.contains(NAME))
        {
            writer.writeTextElement(NAME, map.value(NAME));
        }
        while (i != map.end())
        {
            if (i.key() == DELETED || i.key() == "xml:lang" || i.key() == "langName" || i.key() == NAME)
            {
                i++;
                continue;
            }
            writer.writeTextElement(i.key(), QString(i.value()));
            i++;
        }
        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
    m_mapList = updateList;
}

void XmlManagement::parseWallpapers(QXmlStreamReader &reader)
{
    int tokenType;
    QMap<QString, QString> bodyMap;
    while (!reader.atEnd())
    {
        tokenType = reader.readNext();
        switch (tokenType)
        {
        case QXmlStreamReader::StartElement:
        {
            if (reader.name() == "wallpaper")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                if (attributes.hasAttribute(DELETED))
                {
                    QString strDeleted = attributes.value(DELETED).toString();
                    bodyMap.insert(DELETED, strDeleted);
                }
            }
            else if (reader.name() == NAME)
            {
                QXmlStreamAttributes attributes = reader.attributes();
                QString name = reader.readElementText();
                if (!attributes.hasAttribute("xml:lang"))
                {
                    bodyMap.insert(NAME, name);
                }
                else
                {
                    QString xmlLang = attributes.value("xml:lang").toString();
                    bodyMap.insert("xml:lang", xmlLang);
                    bodyMap.insert("langName", name);
                }
            }
            else if (reader.name() == FILENAME)
            {
                QString fileName = reader.readElementText();
                bodyMap.insert(FILENAME, fileName);
            }
            else if (reader.name() == OPTIONS)
            {
                QString options = reader.readElementText();
                bodyMap.insert(OPTIONS, options);
            }
            else
            {
                bodyMap.insert(reader.name().toString(), reader.readElementText());
            }
            break;
        }
        case QXmlStreamReader::EndElement:
        {
            if (reader.name() == "wallpaper")
            {
                m_mapList.append(bodyMap);
            }
            break;
        }
        default:
            break;
        }
    }
    if (reader.hasError())
    {
        KLOG_ERROR() << "wallpaper error" << reader.error();
    }
}

QList<QMap<QString, QString>> XmlManagement::getXmlBodyInfo()
{
    return m_mapList;
}
