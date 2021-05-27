/**
 * @file kiran-control-center-single-plugin.h
 * @brief description
 * @author yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 * @copyright (c) 2021 KylinSec. All rights reserved.
*/
#ifndef KIRANCONTROLCENTERSINGLEPLUGIN_H
#define KIRANCONTROLCENTERSINGLEPLUGIN_H

#include "kiranwidgets-qt5/kiran-titlebar-window.h"
#include <QWidget>
#include "plugin-info.h"

class KiranModuleWidget;
class Launcher : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit Launcher(QWidget *parent = nullptr);
    ~Launcher();

    void setPlugins(const QList<QSharedPointer<CPanelPluginHelper>>& plugins );
    void setPlugin(QSharedPointer<CPanelPluginHelper> plguins);

public:
    QSize sizeHint() const override;

private:
    KiranModuleWidget* m_moduleWidget = nullptr;
};


#endif // KIRANCONTROLCENTERSINGLEPLUGIN_H
