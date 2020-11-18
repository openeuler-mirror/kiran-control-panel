#include "kiran-control-panel-global.h"
#include <QDirIterator>
#include <QSettings>

#define KIRAN_MODE_CLASS_PATH "/root/kiran-control-panel/example/desktop_modeclass"
#define KIRAN_MODE_ITEM_PATH "/root/kiran-control-panel/example/desktop_modeitem"


QMap<int, KiranControlPanelGlobal::ModelClass> KiranControlPanelGlobal::getModeCLass()
{
    QMap<int, KiranControlPanelGlobal::ModelClass> ret;
    QHash<QString, QMap<int, KiranControlPanelGlobal::ModelItem> >  items = getModeItem();
    //
    QString modeItemPath = KIRAN_MODE_CLASS_PATH;
    QDirIterator it(modeItemPath, QStringList() << "*.desktop", QDir::Files);
    while (it.hasNext()) {
        QSettings settings(it.next(), QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");

        KiranControlPanelGlobal::ModelClass modeClass;
        modeClass.name = QString::fromUtf8(settings.value("Name").toString().toLatin1().data());
        modeClass.nameZh = QString::fromUtf8(settings.value("Name[zh_CN]").toString().toLatin1().data());
        modeClass.comment = settings.value("Comment").toString();
        modeClass.commentZh = QString::fromUtf8(settings.value("Comment[zh_CN]").toString().toLatin1().data());
        modeClass.icon = "/usr/share/icons/Kiran/emblems/scalable/"+settings.value("Icon").toString();
        QVariantList keys = settings.value("Keywords").toList();
        foreach (QVariant var, keys) {
            modeClass.keywords <<QString::fromUtf8(var.toString().toLatin1().data());
        }

        modeClass.itemMap = items.value(modeClass.name);
        ret .insert(settings.value("Weight").toInt(), modeClass);
    }
    return ret;
}

QHash<QString, QMap<int, KiranControlPanelGlobal::ModelItem> > KiranControlPanelGlobal::getModeItem()
{
    QHash<QString, QMap<int, KiranControlPanelGlobal::ModelItem> >  ret;

    QString modeItemPath = KIRAN_MODE_ITEM_PATH;
    QDirIterator it(modeItemPath, QStringList() << "*.desktop", QDir::Files);
    while (it.hasNext()) {
        QString filePath = it.next();
        QSettings settings(filePath, QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");

        QString category = settings.value("Category").toString();
        if(!ret.contains(category))
        {
            QMap<int, KiranControlPanelGlobal::ModelItem> m;
            ret.insert(category, m);
        }

        QMap<int, KiranControlPanelGlobal::ModelItem> &m = ret[category];

        KiranControlPanelGlobal::ModelItem stu;
        stu.isEmpty    = false;
        stu.name       = QString::fromUtf8(settings.value("Name").toString().toLatin1().data());
        stu.nameZh     = QString::fromUtf8(settings.value("Name[zh_CN]").toString().toLatin1().data());
        stu.comment    = settings.value("Comment").toString();
        stu.commnetZh  = QString::fromUtf8(settings.value("Comment[zh_CN]").toString().toLatin1().data());
        stu.icon       = "/usr/share/icons/Kiran/emblems/scalable/"+settings.value("Icon").toString();
        stu.pluginFile = settings.value("PluginFile").toString();
        stu.init();

        m.insert(settings.value("Weight").toInt(), stu);
    }

    return ret;
}

KiranControlPanelGlobal::ModelItem KiranControlPanelGlobal::getModeItem(const QString &modeName)
{
    KiranControlPanelGlobal::ModelItem ret;

    QString modeItemPath = KIRAN_MODE_ITEM_PATH;
    QDirIterator it(modeItemPath, QStringList() << "*.desktop", QDir::Files);
    while (it.hasNext()) {
        QString filePath = it.next();
        QSettings settings(filePath, QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");
        if(settings.value("Name").toString() != modeName) continue;

        ret.isEmpty = false;
        ret.name       = QString::fromUtf8(settings.value("Name").toString().toLatin1().data());
        ret.nameZh     = QString::fromUtf8(settings.value("Name[zh_CN]").toString().toLatin1().data());
        ret.comment    = settings.value("Comment").toString();
        ret.commnetZh  = QString::fromUtf8(settings.value("Comment[zh_CN]").toString().toLatin1().data());
        ret.icon       = "/usr/share/icons/Kiran/emblems/scalable/"+settings.value("Icon").toString();
        ret.pluginFile = settings.value("PluginFile").toString();
        ret.init();
        return ret;
    }

    return ret;
}

QString KiranControlPanelGlobal::ModelClassStu::nameF()
{
    if(1) return nameZh;
}

QString KiranControlPanelGlobal::ModelClassStu::commentF()
{
    if(1) return commentZh;
}

QStringList KiranControlPanelGlobal::ModelClassStu::itemKeys() const
{
    QStringList ret;
    QMapIterator<int, KiranControlPanelGlobal::ModelItem> i(itemMap);
    while (i.hasNext()) {
        i.next();
        ret << i.value().sumKeyList;
    }
    return ret;
}

QListWidgetItem *KiranControlPanelGlobal::ModelClassStu::completeredItem(const QString &c)
{
    QMapIterator<int, KiranControlPanelGlobal::ModelItem> i(itemMap);
    while (i.hasNext()) {
        i.next();
        int index = i.value().sumKeyList.indexOf(c);
        if(index >= 0)
        {
            return i.value().subItemList.at(index);
        }
    }
    return nullptr;
}
int KiranControlPanelGlobal::ModelClassStu::completeredItemRow(const QString &c)
{
    int row = 0;
    QMapIterator<int, KiranControlPanelGlobal::ModelItem> i(itemMap);
    while (i.hasNext()) {
        i.next();
        if(i.value().sumKeyList.contains(c))
            return row;
        ++row;
    }
    return -1;
}

typedef void (*GetSubItemsFun)(QStringList &nameList, QStringList &iconList, QStringList &keyList);
typedef QWidget*(*GetSubItemWidgetFun)(QString);
typedef QString(*GetTranslationPathFun)();
typedef bool(*HasUnsavedOptionsFun)();

void KiranControlPanelGlobal::ModelItemStu::getModeItemSubInfo()
{
    openPlugin();
    auto fun = getModeItemFun<GetSubItemsFun>("get_subitems");
    if(!fun) return;

    subNameList.clear();
    subIconList.clear();
    sumKeyList.clear();
    fun(subNameList, subIconList, sumKeyList);
    closePlugin();
}

QWidget *KiranControlPanelGlobal::ModelItemStu::createModeItemSubWgt(const QString &name)
{
    openPlugin();
    auto fun = getModeItemFun<GetSubItemWidgetFun>("get_subitem_widget");
    if(!fun) return nullptr;
    auto ret = fun(name);
    closePlugin();
    return ret;
}

QString KiranControlPanelGlobal::ModelItemStu::nameF()
{
    if(1) return nameZh;
}

QString KiranControlPanelGlobal::ModelItemStu::commentF()
{
    if(1) return commnetZh;
}

bool KiranControlPanelGlobal::ModelItemStu::openPlugin()
{
    if(!modePlugin)
    {
        modePlugin = dlopen(pluginFile.toLatin1().data(), RTLD_LAZY);
        if (!modePlugin) {
            qWarning() << "Cannot load library: " << dlerror();
            return false;
        }
        dlerror();
    }

    return true;
}

void KiranControlPanelGlobal::ModelItemStu::closePlugin()
{
    if(modePlugin)
    {
        if(!dlclose(modePlugin))
        {
            modePlugin = nullptr;
            qDebug() << "dlclose plugin:" << modePlugin;
        }
    }
}

bool KiranControlPanelGlobal::ModelItemStu::hasUnsavedOptions()
{
    openPlugin();
    auto fun = getModeItemFun<HasUnsavedOptionsFun>("has_unsaved_options");
    if(!fun) return false;
    auto ret = fun();
    closePlugin();
    return ret;
}

void KiranControlPanelGlobal::ModelItemStu::getTranslationPath()
{
    openPlugin();
    auto fun = getModeItemFun<GetTranslationPathFun>("get_translation_path");
    if(!fun) return;
    translationPath = fun();
    closePlugin();
}

