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
#ifndef LOADANIMATIONWIDGET_H
#define LOADANIMATIONWIDGET_H

#include <QWidget>
#include <QTimer>

class LoadAnimationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoadAnimationWidget(QWidget *parent = nullptr);
    ~LoadAnimationWidget() override = default;


protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual QSize sizeHint() const override;


public slots:
    /* restart Animation */
    void resetAnimation();

    /* stop Animation */
    void stopAnimation();

    /* resume Animation */
    void resumeAnimation();

private slots:
    void renderNextFrame();
private:
    QTimer timer;
    int index;
};

#endif // LOADANIMATIONWIDGET_H
