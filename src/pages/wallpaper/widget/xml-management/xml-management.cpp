#include "xml-management.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDir>
#include <iostream>
#define LOCAL_WALLPAPER_FILE    ".config/kylinsec/wallpaper.xml"
#define SYSTEM_WALLPAPER_FILE   "/usr/share/mate-background-properties/kiran_background.xml"


XmlManagement::XmlManagement(QWidget *parent) :
    QWidget(parent)
{
    loadXmlFiles();
}

XmlManagement::~XmlManagement()
{

}

void XmlManagement::loadXmlFiles()
{
    QString localFile = QString("%1/%2").arg(QDir::homePath()).arg(LOCAL_WALLPAPER_FILE);
    QFile file(localFile);
    if(!file.exists())
    {
        std::cout << "local xml file no exist" << std::endl;
        xmlReader(SYSTEM_WALLPAPER_FILE);
        xmlWriter();
    }
    xmlReader(localFile);
}

bool XmlManagement::xmlReader(QString filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cout << "open " << filePath.toStdString() << " failed!" << std::endl;
        return false;
    }
    m_wallpaperXmlHead.clear();
    m_mapList.clear();

    QXmlStreamReader reader;
    reader.setDevice(&file);
    int tokenType;
    while (!reader.atEnd()) {
        tokenType = reader.readNext();
        switch (tokenType) {
        case QXmlStreamReader::StartDocument:
        {// head
            std::cout << "********** 开始文档（XML 声明） ********** " << std::endl;
            // XML 声明
            QString strVersion = reader.documentVersion().toString();
            QString strEncoding = reader.documentEncoding().toString();
            std::cout << "版本：" << strVersion.toStdString() << " 编码：" << strEncoding.toStdString() << std::endl;
            m_wallpaperXmlHead.insert("version",strVersion);
            m_wallpaperXmlHead.insert("encoding",strEncoding);
            break;
        }
        case QXmlStreamReader::DTD:
        {   // DTD
            std::cout << "********** DTD ********** " << std::endl;
            // DTD 声明
            QString strDTDName = reader.dtdName().toString();
            QString strDTDSystemId = reader.dtdSystemId().toString();  // DTD 系统标识符
            std::cout << "DTD 名称 : " <<strDTDName.toStdString() << std::endl;
            std::cout << "DTD 系统标识符 : " <<strDTDSystemId.toStdString() << std::endl;
            m_wallpaperXmlHead.insert("DTD name",strDTDName);
            m_wallpaperXmlHead.insert("DTD systemId",strDTDSystemId);
            break;
        }
        case QXmlStreamReader::StartElement:
        {
            if(reader.name() == "wallpapers")
            {
                std::cout <<"********** 开始元素<wallpapers> ********** "<< std::endl;
                // parse wallpapers
                parseWallpapers(reader);
            }
            break;
        }
        case QXmlStreamReader::EndDocument:
        {  // 结束文档
            std::cout << "********** 结束文档 ********** "<< std::endl;
            break;
        }
        default:
            reader.skipCurrentElement();
        }
    }
    if(reader.hasError())
    {
        file.close();
        std::cout << "error: " << reader.error() << std::endl;
        return false;
    }
    file.close();
    return true;
}

bool XmlManagement::xmlWriter()
{
    QString localFile = QString("%1/%2").arg(QDir::homePath()).arg(LOCAL_WALLPAPER_FILE);
    QFile file(localFile);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        std::cout << "open " <<localFile.toStdString() <<" failed!" << std::endl;
        return false;
    }

    QXmlStreamWriter writer;
    writer.setDevice(&file);
    writer.setAutoFormatting(true); // 自动格式化

    //write head
    QString version = m_wallpaperXmlHead.find("version").value();
    writer.writeStartDocument(version,false);

    writer.writeDTD(QString::fromLocal8Bit("<!DOCTYPE %1 SYSTEM \"%2\">").
                    arg(m_wallpaperXmlHead.find("DTD name").value()).
                    arg(m_wallpaperXmlHead.find("DTD systemId").value()));

    //write body
    writer.writeStartElement("wallpapers");

    std::cout << "m_mapList size " << m_mapList.size() << std::endl;
    for(QMap<QString,QString> map:m_mapList)
    {
        QMap<QString, QString>::const_iterator i = map.begin();
        writer.writeStartElement("wallpaper");
        if(map.contains("deleted"))
            writer.writeAttribute("deleted", map.find("deleted").value());
        else
            writer.writeAttribute("deleted","false");
        if(map.contains("name"))
        {
            writer.writeTextElement("name",map.find("name").value());
        }
        while (i != map.end())
        {
          if(i.key() == "deleted" || i.key() == "xml:lang" || i.key() == "langName" || i.key() == "name")
          {
              i++;
              continue;
          }
          writer.writeTextElement(i.key(),QString(i.value()));
          i++;
        }

        if(map.contains("artist"))
        {
            writer.writeTextElement("artist",map.find("artist").value());
        }
        else
            writer.writeTextElement("artist","(none)");
        if(map.contains("pcolor"))
        {
            writer.writeTextElement("pcolor",map.find("pcolor").value());
        }
        else
            writer.writeTextElement("pcolor","#000000");
        if(map.contains("scolor"))
        {
            writer.writeTextElement("scolor",map.find("scolor").value());
        }
        else
            writer.writeTextElement("scolor","#000000");
        if(map.contains("shade_type"))
        {
            writer.writeTextElement("shade_type",map.find("shade_type").value());
        }
        else
            writer.writeTextElement("shade_type","vertical-gradient");
        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
    return true;
}

