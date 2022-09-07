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
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void updateContentMask(QWidget* widget);
    void init();

private:
    QLayout* m_contentLayout;
    QWidget* m_contentWidget=nullptr;
};

#endif  // KIRAN_CONTROL_PANEL_KIRAN_ROUNDED_TRAY_POPUP_H
