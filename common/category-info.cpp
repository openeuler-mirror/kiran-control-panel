//
// Created by lxh on 2021/5/8.
//
#include "category-info.h"
#include "config.h"

#include <qt5-log-i.h>
#include <QFileInfo>
#include <glib.h>

#define GROUP_DESKTOP_ENTRY "Desktop Entry"
#define KEY_NAME "Name"
#define KEY_COMMENT "Comment"
#define KEY_ICON "Icon"

#define GROUP_KIRAN_CONTROL_PANEL_CATEGORY "Kiran Control Panel Category"
#define KEY_CATEGORY "Category"
#define KEY_WEIGHT "Weight"
#define KEY_KEYWORDS "Keywords"

QSharedPointer<CPanelCategoryInfo> CPanelCategoryInfo::loadCategory(const QString& categoryPath)
{
    QSharedPointer<CPanelCategoryInfo> pSharedCategoryInfo(new CPanelCategoryInfo);
    if (!pSharedCategoryInfo->load(categoryPath))
    {
        KLOG_ERROR() << "can't load category desktop <" << categoryPath << ">!";
        return nullptr;
    }
    return pSharedCategoryInfo;
}

CPanelCategoryInfo::CPanelCategoryInfo()
{
}

CPanelCategoryInfo::~CPanelCategoryInfo()
{
    clear();
}

bool CPanelCategoryInfo::load(const QString& categoryPath)
{
    clear();

    if (!QFileInfo::exists(categoryPath))
    {
        KLOG_ERROR() << "category desktop file <" << categoryPath << "> isn't exist!";
        return false;
    }

    QSettings desktopSettings(categoryPath, QSettings::IniFormat);
    desktopSettings.setIniCodec("UTF-8");
    if (desktopSettings.status() != QSettings::NoError)
    {
        KLOG_ERROR() << "can't parse" << categoryPath << ",has error:" << desktopSettings.status();
        return false;
    }

    CategoryDesktopInfo desktopInfo;
    if(!parseDesktopInfo(categoryPath,
                          desktopInfo))
    {
        KLOG_ERROR() << "parser desktop entry group failed!," << categoryPath;
        return false;
    }

    m_categoryDesktopInfo = desktopInfo;
    m_isValid = true;
    return true;
}

bool CPanelCategoryInfo::isValid()
{
    return m_isValid;
}

const CPanelCategoryInfo::CategoryDesktopInfo& CPanelCategoryInfo::getCategoryDesktopInfo() const
{
    return m_categoryDesktopInfo;
}

void CPanelCategoryInfo::insertPlugin(QSharedPointer<CPanelPluginHelper> pluginHelper)
{
    auto pluginDesktopInfo = pluginHelper->getPluginDesktopInfo();
    if (pluginDesktopInfo.category != m_categoryDesktopInfo.categoryName)
    {
        KLOG_ERROR() << "plugin category can't match!,"
                       << "plugin category [" << pluginDesktopInfo.category << "],"
                       << "category [" << m_categoryDesktopInfo.categoryName << "]";
        return;
    }

    auto beforeIter = m_plugins.end();
    for (auto iter = m_plugins.begin(); iter != m_plugins.end(); iter++)
    {
        if (pluginDesktopInfo.weight > (*iter)->getPluginDesktopInfo().weight)
        {
            beforeIter = iter;
        }
    }
    m_plugins.insert(beforeIter, pluginHelper);
}

int CPanelCategoryInfo::pluginsCount()
{
    return m_plugins.count();
}

void CPanelCategoryInfo::clear()
{
    m_isValid = false;
    m_plugins.clear();
    m_categoryDesktopInfo = {};
}

bool CPanelCategoryInfo::parseDesktopInfo(const QString& desktopPath, CPanelCategoryInfo::CategoryDesktopInfo& desktopInfo)
{
    bool bRes = false;
    GKeyFile *keyFile = nullptr;
    GError *error = nullptr;
    std::string sDesktopPath = desktopPath.toStdString();
    gchar *name,*comment,*icon;
    gchar *category,*keywords;
    int weight;

    name = comment = icon = category = keywords = nullptr;

    keyFile = g_key_file_new();
    if(!g_key_file_load_from_file(keyFile,
                              sDesktopPath.c_str(),
                              G_KEY_FILE_KEEP_TRANSLATIONS,
                              &error))
    {
        KLOG_ERROR() << "can't parse" << desktopPath << (error?error->message:"");
        goto out;
    }

    name = g_key_file_get_locale_string(keyFile,GROUP_DESKTOP_ENTRY,KEY_NAME, nullptr,&error);
    if( !name )
    {
        KLOG_ERROR() << "missing" << GROUP_DESKTOP_ENTRY << KEY_NAME << (error?error->message:"");
        goto out;
    }
    desktopInfo.name = name;
    g_free(name);

    comment = g_key_file_get_locale_string(keyFile,GROUP_DESKTOP_ENTRY,KEY_COMMENT, nullptr,&error);
    if( !comment )
    {
        KLOG_ERROR() << "missing" << GROUP_DESKTOP_ENTRY << KEY_COMMENT << (error?error->message:"");
        goto out;
    }
    desktopInfo.comment = comment;
    g_free(comment);

    icon = g_key_file_get_string(keyFile,GROUP_DESKTOP_ENTRY,KEY_ICON,&error);
    if( !icon )
    {
        KLOG_ERROR() << "missing" << GROUP_DESKTOP_ENTRY << KEY_ICON << (error?error->message:"");
        goto out;
    }
    desktopInfo.icon = icon;
    g_free(icon);
    if( !desktopInfo.icon.startsWith('/') )
    {
        desktopInfo.icon.insert(0,CATEGORY_DESKTOP_ICON_DIR"/");
    }

    category = g_key_file_get_string(keyFile,GROUP_KIRAN_CONTROL_PANEL_CATEGORY,KEY_CATEGORY,&error);
    if( !category )
    {
        KLOG_ERROR() << "missing" << GROUP_KIRAN_CONTROL_PANEL_CATEGORY << KEY_CATEGORY << (error?error->message:"");
        goto out;
    }
    desktopInfo.categoryName = category;
    g_free(category);

    weight = g_key_file_get_int64(keyFile,GROUP_KIRAN_CONTROL_PANEL_CATEGORY,KEY_WEIGHT,&error);
    if( error)
    {
        KLOG_ERROR() << "missing" << GROUP_KIRAN_CONTROL_PANEL_CATEGORY << KEY_WEIGHT << error->message;
        g_error_free(error);
        error = nullptr;
    }
    desktopInfo.weight = weight;

    keywords = g_key_file_get_locale_string(keyFile,GROUP_KIRAN_CONTROL_PANEL_CATEGORY,KEY_KEYWORDS, nullptr,&error);
    if( !keywords )
    {
        KLOG_ERROR() << "missing" << GROUP_KIRAN_CONTROL_PANEL_CATEGORY << KEY_KEYWORDS << (error?error->message:"");
        goto out;
    }
    {
        QString temp = keywords;
        desktopInfo.keywords = temp.split(',');
        g_free(keywords);
    }
    bRes = true;
out:
    if( error )
        g_error_free(error);
    if( keyFile )
        g_key_file_free(keyFile);
    return bRes;
}

QSharedPointer<CPanelPluginHelper> CPanelCategoryInfo::getPlugin(int idx)
{
    if (idx >= m_plugins.count())
    {
        return nullptr;
    }
    return m_plugins.at(idx);
}

QList<QSharedPointer<CPanelPluginHelper>> CPanelCategoryInfo::getPlugins()
{
    return m_plugins;
}