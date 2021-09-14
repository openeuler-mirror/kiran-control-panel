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

#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QWidget>

namespace Ui
{
class LayoutItem;
}

class LayoutItem : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isPressed READ isPressed WRITE setisPressed)
public:
    explicit LayoutItem(QWidget *parent = 0);
    ~LayoutItem();
    void setItemText(QString text);
    QString getItemText();
    void selectedItemChanged(QString coutryName);
    void setisPressed(bool pressed);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void clicked();

private:
    bool isPressed() const;

private:
    Ui::LayoutItem *ui;
    bool m_isPressed = false;
};

#endif  // LAYOUTITEM_H
