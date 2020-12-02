/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANCONTROLPANELMODULEINTERFACE_H
#define KIRANCONTROLPANELMODULEINTERFACE_H

#include <QWidget>
#include <QStringList>

namespace KiranControlPanelModuleInterface {

typedef struct SubItemStu{
    /*!
     * \brief SubItemStu 功能项结构体
     * \param name 功能项名
     * \param icon 功能项图标
     * \param key  功能项搜索关键字
     */
    SubItemStu(const QString &name, const QString &icon, const QString &key){this->name = name; this->icon = icon; this->key = key;}
    QString name;
    QString icon;
    QString key;
}SubItem;

}

typedef QList<KiranControlPanelModuleInterface::SubItem> (GetSubItemsFun)();
typedef QWidget*(GetSubItemWidgetFun)(QString);
typedef QString(GetTranslationPathFun)();
typedef bool(HasUnsavedOptionsFun)();

extern "C"  {
GetSubItemsFun getSubitems;//获取模块的功能项列表。
GetSubItemWidgetFun getSubitemWidget;//获取给定名称的功能项的配置界面控件。
GetTranslationPathFun getTranslationPath;//获取模块的翻译文件路径。
HasUnsavedOptionsFun hasUnsavedOptions;//当前功能项是否有未保存的内容。
}

#endif // KIRANCONTROLPANELMODULEINTERFACE_H
