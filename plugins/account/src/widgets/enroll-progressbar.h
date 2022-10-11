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

#ifndef KIRAN_ACCOUNT_MANAGER_ENROLL_PROGRESSBAR_H
#define KIRAN_ACCOUNT_MANAGER_ENROLL_PROGRESSBAR_H

#include <QWidget>

class EnrollProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit EnrollProgressBar(QWidget *parent = nullptr);
    ~EnrollProgressBar() override;

    ///设置进度条数据 0~100
    void setProgressValue(unsigned int value);
    ///设置进度条中心图片
    void updateCenterImage(const QPixmap &centerPixmap);
    ///获取中心原型图片大小
    qreal getCenterImageRadius();

private:
    void initUI();
    void paintProgressBar(QPainter &painter);
    void paintCenterPixmap(QPainter &painter, const QPixmap &pixmap);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    /* 进度条线段长度 */
    unsigned int m_paintProgressLength = 18;
    /* 进度条线段数量 */
    unsigned int m_progressLinesCount = 80;
    /* 进度值 */
    unsigned int m_progressValue = 0;
    /* 进度条线段距边框距离 */
    unsigned int m_borerMargin = 4;
    QPixmap m_centerPixmap;
};

#endif  //KIRAN_ACCOUNT_MANAGER_ENROLL_PROGRESSBAR_H
