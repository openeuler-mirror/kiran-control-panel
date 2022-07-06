/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-keybinding is licensed under Mulan PSL v2.
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

#ifndef THREADOBJECT_H
#define THREADOBJECT_H
#include <QObject>
#include "keybinding_backEnd_proxy.h"
#include "keybinding_def.h"

class ThreadObject : public QObject
{
    Q_OBJECT
public:
    explicit ThreadObject(QObject *parent = nullptr);
    ~ThreadObject();

private:
    int getJsonValueFromString(QString jsonString);
    void convetToValue(QJsonObject obj, QString key);

public slots:
    void loadShortcutInfo();

signals:
    void getShortcutInfo(QList<ShortcutInfo *> shortcutsList);

private:
    QList<ShortcutInfo *> m_shortcutsList;
    KeybindingBackEndProxy *m_keybindingInterface;
};

#endif  // THREADOBJECT_H
