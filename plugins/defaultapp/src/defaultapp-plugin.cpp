#include "defaultapp-plugin.h"
#include "config.h"
#include "defaultapp-subitem.h"

#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/appearance-i.h>
#include <QCoreApplication>
#include <tuple>

DefaultappPlugin::DefaultappPlugin(QObject* parent) : QObject(parent)
{
}

DefaultappPlugin::~DefaultappPlugin()
{
}

int DefaultappPlugin::init(KiranControlPanel::PanelInterface* interface)
{
    initTranslator();
    initSubItem();
    return 0;
}

void DefaultappPlugin::uninit()
{
    if (m_translator)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QVector<KiranControlPanel::SubItemPtr> DefaultappPlugin::getSubItems()
{
    return m_subitems;
}

void DefaultappPlugin::initSubItem()
{
    struct SubItemStruct
    {
        QString id;
        QString name;
        QString category;
        QString desc;
        QString icon;
        int weight;
        EnumMimeType enumMimeType;
    };
    QList<SubItemStruct> subitemInfos = {
        {"Browser",
         tr("Browser"),
         "defaultapp-manager",
         "",
         ":/kcp-defaultapp-images/browser-icon.svg",
         99,
         DA_TYPE_WEB_BROWSER},
        {"Email",
         tr("Email"),
         "defaultapp-manager",
         "",
         ":/kcp-defaultapp-images/email-icon.svg",
         98,
         DA_TYPE_EMAIL},
        {"Text",
         tr("Text"),
         "defaultapp-manager",
         "",
         ":/kcp-defaultapp-images/text-icon.svg",
         97,
         DA_TYPE_TEXT},
        {"Music",
         tr("Music"),
         "defaultapp-manager",
         "",
         ":/kcp-defaultapp-images/music-icon.svg",
         96,
         DA_TYPE_MEDIA},
        {"Video",
         tr("Video"),
         "defaultapp-manager",
         "",
         ":/kcp-defaultapp-images/video-icon.svg",
         95,
         DA_TYPE_VIDEO},
        {"Image",
         tr("Image"),
         "defaultapp-manager",
         "",
         ":/kcp-defaultapp-images/image-icon.svg",
         94,
         DA_TYPE_IMAGE},
        {"Document",
         tr("Document"),
         "defaultapp-manager",
         "",
         ":/kcp-defaultapp-images/terminal-icon.svg",
         93,
         DA_TYPE_DOCUMENT},
        {"Word",
         tr("Word"),
         "defaultapp-manager",
         "",
         ":/kcp-defaultapp-images/terminal-icon.svg",
         92,
         DA_TYPE_WORD},
        {"Spreadsheet",
         tr("Spreadsheet"),
         "defaultapp-manager",
         "",
         ":/kcp-defaultapp-images/terminal-icon.svg",
         91,
         DA_TYPE_SPREADSHEET}};

    for (const SubItemStruct& subitemInfo : subitemInfos)
    {
        DefaultAppSubItem* subitem = new DefaultAppSubItem(subitemInfo.enumMimeType);

        subitem->setID(subitemInfo.id);
        subitem->setName(subitemInfo.name);
        subitem->setCategory(subitemInfo.category);
        subitem->setDesc(subitemInfo.desc);
        subitem->setIcon(subitemInfo.icon);
        subitem->setWeight(subitemInfo.weight);
        m_subitems.append(KiranControlPanel::SubItemPtr(subitem));
    }
}

void DefaultappPlugin::initTranslator()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-defaultapp",
                            ".",
                            TRANSLATE_PREFIX,
                            ".qm"))
    {
        KLOG_DEBUG() << "Kiran cpanel defaultapp load translation failed";
        delete m_translator;
        m_translator = nullptr;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }
}