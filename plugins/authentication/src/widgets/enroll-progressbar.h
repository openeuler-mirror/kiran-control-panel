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
class QPropertyAnimation;

class EnrollProgressBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(uint progressValue READ getProgressValue WRITE setProgressValue)
public:
    explicit EnrollProgressBar(QWidget *parent = nullptr);
    virtual ~EnrollProgressBar() override;

    // 生存周期将由该类接管，外部不应释放
    void setCenterWidget(QWidget* widget);
    virtual void setProgress(uint progress);
    void setAnimationEnable(bool enable);

private:
    uint getProgressValue() { return m_progressValue; };
    void setProgressValue(unsigned int value);
    void adjustCenterWidget(QSize size);
    void paintProgressBar(QPainter &painter);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    static const unsigned int m_paintProgressLength;
    static const unsigned int m_progressLinesCount;
    static const unsigned int m_borerMargin;

    unsigned int m_progressValue = 0;
    bool m_animationEnable = true;
    QWidget *m_centerWidget = nullptr;
    QPropertyAnimation *m_animation = nullptr;
};

#endif  //KIRAN_ACCOUNT_MANAGER_ENROLL_PROGRESSBAR_H
