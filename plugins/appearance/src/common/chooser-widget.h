/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
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

#ifndef CHOOSERWIDGET_H
#define CHOOSERWIDGET_H

#include <QWidget>

namespace Ui
{
class ChooserWidget;
}

class ChooserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChooserWidget(QString title, int type = -1, QWidget *parent = 0);
    ~ChooserWidget();

    void setName(QString name);

private:
    void initUI(QString title);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

signals:
    void clicked();

private:
    Ui::ChooserWidget *ui;
    int m_wallpaperType = -1;
};

#endif  // CHOOSERWIDGET_H
