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

#include "thread-object.h"
#include "xml-management.h"
ThreadObject::ThreadObject(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QList<QMap<QString, QString>>>("QList<QMap<QString, QString>>");
    m_xmlManagement = new XmlManagement;
}

ThreadObject::~ThreadObject()
{
    delete m_xmlManagement;
}

void ThreadObject::updateWallpaperXml(QList<QMap<QString, QString>> mapList)
{
    m_xmlManagement->xmlUpdate(mapList);
}

void ThreadObject::loadWallpaperInfo()
{
    m_listMap = m_xmlManagement->getXmlBodyInfo();

    emit getWallpaperInfo(m_listMap);
}
