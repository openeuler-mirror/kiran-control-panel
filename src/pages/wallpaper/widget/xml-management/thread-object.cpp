#include "thread-object.h"
#include "xml-management.h"
#include <iostream>
ThreadObject::ThreadObject(QObject *parent):
    QObject(parent)
{
     qRegisterMetaType<QList<QMap<QString, QString>>>("QList<QMap<QString, QString>>");
     m_xmlManagement = new XmlManagement;
}

ThreadObject::~ThreadObject()
{
    delete m_xmlManagement;
}

void ThreadObject::updateWallpaperXml(QList<QMap<QString, QString> > mapList)
{
    m_xmlManagement->xmlUpdate(mapList);
}

void ThreadObject::loadWallpaperInfo()
{
    std::cout << "loadWallpaperInfo" << std::endl;

    m_listMap = m_xmlManagement->getXmlBodyInfo();

    emit getWallpaperInfo(m_listMap);
}
