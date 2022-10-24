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

#ifndef SCROLLCONTAINER_H
#define SCROLLCONTAINER_H

#include <QWidget>

class ScrollContainer : public QWidget
{
    Q_OBJECT
public:
    explicit ScrollContainer(QWidget *parent = nullptr);

signals:
    void resized(QSize size);

protected:
    virtual void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *event);
};

#endif  // SCROLLCONTAINER_H
