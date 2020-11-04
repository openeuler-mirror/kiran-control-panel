//
// Created by lxh on 2020/11/2.
//

#ifndef KIRAN_ACCOUNT_MANAGER_ANIMATION_BUTTON_H
#define KIRAN_ACCOUNT_MANAGER_ANIMATION_BUTTON_H

#include <QAbstractButton>
#include <QColor>

class AnimationButtonDelegate;
class QParallelAnimationGroup;
class AnimationButton : public QAbstractButton {
    Q_OBJECT
    //圆圈线条宽度
    Q_PROPERTY(int circleWidth READ circleWidth WRITE setCircleWidth)
    //圆圈大小
    Q_PROPERTY(int circleSize READ circleSize WRITE setCircleSize)
    //圆圈线条颜色
    Q_PROPERTY(QColor circleColor READ circleColor WRITE setCircleColor)
public:
    explicit AnimationButton(QWidget* parent);
    ~AnimationButton();

    void setElideTextEnable(bool enable);
    bool elideTextEnable();

    //圆圈大小
    int circleSize();
    //圆圈线条宽度
    int circleWidth();
    //圆圈颜色
    QColor circleColor();

    //开启或停止动画
    void setBusy(bool busy);
    //当前动画状态
    bool isBusy();

public Q_SLOTS:
    void setCircleWidth(int lineWidth);
    void setCircleSize(int size);
    void setCircleColor(QColor color);

private:
    void initAnimation();

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    AnimationButtonDelegate *m_animationDelegate;
    int m_circleWidth;
    int m_circleSize;
    QColor m_circleColor;
    QParallelAnimationGroup *m_animationGroup;
    bool m_elideTextEnable = true;
};

#endif //KIRAN_ACCOUNT_MANAGER_ANIMATION_BUTTON_H
