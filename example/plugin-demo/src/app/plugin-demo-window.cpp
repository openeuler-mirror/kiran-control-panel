/**
 * @file system-tray-window.cpp
 * @brief description
 * @author yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
*/
#include "plugin-demo-window.h"
#include "dialog001.h"
#include <QIcon>

PluginDemoWindow::PluginDemoWindow() : KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    setIcon(QIcon(APP_ICON_PATH));
    setTitle(tr("PluginDemo"));
    Dialog001 *centerWgt = new Dialog001(this);
    setWindowContentWidget(centerWgt);
    resize(628, 658);
}
