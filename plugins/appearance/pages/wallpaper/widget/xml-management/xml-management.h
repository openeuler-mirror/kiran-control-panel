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

#ifndef XMLMANAGEMENT_H
#define XMLMANAGEMENT_H

#include <QMap>
#include <QWidget>

class QXmlStreamReader;
class XmlManagement : public QWidget
{
    Q_OBJECT

public:
    explicit XmlManagement(QWidget *parent = 0);
    ~XmlManagement();

    bool xmlReader(QString filePath);
    bool xmlWriter();
    void xmlUpdate(QList<QMap<QString, QString>> updateList);
    QList<QMap<QString, QString>> getXmlBodyInfo();

private:
    void loadXmlFiles();
    void parseWallpapers(QXmlStreamReader &reader);

private:
    QMap<QString, QString> m_wallpaperXmlHead;
    QList<QMap<QString, QString>> m_mapList;
};

#endif  // XMLMANAGEMENT_H
