/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http: //www.gnu.org/licenses/>.
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
