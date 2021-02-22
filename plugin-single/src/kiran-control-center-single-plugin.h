/**
 * @file kiran-control-center-single-plugin.h
 * @brief description
 * @author yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 * @copyright (c) 2021 KylinSec. All rights reserved.
*/
#ifndef KIRANCONTROLCENTERSINGLEPLUGIN_H
#define KIRANCONTROLCENTERSINGLEPLUGIN_H

#include "kiranwidgets-qt5/kiran-titlebar-window.h"
#include "kiran-control-center-global.h"
#include <QWidget>
using namespace KiranControlCenterGlobal;

class KiranControlCenterSinglePlugin : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit KiranControlCenterSinglePlugin();
    ~KiranControlCenterSinglePlugin();
    void setModuleName(const QString &name);

private:
     QMap<int, ModuleItem > m_map;
};


#endif // KIRANCONTROLCENTERSINGLEPLUGIN_H
