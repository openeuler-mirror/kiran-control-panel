#include "xml-management.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDir>
#include <kiran-log/qt5-log-i.h>
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

/**
 * @brief XmlManagement::loadXmlFiles 加载xml文件
 */
void XmlManagement::loadXmlFiles()
{
    QString localFile = QString("%1/%2").arg(QDir::homePath()).arg(LOCAL_WALLPAPER_FILE);
    QFile file(localFile);
    if(!file.exists())
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
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        KLOG_DEBUG() << "open " << filePath << " failed!";
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
            KLOG_INFO() << "********** 开始文档（XML 声明） ********** ";
            // XML 声明
            QString strVersion = reader.documentVersion().toString();
            QString strEncoding = reader.documentEncoding().toString();
            KLOG_INFO() << "版本：" << strVersion << " 编码：" << strEncoding;
            m_wallpaperXmlHead.insert("version",strVersion);
            m_wallpaperXmlHead.insert("encoding",strEncoding);
            break;
        }
        case QXmlStreamReader::DTD:
        {   // DTD
            KLOG_INFO() << "********** DTD ********** ";
            // DTD 声明
            QString strDTDName = reader.dtdName().toString();
            QString strDTDSystemId = reader.dtdSystemId().toString();  // DTD 系统标识符
            KLOG_INFO() << "DTD 名称 : " <<strDTDName;
            KLOG_INFO() << "DTD 系统标识符 : " <<strDTDSystemId;
            m_wallpaperXmlHead.insert("DTD name",strDTDName);
            m_wallpaperXmlHead.insert("DTD systemId",strDTDSystemId);
            break;
        }
        case QXmlStreamReader::StartElement:
        {
            if(reader.name() == "wallpapers")
            {
                KLOG_INFO() <<"********** 开始元素<wallpapers> ********** ";
                // parse wallpapers
                parseWallpapers(reader);
            }
            break;
        }
        case QXmlStreamReader::EndDocument:
        {  // 结束文档
            KLOG_INFO() << "********** 结束文档 ********** ";
            break;
        }
        default:
            reader.skipCurrentElement();
        }
    }
    if(reader.hasError())
    {
        file.close();
        KLOG_DEBUG() << "error: " << reader.error();
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
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        KLOG_DEBUG() << "open " <<localFile <<" failed!";
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

    KLOG_INFO() << "m_mapList size " << m_mapList.size();
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
/**
 * @brief XmlManagement::xmlUpdate 更新xml文件内容
 * @param updateList 更新的壁纸信息列表
 */
void XmlManagement::xmlUpdate(QList<QMap<QString,QString>> updateList)
{
    QString localFile = QString("%1/%2").arg(QDir::homePath()).arg(LOCAL_WALLPAPER_FILE);
    QFile file(localFile);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        KLOG_DEBUG() << "open " <<localFile <<" failed!";
        return;
    }
    if(updateList.size() < 1)
    {
        KLOG_DEBUG() << "there has no element to update!";
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

    KLOG_INFO() << "updateList size " << updateList.size();
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
                KLOG_INFO() << "********** 开始元素<wallpaper> ********** " ;
                QXmlStreamAttributes attributes = reader.attributes();
                if (attributes.hasAttribute("deleted")) {
                    QString strDeleted = attributes.value("deleted").toString();
                    KLOG_INFO() << "属性：deleted:" << strDeleted;
                    bodyMap.insert("deleted",strDeleted);
                }
            }
            else if(reader.name() == "name")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                QString name = reader.readElementText();
                if(!attributes.hasAttribute("xml:lang"))
                {
                    KLOG_INFO() << "wallpaper name = " << name;
                    bodyMap.insert("name",name);
                }
                else
                {
                    QString xmlLang = attributes.value("xml:lang").toString();
                    KLOG_INFO() << "语言：xml:lang:" << xmlLang<< " 名字：" << name;
                    bodyMap.insert("xml:lang",xmlLang);
                    bodyMap.insert("langName",name);
                }
            }
            else if(reader.name() == "filename")
            {
                QString fileName = reader.readElementText();
                KLOG_INFO() << "wallpaper fileName = " << fileName;
                bodyMap.insert("filename",fileName);
            }
            else if(reader.name() == "options")
            {
                QString options = reader.readElementText();
                KLOG_INFO() << "wallpaper options = " << options;
                bodyMap.insert("options",options);
            }
            else
            {
                KLOG_INFO() << "wallpaper = " << reader.name().toString();
                bodyMap.insert(reader.name().toString(), reader.readElementText());
            }
            break;
        }
        case QXmlStreamReader::EndElement:
        {
            KLOG_INFO() << "********** 结束元素<" <<reader.name().toString()<< "> ********** ";
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
        KLOG_INFO() << "wallpaper error" << reader.error();
    }
}

QList<QMap<QString, QString> > XmlManagement::getXmlBodyInfo()
{
    return m_mapList;
}
