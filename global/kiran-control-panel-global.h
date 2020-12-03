#ifndef KIRANCONTROLPANELGLOBAL_H
#define KIRANCONTROLPANELGLOBAL_H

#include <dlfcn.h>
#include "../interface/kiran-control-panel-module-interface.h"
#include <QWidget>
#include <QString>
#include <QMap>
#include <QHash>
#include <QDebug>
using namespace KiranControlPanelModuleInterface;


class QListWidgetItem;
namespace KiranControlPanelGlobal {
//########################### ModuleItemStu ############################################
typedef struct ModuleItemStu{
    bool isEmpty;
    QString name;
    QString nameZh;
    QString comment;
    QString commentZh;
    QString icon;
    QList<SubItem> subItems;
    QString translationPath;

    QString category;
    QString pluginFile;

public:
    void init()
    {
        openPlugin();
        getModuleItemSubInfo();
        getTranslationPath();
        closePlugin();
    }

    QString getNameTranslate();
    QString getCommentTranslate();
    QWidget *createModuleItemSubWgt(const QString &name);
    bool hasUnsavedOptions();
    void loadTranslator();
    void removeTranslator();

    ModuleItemStu():isEmpty(true),modulePlugin(nullptr){}
    ~ModuleItemStu(){
        closePlugin();
    }
    void closePlugin();

private:
    void* modulePlugin;

private:
    bool openPlugin();
    template <typename T>
    T getModuleItemFun(const QString &funName, T * = NULL)
    {
        if(!modulePlugin) return nullptr;
        T funPtr = (T) dlsym(modulePlugin, funName.toLatin1().data());
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            qWarning() << "Cannot load symbol create: " << dlsym_error;
            return nullptr;
        }
        return funPtr;
    }
    void getTranslationPath();
    void getModuleItemSubInfo();
}ModuleItem;

//############################### ModuleClassStu #########################################
typedef struct ModuleClassStu{
    ModuleClassStu():row(0){}
    QString name;
    QString nameZh;
    QString comment;
    QString commentZh;
    QString icon;

    QStringList keywords;
    int row;//用于搜索时选中分类项，减少查找计算。分类列表不会改变。

    QString getNameTranslate();
    QString getCommentTranslate();
    QMap<int, ModuleItem> itemMap;//分类中的子模块。
    QStringList itemKeys()const ;
}ModuleClass;

//#############################  ##################################3333
QMap<int, ModuleClass> getModuleCLass();
QHash<QString, QMap<int, ModuleItem> > getModuleItem();
ModuleItem getModuleItem(const QString &moduleName);

/*!
 *获取map排序中第row个元素。
 */
template <typename T>
T *getMapValueByRow(const int &row, QMap<int, T> &map, T * = NULL)
{
    if(map.isEmpty()) return nullptr;
    int index = 0;
    QMapIterator<int, T> i(map);
    while (i.hasNext()) {
        i.next();
        if(row == index)
        {
            return &map[i.key()];
        }
        ++index;
    }

    return nullptr;
}
}
Q_DECLARE_METATYPE(KiranControlPanelGlobal::ModuleClass*)
Q_DECLARE_METATYPE(KiranControlPanelGlobal::ModuleItem*)
#endif // KIRANCONTROLPANELGLOBAL_H
