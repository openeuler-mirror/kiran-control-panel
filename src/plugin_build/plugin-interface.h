/**
 *@file  plugin-interface.h
 *@brief kiran-control-center插件接口实现
 *@auth  liuxinhao <liuxinhao@kylinos.com.cn>
 *@copyright (c) 2021 KylinSec. All rights reserved.
 */
#ifndef KIRAN_TIMEDATE_MANAGER_PLUGIN_INTERFACE_H
#define KIRAN_TIMEDATE_MANAGER_PLUGIN_INTERFACE_H

#include <module-interface.h>

extern QList<ModuleInterface::SubItem> getSubitems();
QWidget* getSubitemWidget(QString widgetName);
QString getTranslationPath();
bool hasUnsavedOptions();

#endif //KIRAN_TIMEDATE_MANAGER_PLUGIN_INTERFACE_H
