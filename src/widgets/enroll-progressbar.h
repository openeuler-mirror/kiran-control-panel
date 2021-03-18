//
// Created by lxh on 2021/3/12.
//

#ifndef KIRAN_ACCOUNT_MANAGER_ENROLL_PROGRESSBAR_H
#define KIRAN_ACCOUNT_MANAGER_ENROLL_PROGRESSBAR_H

#include <QWidget>

class EnrollProgressBar : public QWidget {
    Q_OBJECT
public:
    explicit EnrollProgressBar(QWidget *parent = nullptr);
    ~EnrollProgressBar() override;

    ///设置进度条数据 0~100
    void setProgressValue(unsigned int value);
    ///设置进度条中心图片
    void updateCenterImage(const QPixmap& centerPixmap);
    ///获取中心原型图片大小
    qreal getCenterImageRadius();

private:
    void initUI();
    void paintProgressBar(QPainter& painter);
    void paintCenterPixmap(QPainter& painter,const QPixmap& pixmap);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    /* 进度条线段长度 */
    unsigned int m_paintProgressLength = 16;
    /* 进度条线段数量 */
    unsigned int m_progressLinesCount = 80;
    /* 进度值 */
    unsigned int m_progressValue = 0;
    /* 进度条线段距边框距离 */
    unsigned int m_borerMargin = 4;
    QPixmap m_centerPixmap;
};

#endif //KIRAN_ACCOUNT_MANAGER_ENROLL_PROGRESSBAR_H
