//
// Created by liuxinhao on 2022/6/24.
//

#ifndef KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_WINDOW_FRAME_H
#define KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_WINDOW_FRAME_H

#include <QFrame>

class KiranRoundedWindowFrame :public  QFrame
{
    Q_OBJECT
public:
    explicit KiranRoundedWindowFrame(QWidget* parent = nullptr);
    ~KiranRoundedWindowFrame();

    static const int radius;
protected:
    void paintEvent(QPaintEvent* event) override;
};

#endif  // KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_WINDOW_FRAME_H
