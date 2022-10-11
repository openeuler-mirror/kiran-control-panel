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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#ifndef KIRAN_ACCOUNT_MANAGER_MASK_WIDGET_H
#define KIRAN_ACCOUNT_MANAGER_MASK_WIDGET_H

#include <QWidget>

class MaskWidget : public QWidget{
    Q_OBJECT
public:
    explicit MaskWidget(QWidget *parent);
    ~MaskWidget() = default;

private Q_SLOTS:

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QWidget *const m_maskedWidget;
};


#endif //KIRAN_ACCOUNT_MANAGER_MASK_WIDGET_H
