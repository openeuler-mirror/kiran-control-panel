#ifndef __PANEL_WINDOW_H__
#define __PANEL_WINDOW_H__
#include "kiranwidgets-qt5/kiran-titlebar-window.h"
#include "panel-widget.h"

/**
 * @brief 控制面板主窗口
 */
class PanelWindow : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit PanelWindow(QWidget *parent = nullptr);

private:
    void initUI();
};

#endif //__PANEL_WINDOW_H__