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
//########################### ModelItemStu ############################################
typedef struct ModelItemStu{
    bool isEmpty;
    QString name;
    QString nameZh;
    QString comment;
    QString commnetZh;
    QString icon;
    QStringList subNameList;
    QStringList subIconList;
    QStringList sumKeyList;
    QString translationPath;

    QString category;
    QString pluginFile/*="/root/build-untitled-Desktop_Qt_5_9_1_GCC_64bit-Debug/libuntitled.so"*/;
    QList<QListWidgetItem *> subItemList;

public:
    void init()
    {
        openPlugin();
        getModeItemSubInfo();
        getTranslationPath();
        closePlugin();
    }

    QString nameF();
    QString commentF();
    QWidget *createModeItemSubWgt(const QString &name);
    bool hasUnsavedOptions();

    ModelItemStu():isEmpty(true),modePlugin(nullptr){}
    ~ModelItemStu(){
        closePlugin();
    }
    void closePlugin();

private:
    void* modePlugin;

private:
    bool openPlugin();
    template <typename T>
    T getModeItemFun(const QString &funName, T * = NULL)
    {
        if(!modePlugin) return nullptr;
        T funPtr = (T) dlsym(modePlugin, funName.toLatin1().data());
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            qWarning() << "Cannot load symbol create: " << dlsym_error;
            return nullptr;
        }
        return funPtr;
    }
    void getTranslationPath();
    void getModeItemSubInfo();
}ModelItem;

//############################### ModelClassStu #########################################
typedef struct ModelClassStu{
    ModelClassStu():item(nullptr){}
    QString name;
    QString nameZh;
    QString comment;
    QString commentZh;
    QString icon;

    QStringList keywords;
    QListWidgetItem *item;

    QString nameF();
    QString commentF();
    QMap<int, ModelItem> itemMap;
    QStringList itemKeys()const ;
    QListWidgetItem *completeredItem(const QString &c);
    int completeredItemRow(const QString &c);
}ModelClass;

//#############################  ##################################3333
QMap<int, ModelClass> getModeCLass();
QHash<QString, QMap<int, ModelItem> > getModeItem();
ModelItem getModeItem(const QString &modeName);
}

#endif // KIRANCONTROLPANELGLOBAL_H
