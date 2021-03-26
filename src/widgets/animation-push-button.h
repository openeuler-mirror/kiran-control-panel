//
// Created by lxh on 2020/11/4.
//

#ifndef KIRAN_ACCOUNT_MANAGER_ANIMATION_PUSH_BUTTON_H
#define KIRAN_ACCOUNT_MANAGER_ANIMATION_PUSH_BUTTON_H

#include <QPushButton>
#include <QTimeLine>
#include <QSvgRenderer>

class AnimationButtonDelegate;

class AnimationPushButton : public QPushButton
{
Q_OBJECT
public:
    explicit AnimationPushButton (QWidget *parent);
    ~AnimationPushButton () = default;

    void setBusy (bool busy);
    bool busy ();

public Q_SLOTS:

private:
    void initTimeLine ();

protected:
    virtual void paintEvent (QPaintEvent *event) override;

private:
    bool m_isBusy = false;
    QTimeLine m_timeLine;
    QSvgRenderer m_svgRender;
    int m_rotationAngle = 0;
};

#endif //KIRAN_ACCOUNT_MANAGER_ANIMATION_PUSH_BUTTON_H