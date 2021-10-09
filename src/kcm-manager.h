/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-mouse is licensed under Mulan PSL v2.
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

#ifndef KCMMANAGER_H
#define KCMMANAGER_H

#include <QObject>
#include <QSharedPointer>

class KSMMouseProxy;
class KSMTouchPadProxy;
class KCMManager : public QObject
{
    Q_OBJECT
public:
    explicit KCMManager(QObject *parent = nullptr);
    ~KCMManager();

    bool isValidConnect();
    QSharedPointer<KSMMouseProxy> getMouseInterface();
    QSharedPointer<KSMTouchPadProxy> getTouchPadInterface();
    bool hasTouchPad();

private:
    QSharedPointer<KSMMouseProxy> m_mouseInterface;
    QSharedPointer<KSMTouchPadProxy> m_touchPadInterface;
};

#endif  // KCMMANAGER_H
