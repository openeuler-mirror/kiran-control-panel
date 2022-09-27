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
 * Author:     yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 */

#ifndef KIRANCONTROLCENTERSINGLEPLUGIN_H
#define KIRANCONTROLCENTERSINGLEPLUGIN_H

#include <QWidget>
#include "kcp-plugin-subitem.h"
#include "kiranwidgets-qt5/kiran-titlebar-window.h"

class KiranModuleWidget;
class Launcher : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit Launcher(QWidget *parent = nullptr);
    ~Launcher();

    void setSubItems(QVector<KcpPluginSubItemPtr> subitems);

public:
    virtual QSize sizeHint() const override;

private slots:
    void handleActivateSlot();

private:
    KiranModuleWidget* m_moduleWidget = nullptr;
};


#endif // KIRANCONTROLCENTERSINGLEPLUGIN_H
