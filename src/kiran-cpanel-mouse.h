/**
 * @file kiran-cpanel-mouse.h
 * @brief  用于加载kiran标题栏的主界面
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#ifndef KIRANCPANELMOUSE_H
#define KIRANCPANELMOUSE_H

#include <QWidget>
#include <kiranwidgets-qt5/kiran-titlebar-window.h>

#include "kiran-cpanel-mouse-widget.h"

class KiranCPanelMouse : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit KiranCPanelMouse();
    ~KiranCPanelMouse();
    virtual QSize sizeHint() const override;

public:
    KiranCPanelMouseWidget* m_cpanelMouseWidget;

};

#endif // KIRANCPANELMOUSE_H
