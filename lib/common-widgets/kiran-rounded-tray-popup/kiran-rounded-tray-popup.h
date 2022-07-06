//
// Created by liuxinhao on 2022/6/24.
//

#ifndef KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_TRAY_POPUP_H
#define KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_TRAY_POPUP_H

#include <QWidget>

class QLayout;
class KiranRoundedTrayPopup:public QWidget
{
    Q_OBJECT
public:
    explicit KiranRoundedTrayPopup(QWidget* parent = nullptr);
    ~KiranRoundedTrayPopup();

    void setContentWidget(QWidget* widget);

private:
    void init();


private:
    QLayout* m_contentLayout;
};

#endif  // KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_TRAY_POPUP_H
