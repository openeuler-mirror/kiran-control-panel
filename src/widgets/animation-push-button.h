//
// Created by lxh on 2020/11/4.
//

#ifndef KIRAN_ACCOUNT_MANAGER_ANIMATION_PUSH_BUTTON_H
#define KIRAN_ACCOUNT_MANAGER_ANIMATION_PUSH_BUTTON_H

#include <QPushButton>
#include <QParallelAnimationGroup>

class AnimationButtonDelegate;

class AnimationPushButton : public QPushButton {
    Q_OBJECT
    //圆圈线条宽度
    Q_PROPERTY(int circleWidth READ circleWidth WRITE setCircleWidth)
    //圆圈大小
    Q_PROPERTY(int circleSize READ circleSize WRITE setCircleSize)
    //圆圈线条颜色
    Q_PROPERTY(QColor circleColor READ circleColor WRITE setCircleColor)
public:
    explicit AnimationPushButton(QWidget *parent);
    ~AnimationPushButton() = default;

    int circleSize();
    int circleWidth();
    QColor circleColor();

    void setBusy(bool busy);
    bool busy();

public Q_SLOTS:
    void setCircleSize(int size);
    void setCircleWidth(int width);
    void setCircleColor(QColor color);

private:
    void initAnimation();

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    int m_circleSize = 24;
    int m_circleWidth = 5;
    QColor m_circleColor = Qt::white;
    bool m_isBusy = false;
    QParallelAnimationGroup *m_animationGroup;
    AnimationButtonDelegate *m_animationDelegate;
};


#endif //KIRAN_ACCOUNT_MANAGER_ANIMATION_PUSH_BUTTON_H
