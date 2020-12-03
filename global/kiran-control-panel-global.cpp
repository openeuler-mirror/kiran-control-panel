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

#define KIRAN_MODULE_CLASS_DESKTOP_PATH "/usr/share/kiran-control-panel/categories"//分类desktop文件所在目录。
#define KIRAN_MODULE_CLASS_ICON_PATH QString("/usr/share/kiran-control-panel/icons")//分类图标目录。
#define KIRAN_MODULE_ITEM_ICON_PATH ""
#define KIRAN_MODULE_ITEM_FUNCTION_ICON_PATH ""

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
        moduleClass.icon = KIRAN_MODULE_CLASS_ICON_PATH+QDir::separator()+settings.value("Icon").toString();
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
        int count = i.value().subItems.count();
        for(int j=0; j<count; ++j)
        {
            ret << i.value().subItems.at(j).key;
        }
    }
    return ret;
}

void KiranControlPanelGlobal::ModuleItemStu::getModuleItemSubInfo()
{
    openPlugin();
    auto fun = getModuleItemFun<GetSubItemsFun*>("getSubitems");
    if(!fun) return;

    subItems = fun();
    //如果某一功能项关键字表为空,则使用功能项名称作为搜索关键字.
    int count = subItems.count();
    for(int i=0; i<count; ++i)
    {
        if(subItems.at(i).key.isEmpty())
        {
            subItems[i].key = subItems.at(i).name;
        }
        //给功能项图标添加全路径.
        subItems[i].icon = KIRAN_MODULE_ITEM_FUNCTION_ICON_PATH + subItems.at(i).icon;
    }

    closePlugin();
}

void KiranControlPanelGlobal::ModuleItemStu::loadTranslator()
{
    QString qmFile = QString("%1.%2.qm").arg(translationPath).arg(gLocaleName);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        qApp->installTranslator(&translator);
}

QWidget *KiranControlPanelGlobal::ModuleItemStu::createModuleItemSubWgt(const QString &name)
{
    openPlugin();
    QString qmFile = QString("%1.%2.qm").arg(translationPath).arg(gLocaleName);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        qApp->installTranslator(&translator);

    auto fun = getModuleItemFun<GetSubItemWidgetFun*>("getSubitemWidget");
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
    auto fun = getModuleItemFun<HasUnsavedOptionsFun*>("hasUnsavedOptions");
    if(!fun) return false;
    auto ret = fun();
    closePlugin();
    return ret;
}

void KiranControlPanelGlobal::ModuleItemStu::removeTranslator()
{
    QString qmFile = QString("%1.%2.qm").arg(translationPath).arg(gLocaleName);
    QTranslator translator;
    if(translator.load(qmFile) == false)
        qDebug() << "remove qm: " << qmFile <<  " error.";
    else
        qApp->removeTranslator(&translator);
}

void KiranControlPanelGlobal::ModuleItemStu::getTranslationPath()
{
    openPlugin();
    auto fun = getModuleItemFun<GetTranslationPathFun*>("getTranslationPath");
    if(!fun) return;
    translationPath = fun();
    closePlugin();

    translationPath = QDir::toNativeSeparators(translationPath);
//    if(!translationPath.endsWith(QDir::separator()))
//        translationPath += QDir::separator();
}

