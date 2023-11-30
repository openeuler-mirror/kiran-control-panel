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

#ifndef KCMMANAGER_H
#define KCMMANAGER_H

#include <QObject>
#include <QSharedPointer>

class MouseBackEndProxy;
class TouchPadBackEndProxy;
class KCMManager : public QObject
{
    Q_OBJECT
public:
    explicit KCMManager(QObject *parent = nullptr);
    ~KCMManager();

    bool isValidConnect();
    QSharedPointer<MouseBackEndProxy> getMouseInterface();
    QSharedPointer<TouchPadBackEndProxy> getTouchPadInterface();
    bool hasTouchPad();

private:
    QSharedPointer<MouseBackEndProxy> m_mouseInterface;
    QSharedPointer<TouchPadBackEndProxy> m_touchPadInterface;
};

#endif  // KCMMANAGER_H
