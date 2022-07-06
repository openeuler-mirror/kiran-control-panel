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

#ifndef KIRANMODULEWIDGET_H
#define KIRANMODULEWIDGET_H

#include <QWidget>
#include "plugin-info.h"

namespace Ui
{
class KiranModuleWidget;
}

class QListWidgetItem;
class KiranModuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KiranModuleWidget(QWidget *parent = 0);
    ~KiranModuleWidget();

    void setLeftContentsMargins(const int &leftmargin);

    void clear();
    void setPlugins(const PluginHelperPtrList &plugins);
    bool checkHasUnSaved();
    void jumpTo(const QString& subItemID);

    bool eventFilter(QObject *watched, QEvent *event) override;
private slots:
    void handleCurrentItemChanged();
    void handlePluginVisibleSubItemsChanged();

private:
    void init();

private:
    PluginHelperPtrList m_plugins;
    Ui::KiranModuleWidget *ui;
    int m_currentSubItemIdx = -1;
    QWidget *m_subItemWidget = nullptr;
};

#endif  // KIRANMODULEWIDGET_H
