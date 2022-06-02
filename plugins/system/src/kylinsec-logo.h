//
// Created by liuxinhao on 2022/5/30.
//

#ifndef KIRAN_CONTROL_PANEL_KYLINSEC_LOGO_H
#define KIRAN_CONTROL_PANEL_KYLINSEC_LOGO_H

#include <QWidget>
#include <QPaintEvent>

class KylinsecLogo : public QWidget
{
    Q_OBJECT
public:
    KylinsecLogo(QWidget* parent = nullptr);
    ~KylinsecLogo();

private:
    void paintEvent(QPaintEvent* event);

};

#endif  // KIRAN_CONTROL_PANEL_KYLINSEC_LOGO_H
