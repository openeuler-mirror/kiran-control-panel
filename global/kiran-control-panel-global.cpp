#include "kiran-control-panel-global.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include <QDirIterator>
#include <QSettings>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTranslator>
#include <QApplication>
#include <QDir>
#include <QApplication>
#include <QTranslator>
#include <QDebug>

#define KIRAN_MODULE_CLASS_DESKTOP_PATH "/usr/share/applications/kiran-control-panel/class"
#define KIRAN_MODULE_ITEM_DESKTOP_PATH "/usr/share/applications/kiran-control-panel/module"
#define KIRAN_MODULE_CLASS_ICON_PATH "/usr/share/icons/Kiran/emblems/scalable/"
#define KIRAN_MODULE_ITEM_ICON_PATH "/usr/share/icons/Kiran/emblems/scalable/"
#define KIRAN_MODULE_ITEM_FUNCTION_ICON_PATH "/usr/share/icons/Kiran/emblems/scalable/"

typedef void (*GetSubItemsFun)(QStringList &nameList, QStringList &iconList, QStringList &keyList);
typedef QWidget*(*GetSubItemWidgetFun)(QString);
typedef QString(*GetTranslationPathFun)();
typedef bool(*HasUnsavedOptionsFun)();

QString gLocaleName;
/*!
 * \brief KiranControlPanelGlobal::getModuleCLass
 * \return
 */
