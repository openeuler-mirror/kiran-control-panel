#include "kiran-control-center-global.h"
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

QString gLocaleName;
/*!
 * \brief KiranControlCenterGlobal::getModuleCLass
 * \return
 */
QMap<int, KiranControlCenterGlobal::ModuleClass> KiranControlCenterGlobal::getModuleCLass()
{
    QMap<int, KiranControlCenterGlobal::ModuleClass> ret;
    QHash<QString, QMap<int, KiranControlCenterGlobal::ModuleItem> >  items = getModuleItem();
    //
    QString moduleItemPath = CATEGORY_DESKTOP_DIR;
    QDirIterator it(moduleItemPath, QStringList() << "*.desktop", QDir::Files);
    while (it.hasNext()) {
        QSettings settings(it.next(), QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");

        KiranControlCenterGlobal::ModuleClass moduleClass;
        moduleClass.name = QString::fromUtf8(settings.value("Name").toString().toLatin1().data());
        moduleClass.nameZh = QString::fromUtf8(settings.value("Name[zh_CN]").toString().toLatin1().data());
        moduleClass.comment = settings.value("Comment").toString();
        moduleClass.commentZh = QString::fromUtf8(settings.value("Comment[zh_CN]").toString().toLatin1().data());
        moduleClass.icon = QString(CATEGORY_DESKTOP_ICON_DIR)+QDir::separator()+settings.value("Icon").toString();
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
 * \brief KiranControlCenterGlobal::getModuleItem 此函数在模块集成运行时被使用.
 * \return
 */
QHash<QString, QMap<int, KiranControlCenterGlobal::ModuleItem> > KiranControlCenterGlobal::getModuleItem()
{
    QHash<QString, QMap<int, KiranControlCenterGlobal::ModuleItem> >  ret;

    QString moduleItemPath = PLUGIN_DESKTOP_DIR;
    QDirIterator it(moduleItemPath, QStringList() << "*.desktop", QDir::Files);
    while (it.hasNext()) {
        QString filePath = it.next();
        QSettings settings(filePath, QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");

        QString category = settings.value("Category").toString();
        if(!ret.contains(category))
        {
            QMap<int, KiranControlCenterGlobal::ModuleItem> m;
            ret.insert(category, m);
        }

        QMap<int, KiranControlCenterGlobal::ModuleItem> &m = ret[category];

        KiranControlCenterGlobal::ModuleItem stu;
        stu.isEmpty    = false;
        stu.name       = QString::fromUtf8(settings.value("Name").toString().toLatin1().data());
        stu.nameZh     = QString::fromUtf8(settings.value("Name[zh_CN]").toString().toLatin1().data());
        stu.comment    = settings.value("Comment").toString();
        stu.commentZh  = QString::fromUtf8(settings.value("Comment[zh_CN]").toString().toLatin1().data());
        stu.icon       = PLUGIN_DESKTOP_ICON_DIR+settings.value("Icon").toString();
        stu.pluginFile = settings.value("PluginFile").toString();
        stu.init();

        m.insert(settings.value("Weight").toInt(), stu);
    }

    return ret;
}
/*!
 * \brief KiranControlCenterGlobal::getModuleItem 此函数在模块单独运行时被使用.
 * \param moduleName
 * \return
 */
KiranControlCenterGlobal::ModuleItem KiranControlCenterGlobal::getModuleItem(const QString &moduleName)
{
    KiranControlCenterGlobal::ModuleItem ret;

    QString moduleItemPath = PLUGIN_DESKTOP_DIR;
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
        ret.icon       = PLUGIN_DESKTOP_ICON_DIR+settings.value("Icon").toString();
        ret.pluginFile = settings.value("PluginFile").toString();
        ret.init();
        return ret;
    }

    return ret;
}

QString KiranControlCenterGlobal::ModuleClassStu::getNameTranslate()
{
    if(gLocaleName == "en_US") return name;
    else if(gLocaleName == "zh_CN") return nameZh;
    else return name;
}

QString KiranControlCenterGlobal::ModuleClassStu::getCommentTranslate()
{
    if(gLocaleName == "en_US") return comment;
    else if(gLocaleName == "zh_CN") return commentZh;
    else return comment;
}
/*!
 * \brief KiranControlCenterGlobal::ModuleClassStu::itemKeys
 * \return 返回所有功能项的搜索关键字。
 */
QStringList KiranControlCenterGlobal::ModuleClassStu::itemKeys() const
{
    QStringList ret;
    QMapIterator<int, KiranControlCenterGlobal::ModuleItem> i(itemMap);
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
/*!
 * \brief KiranControlCenterGlobal::ModuleItemStu::getModuleItemSubInfo
 * 从动态可中获取所有功能项的名称、图标、搜索关键字信息。
 */
void KiranControlCenterGlobal::ModuleItemStu::getModuleItemSubInfo()
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
        subItems[i].icon = subItems.at(i).icon;
    }

    closePlugin();
}

void KiranControlCenterGlobal::ModuleItemStu::loadTranslator()
{
    if(translator) return;

    QString qmFile = QString("%1.%2.qm").arg(translationPath).arg(gLocaleName);
    translator = new QTranslator();//在界面被切换之后释放。
    if(translator->load(qmFile) == false)
        qDebug() << "load qm: " << qmFile <<  " error.";
    else
        qApp->installTranslator(translator);
}
/*!
 * \brief KiranControlCenterGlobal::ModuleItemStu::createModuleItemSubWgt
 * \param name 根据功能项名称，创建功能项节目。
 * \return
 */
QWidget *KiranControlCenterGlobal::ModuleItemStu::createModuleItemSubWgt(const QString &name)
{
    openPlugin();
    //加载 要创建的界面的翻译文件。当界面被切换之后，翻译文件会被删除。
    loadTranslator();

    auto fun = getModuleItemFun<GetSubItemWidgetFun*>("getSubitemWidget");
    if(!fun) return nullptr;

    auto ret = fun(name);
    //closePlugin();
    return ret;
}
/*!
 * \brief KiranControlCenterGlobal::ModuleItemStu::getNameTranslate
 * \return  根据本地语言类型，返回模块的desktop文件中的模块名翻译。
 */
QString KiranControlCenterGlobal::ModuleItemStu::getNameTranslate()
{
    if(gLocaleName == "en_US") return name;
    else if(gLocaleName == "zh_CN") return nameZh;
    else return name;
}
/*!
 * \brief KiranControlCenterGlobal::ModuleItemStu::getCommentTranslate
 * \return 根据本地语言类型，返回模块的desktop文件中的介绍的翻译。
 */
QString KiranControlCenterGlobal::ModuleItemStu::getCommentTranslate()
{
    if(gLocaleName == "en_US") return comment;
    else if(gLocaleName == "zh_CN") return commentZh;
    else return comment;
}

bool KiranControlCenterGlobal::ModuleItemStu::openPlugin()
{
//    if(!modulePlugin)
//    {
        modulePlugin = dlopen(pluginFile.toLatin1().data(), RTLD_LAZY);
        if (!modulePlugin) {
            KiranMessageBox box;
            box.setTitle(QObject::tr("Tips"));

            QPushButton btn;
            btn.setText(QObject::tr("OK(K)"));
            btn.setFixedSize(QSize(200, box.buttonSize().height()));
            btn.setShortcut(Qt::CTRL + Qt::Key_K);
            box.addButton(&btn, QDialogButtonBox::AcceptRole);
            box.setText(QObject::tr("Failed to load plug-in %1。%2").arg(pluginFile).arg(dlerror()));
            box.exec();

            return false;
        }
        dlerror();
//    }

    return true;
}

void KiranControlCenterGlobal::ModuleItemStu::closePlugin()
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

bool KiranControlCenterGlobal::ModuleItemStu::hasUnsavedOptions()
{
    openPlugin();
    auto fun = getModuleItemFun<HasUnsavedOptionsFun*>("hasUnsavedOptions");
    if(!fun) return false;
    auto ret = fun();
    closePlugin();
    return ret;
}

void KiranControlCenterGlobal::ModuleItemStu::removeTranslator()
{
    if(translator)
    {
        qApp->removeTranslator(translator);
        delete translator;
        translator = nullptr;
    }
}

void KiranControlCenterGlobal::ModuleItemStu::getTranslationPath()
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

