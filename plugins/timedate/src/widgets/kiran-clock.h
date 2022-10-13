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
#ifndef KIRANCLOCK_H
#define KIRANCLOCK_H

#include <QWidget>
#include <QPixmap>
#include <QSvgRenderer>

class QGraphicsDropShadowEffect;

/**
 * @brief 时钟控件
 */
class KiranClock : public QWidget
{
    Q_OBJECT
public:
    explicit KiranClock(QWidget *parent = nullptr);
    virtual ~KiranClock();

    /// 设置更新时间ms
    void setUpdateTimeout(int ms);
protected:
    void loadPixmap();
    void initShadow();
    void updateDrawRect();

    void paintDial(QPainter& painter);
    void paintHour(QPainter& painter);
    void paintMinute(QPainter& painter);
    void paintSecond(QPainter& painter);

    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
signals:
private:
    QGraphicsDropShadowEffect* m_effect;
    QRect m_drawRect;
    int m_updateTimerID;

    QSvgRenderer m_dialReader;
    QSvgRenderer m_hourReader;
    QSvgRenderer m_minuteReader;
    QSvgRenderer m_secondReader;
};

#endif // KIRANCLOCK_H