//add(one),delete
void XmlManagement::xmlUpdate(QList<QMap<QString,QString>> updateList)
{
    QString localFile = QString("%1/%2").arg(QDir::homePath()).arg(LOCAL_WALLPAPER_FILE);
    QFile file(localFile);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        std::cout << "open " <<localFile.toStdString() <<" failed!" << std::endl;
        return;
    }
    if(updateList.size() < 1)
    {
        std::cout << "there has no element to update!" << std::endl;
        return;
    }

    QXmlStreamWriter writer;
    writer.setDevice(&file);
    writer.setAutoFormatting(true); // 自动格式化

    //write head
    QString version = m_wallpaperXmlHead.find("version").value();
    writer.writeStartDocument(version,false);

    writer.writeDTD(QString::fromLocal8Bit("<!DOCTYPE %1 SYSTEM \"%2\">").
                    arg(m_wallpaperXmlHead.find("DTD name").value()).
                    arg(m_wallpaperXmlHead.find("DTD systemId").value()));

    //write body
    writer.writeStartElement("wallpapers");

    std::cout << "updateList size " << updateList.size() << std::endl;
    for(QMap<QString,QString> map:updateList)
    {
        QMap<QString, QString>::const_iterator i = map.begin();
        writer.writeStartElement("wallpaper");
        if(map.contains("deleted"))
            writer.writeAttribute("deleted", map.find("deleted").value());
        if(map.contains("name"))
        {
            writer.writeTextElement("name",map.find("name").value());
        }
        while (i != map.end())
        {
          if(i.key() == "deleted" || i.key() == "xml:lang" || i.key() == "langName" || i.key() == "name" )
          {
              i++;
              continue;
          }
          writer.writeTextElement(i.key(),QString(i.value()));
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
    while (!reader.atEnd()) {
        tokenType = reader.readNext();
        switch (tokenType) {
        case QXmlStreamReader::StartElement:
        {
            if(reader.name() == "wallpaper")
            {
                std::cout << "********** 开始元素<wallpaper> ********** " << std::endl;
                QXmlStreamAttributes attributes = reader.attributes();
                if (attributes.hasAttribute("deleted")) {
                    QString strDeleted = attributes.value("deleted").toString();
                    std::cout << "属性：deleted:" << strDeleted.toStdString() << std::endl;
                    bodyMap.insert("deleted",strDeleted);
                }
            }
            else if(reader.name() == "name")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                QString name = reader.readElementText();
                if(!attributes.hasAttribute("xml:lang"))
                {
                    std::cout << "wallpaper name = " << name.toStdString() << std::endl;
                    bodyMap.insert("name",name);
                }
                else
                {
                    QString xmlLang = attributes.value("xml:lang").toString();
                    std::cout << "语言：xml:lang:" << xmlLang.toStdString() << " 名字：" << name.toStdString()<< std::endl;
                    bodyMap.insert("xml:lang",xmlLang);
                    bodyMap.insert("langName",name);
                }
            }
            else if(reader.name() == "filename")
            {
                QString fileName = reader.readElementText();
                std::cout << "wallpaper fileName = " << fileName.toStdString() << std::endl;
                bodyMap.insert("filename",fileName);
            }
            else if(reader.name() == "options")
            {
                QString options = reader.readElementText();
                std::cout << "wallpaper options = " << options.toStdString() << std::endl;
                bodyMap.insert("options",options);
            }
            else
            {
                std::cout << "wallpaper = " << reader.name().toString().toStdString() << std::endl;
                bodyMap.insert(reader.name().toString(), reader.readElementText());
            }
            break;
        }
        case QXmlStreamReader::EndElement:
        {
            std::cout << "********** 结束元素<" <<reader.name().toString().toStdString() << "> ********** " << std::endl;
            if(reader.name() == "wallpaper")
            {
                m_mapList.append(bodyMap);
            }
            break;
        }
        default:
            break;
        }
    }
    if(reader.hasError())
    {
        std::cout << "wallpaper error" << reader.error() << std::endl;
    }

    //getVisibleWallpaper();
}

//void XmlManagement::getVisibleWallpaper()
//{
//    for(QMap<QString,QString> map:m_mapList)
//    {
//        QMap<QString, QString>::const_iterator i = map.constBegin();
//          while (i != map.constEnd()) {
//              QString deleted = map.find("deleted").value();
//              if(deleted == "false")
//              {
//                  m_visibleWallpaper.append(map.find("filename").value());
//                  break;
//              }
//          }
//    }
////    for(QString file:m_visibleWallpaper)
////    {
////        std::cout << "visible wallpaper :" << file.toStdString() << std::endl;
//    //    }
//}

QList<QMap<QString, QString> > XmlManagement::getXmlBodyInfo()
{
    return m_mapList;
}
