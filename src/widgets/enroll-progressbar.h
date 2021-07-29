 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
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