QMap<int, KiranControlPanelGlobal::ModuleClass> KiranControlPanelGlobal::getModuleCLass()
{
    QMap<int, KiranControlPanelGlobal::ModuleClass> ret;
    QHash<QString, QMap<int, KiranControlPanelGlobal::ModuleItem> >  items = getModuleItem();
    //
    QString moduleItemPath = KIRAN_MODULE_CLASS_DESKTOP_PATH;
    QDirIterator it(moduleItemPath, QStringList() << "*.desktop", QDir::Files);
    while (it.hasNext()) {
        QSettings settings(it.next(), QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");

        KiranControlPanelGlobal::ModuleClass moduleClass;
        moduleClass.name = QString::fromUtf8(settings.value("Name").toString().toLatin1().data());
        moduleClass.nameZh = QString::fromUtf8(settings.value("Name[zh_CN]").toString().toLatin1().data());
        moduleClass.comment = settings.value("Comment").toString();
        moduleClass.commentZh = QString::fromUtf8(settings.value("Comment[zh_CN]").toString().toLatin1().data());
        moduleClass.icon = KIRAN_MODULE_CLASS_ICON_PATH+settings.value("Icon").toString();
        QVariantList keys = settings.value("Keywords").toList();
        foreach (QVariant var, keys) {
            moduleClass.keywords <<QString::fromUtf8(var.toString().toLatin1().data());
        }

        moduleClass.itemMap = items.value(moduleClass.name);
        ret .insert(settings.value("Weight").toInt(), moduleClass);
    }
    return ret;
}
/*!
 * \brief KiranControlPanelGlobal::getModuleItem 此函数在模块集成运行时被使用.
 * \return
 */
QHash<QString, QMap<int, KiranControlPanelGlobal::ModuleItem> > KiranControlPanelGlobal::getModuleItem()
{
    QHash<QString, QMap<int, KiranControlPanelGlobal::ModuleItem> >  ret;

    QString moduleItemPath = KIRAN_MODULE_ITEM_DESKTOP_PATH;
    QDirIterator it(moduleItemPath, QStringList() << "*.desktop", QDir::Files);
    while (it.hasNext()) {
        QString filePath = it.next();
        QSettings settings(filePath, QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");

        QString category = settings.value("Category").toString();
        if(!ret.contains(category))
        {
            QMap<int, KiranControlPanelGlobal::ModuleItem> m;
            ret.insert(category, m);
        }

        QMap<int, KiranControlPanelGlobal::ModuleItem> &m = ret[category];

        KiranControlPanelGlobal::ModuleItem stu;
        stu.isEmpty    = false;
        stu.name       = QString::fromUtf8(settings.value("Name").toString().toLatin1().data());
        stu.nameZh     = QString::fromUtf8(settings.value("Name[zh_CN]").toString().toLatin1().data());
        stu.comment    = settings.value("Comment").toString();
        stu.commentZh  = QString::fromUtf8(settings.value("Comment[zh_CN]").toString().toLatin1().data());
        stu.icon       = KIRAN_MODULE_ITEM_ICON_PATH+settings.value("Icon").toString();
        stu.pluginFile = settings.value("PluginFile").toString();
        stu.init();

        m.insert(settings.value("Weight").toInt(), stu);
    }

    return ret;
}
/*!
 * \brief KiranControlPanelGlobal::getModuleItem 此函数在模块单独运行时被使用.
 * \param moduleName
 * \return
 */
KiranControlPanelGlobal::ModuleItem KiranControlPanelGlobal::getModuleItem(const QString &moduleName)
{
    KiranControlPanelGlobal::ModuleItem ret;

    QString moduleItemPath = KIRAN_MODULE_ITEM_DESKTOP_PATH;
    QDirIterator it(moduleItemPath, QStringList() << "*.desktop", QDir::Files);
    while (it.hasNext()) {
        QString filePath = it.next();
        QSettings settings(filePath, QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");
        if(settings.value("Name").toString() != moduleName) continue;

        ret.isEmpty = false;
        ret.name       = QString::fromUtf8(settings.value("Name").toString().toLatin1().data());
        ret.nameZh     = QString::fromUtf8(settings.value("Name[zh_CN]").toString().toLatin1().data());
        ret.comment    = settings.value("Comment").toString();
        ret.commentZh  = QString::fromUtf8(settings.value("Comment[zh_CN]").toString().toLatin1().data());
        ret.icon       = KIRAN_MODULE_ITEM_ICON_PATH+settings.value("Icon").toString();
        ret.pluginFile = settings.value("PluginFile").toString();
        ret.init();
        return ret;
    }

    return ret;
}

QString KiranControlPanelGlobal::ModuleClassStu::getNameTranslate()
{
    if(gLocaleName == "en_US") return name;
    else if(gLocaleName == "zh_CN") return nameZh;
    else return name;
}

QString KiranControlPanelGlobal::ModuleClassStu::getCommentTranslate()
{
    if(gLocaleName == "en_US") return comment;
    else if(gLocaleName == "zh_CN") return commentZh;
    else return comment;
}

QStringList KiranControlPanelGlobal::ModuleClassStu::itemKeys() const
{
    QStringList ret;
    QMapIterator<int, KiranControlPanelGlobal::ModuleItem> i(itemMap);
    while (i.hasNext()) {
        i.next();
        ret << i.value().subKeyList;
    }
    return ret;
}

int KiranControlPanelGlobal::ModuleClassStu::completeredItemRow(const QString &c)
{
    int row = 0;
    QMapIterator<int, KiranControlPanelGlobal::ModuleItem> i(itemMap);
    while (i.hasNext()) {
        i.next();
        if(i.value().subKeyList.contains(c))
            return row;
        ++row;
    }
    return -1;
}

void KiranControlPanelGlobal::ModuleItemStu::getModuleItemSubInfo()
{
    openPlugin();
    auto fun = getModuleItemFun<GetSubItemsFun>("getSubitems");
    if(!fun) return;

    subNameList.clear();
    subIconList.clear();
    subKeyList.clear();
    fun(subNameList, subIconList, subKeyList);
    if(subNameList.count() != subIconList.count() || subIconList.count() != subKeyList.count())
    {
        qWarning() << "module name、icon、key quantity discrepancy!";
    }
    else
    {
        //如果某一功能项关键字表为空,则使用功能项名称作为搜索关键字.
        int count = subNameList.count();
        for(int i=0; i<count; ++i)
        {
            if(subKeyList.at(i).isEmpty())
            {
                subKeyList[i] = subNameList.at(i);
            }
            //给功能项图标添加全路径.
            subIconList[i] = KIRAN_MODULE_ITEM_FUNCTION_ICON_PATH + subIconList.at(i);
        }
    }
    //如果模块中所有功能项关键字都为空,则整体使用功能项名称作为关键字.
    if(subKeyList.isEmpty()) subKeyList = subNameList;
    closePlugin();
}

QWidget *KiranControlPanelGlobal::ModuleItemStu::createModuleItemSubWgt(const QString &name)
{
    QString qmFile = QString("%1%2.%3.qm").arg(translationPath).arg(this->name).arg(gLocaleName);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        qApp->installTranslator(&translator);

    openPlugin();
    auto fun = getModuleItemFun<GetSubItemWidgetFun>("getSubitemWidget");
    if(!fun) return nullptr;
    auto ret = fun(name);
    //closePlugin();
    return ret;
}

QString KiranControlPanelGlobal::ModuleItemStu::getNameTranslate()
{
    if(gLocaleName == "en_US") return name;
    else if(gLocaleName == "zh_CN") return nameZh;
    else return name;
}

QString KiranControlPanelGlobal::ModuleItemStu::getCommentTranslate()
{
    if(gLocaleName == "en_US") return comment;
    else if(gLocaleName == "zh_CN") return commentZh;
    else return comment;
}

bool KiranControlPanelGlobal::ModuleItemStu::openPlugin()
{
//    if(!modulePlugin)
//    {
        modulePlugin = dlopen(pluginFile.toLatin1().data(), RTLD_LAZY);
        if (!modulePlugin) {
            KiranMessageBox box;
            box.setTitle(QObject::tr("提示"));

            QPushButton btn;
            btn.setText(QObject::tr("确定(K)"));
            btn.setFixedSize(QSize(200, box.buttonSize().height()));
            btn.setShortcut(Qt::CTRL + Qt::Key_K);
            box.addButton(&btn, QDialogButtonBox::AcceptRole);
            box.setText(QObject::tr("加载插件%1失败。%2").arg(pluginFile).arg(dlerror()));
            box.exec();

            return false;
        }
        dlerror();
//    }

    return true;
}

void KiranControlPanelGlobal::ModuleItemStu::closePlugin()
{
    if(modulePlugin)
    {
        if(!dlclose(modulePlugin))
        {
            //关闭插件成功，计数器为0，将句柄置空，防止再次操作该句柄。
            modulePlugin = nullptr;
        }
    }
}

bool KiranControlPanelGlobal::ModuleItemStu::hasUnsavedOptions()
{
    openPlugin();
    auto fun = getModuleItemFun<HasUnsavedOptionsFun>("hasUnsavedOptions");
    if(!fun) return false;
    auto ret = fun();
    closePlugin();
    return ret;
}

void KiranControlPanelGlobal::ModuleItemStu::loadTranslator()
{
    QString qmFile = QString("%1%2.%3.qm").arg(translationPath).arg(this->name).arg(gLocaleName);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        qApp->installTranslator(&translator);
}

void KiranControlPanelGlobal::ModuleItemStu::removeTranslator()
{
    QString qmFile = QString("%1%2.%3.qm").arg(translationPath).arg(this->name).arg(gLocaleName);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        qApp->removeTranslator(&translator);
}

void KiranControlPanelGlobal::ModuleItemStu::getTranslationPath()
{
    openPlugin();
    auto fun = getModuleItemFun<GetTranslationPathFun>("getTranslationPath");
    if(!fun) return;
    translationPath = fun();
    closePlugin();

    translationPath = QDir::toNativeSeparators(translationPath);
    if(!translationPath.endsWith(QDir::separator()))
        translationPath += QDir::separator();
}

