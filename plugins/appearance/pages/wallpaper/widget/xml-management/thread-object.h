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

#ifndef THREADOBJECT_H
#define THREADOBJECT_H

#include <QMap>
#include <QObject>
class XmlManagement;
class ThreadObject : public QObject
{
    Q_OBJECT
public:
    explicit ThreadObject(QObject *parent = nullptr);
    ~ThreadObject();

    void updateWallpaperXml(QList<QMap<QString, QString>> mapList);

public slots:
    void loadWallpaperInfo();

signals:
    void getWallpaperInfo(QList<QMap<QString, QString>> mapList);

private:
    XmlManagement *m_xmlManagement;

    QList<QMap<QString, QString>> m_listMap;
};

#endif  // THREADOBJECT_H
