/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-keyboard is licensed under Mulan PSL v2.
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

#ifndef CHOOSEITEM_H
#define CHOOSEITEM_H

#include <QWidget>

namespace Ui
{
class ChooseItem;
}

class ChooseItem : public QWidget
{
    Q_OBJECT

public:
    explicit ChooseItem(QWidget *parent = 0);
    ~ChooseItem();
    void setSelected(bool isSelected);
    void setEditMode(bool isEditMode);
    QString getLayoutName();
    void setNames(QString countryName, QString layoutName);

signals:
    void sigDelete(QString layoutName);
    void clicked();

private:
    void initUI();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

public slots:
    void seletedLayoutChanged(QString selectLayout);

private:
    Ui::ChooseItem *ui;
    QString m_layoutName;
    bool m_hover = false;
};

#endif  // CHOOSEITEM_H
