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
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef __PANEL_WINDOW_H__
#define __PANEL_WINDOW_H__
#include "kiranwidgets-qt5/kiran-titlebar-window.h"
#include "panel-widget.h"

class PanelWidget;
/**
 * @brief 控制面板主窗口
 */
class PanelWindow : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit PanelWindow(QWidget *parent = nullptr);

    void jump(const QString& categoryName,const QString& subItem);

private:
    void initUI();

private slots:
    void handleInstanceStarted();
    void handleReceivedMessage( quint32 instanceId, QByteArray message );

private:
    PanelWidget* m_panelWidget = nullptr;
};

#endif //__PANEL_WINDOW_H__