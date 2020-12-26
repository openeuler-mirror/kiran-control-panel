/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include <QWidget>
#include <QStringList>

#ifdef PLUGIN_DESKTOP_DIR
#define KIRAN_MODULE_ITEM_DESKTOP_PATH PLUGIN_DESKTOP_DIR //将插件(模块)的desktop文件安装到此处。控制面板主程序将从这里读取插件的desktop，根据desktop描述加载插件。
#else
#define KIRAN_MODULE_ITEM_DESKTOP_PATH "/usr/share/kiran-control-panel/plugins" //将插件(模块)的desktop文件安装到此处。控制面板主程序将从这里读取插件的desktop，根据desktop描述加载插件。
#endif

namespace ModuleInterface {

typedef struct SubItemStu{
    /*!
     * \brief SubItemStu 功能项结构体
     * \param name 功能项名
     * \param icon 功能项图标 可以为图标的全路径，或只包含图标名称(将使用系统图标)，根据实际情况保证相应路径下图标存在。
     * \param key  功能项搜索关键字，可为空，为空时使用name替代key.
     */
    SubItemStu(const QString &name, const QString &icon, const QString &key){this->name = name; this->icon = icon; this->key = key;}
    QString name;
    QString icon;
    QString key;
}SubItem;

}

typedef QList<ModuleInterface::SubItem> (GetSubItemsFun)();
typedef QWidget*(GetSubItemWidgetFun)(QString);
typedef QString(GetTranslationPathFun)();
typedef bool(HasUnsavedOptionsFun)();

extern "C"  {
GetSubItemsFun getSubitems;//获取模块的功能项列表。模块中有多少个功能项，就对应多少个SubItem结构体。
GetSubItemWidgetFun getSubitemWidget;//获取给定名称的功能项的界面QWidget。给定的名称来自getSubitems函数返回的name,所以要保证这两处name名称一致.
/*!
 * \brief getTranslationPath 获取模块的翻译文件路径.只需返回全路径+翻译文件名，无需添加翻译文件后缀。
 * 如：‘/usr/share/kiran-control-panel/module/kiran-display-tools/kiran-display-tools.*_*.qm’ 只需返回
 * ‘/usr/share/kiran-control-panel/module/kiran-display-tools/kiran-display-tools’即可，主程序将根据当前翻译类型补全后缀名，加载合适的翻译文件。
 */
GetTranslationPathFun getTranslationPath;
HasUnsavedOptionsFun hasUnsavedOptions;//当前功能项是否有未保存的内容。当前功能项被修改且未保存时，返回true,否则返回false.
}

#endif // MODULEINTERFACE_H
