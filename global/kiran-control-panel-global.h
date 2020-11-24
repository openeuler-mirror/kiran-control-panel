#ifndef KIRANCONTROLPANELGLOBAL_H
#define KIRANCONTROLPANELGLOBAL_H

#include <dlfcn.h>
#include <QWidget>
#include <QString>
#include <QMap>
#include <QHash>
#include <QDebug>

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
    QStringList subNameList;
    QStringList subIconList;
    QStringList subKeyList;
    QString translationPath;

    QString category;
    QString pluginFile/*="/root/build-untitled-Desktop_Qt_5_9_1_GCC_64bit-Debug/libuntitled.so"*/;
    QList<QListWidgetItem *> subItemList;

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
    ModuleClassStu():item(nullptr){}
    QString name;
    QString nameZh;
    QString comment;
    QString commentZh;
    QString icon;

    QStringList keywords;
    QListWidgetItem *item;

    QString getNameTranslate();
    QString getCommentTranslate();
    QMap<int, ModuleItem> itemMap;
    QStringList itemKeys()const ;
    QListWidgetItem *completeredItem(const QString &c);
    int completeredItemRow(const QString &c);
}ModuleClass;

//#############################  ##################################3333
QMap<int, ModuleClass> getModuleCLass();
QHash<QString, QMap<int, ModuleItem> > getModuleItem();
ModuleItem getModuleItem(const QString &moduleName);
}

#endif // KIRANCONTROLPANELGLOBAL_H
