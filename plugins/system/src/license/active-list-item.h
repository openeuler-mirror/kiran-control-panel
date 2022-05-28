/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
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
#ifndef ACTIVELISTITEM_H
#define ACTIVELISTITEM_H

#include <QWidget>

namespace Ui {
class ActListItem;
}

class ActListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ActListItem(QWidget *parent = 0);
    ~ActListItem();

public:
    void iniUI();
    void setItemText(const QString &text);


private:
    Ui::ActListItem *ui;
};

#endif // ACTIVELISTITEM_H
