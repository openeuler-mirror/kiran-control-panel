#include "network-subitem.h"
#include "cpanel-network-widget.h"
#include <QEvent>
#include <qt5-log-i.h>
#include "utils.h"

NetworkSubItem::NetworkSubItem(KiranControlPanel::PanelInterface* interface, QObject *parent)
    : m_interface(interface),
      QObject(parent)
{
}

NetworkSubItem::~NetworkSubItem()
{
}

bool NetworkSubItem::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_subitemWidget && event->type() == QEvent::Destroy)
    {
        m_subitemWidget = nullptr;
    }
    return QObject::eventFilter(watched, event);
}

QString NetworkSubItem::getID()
{
    return "Network";
}

QString NetworkSubItem::getName()
{
    return "";
}

QString NetworkSubItem::getCategory()
{
    return "network";
}

QString NetworkSubItem::getDesc()
{
    return "";
}

QString NetworkSubItem::getIcon()
{
    return "";
}

int NetworkSubItem::getWeight()
{
    return 0;
}

QWidget *NetworkSubItem::createWidget()
{
    m_subitemWidget = new CPanelNetworkWidget;
    m_subitemWidget->installEventFilter(this);
    connect(m_subitemWidget,&CPanelNetworkWidget::subItemsChanged,this,&NetworkSubItem::handleSubItemsChanged);
    return m_subitemWidget;
}

QVector<QPair<QString, QString> > NetworkSubItem::getSearchKeys()
{
    QVector<QPair<QString, QString>> searchKeys;
    m_subItemsList =  subItemsList();

    foreach(auto subItem,m_subItemsList)
    {
        KLOG_DEBUG() << "subItem:" << subItem;
        searchKeys.append({subItem,subItem});
    }

    return searchKeys;
}

bool NetworkSubItem::jumpToSearchEntry(const QString &key)
{
    if (!m_subitemWidget)
        return false;

    int index = m_subItemsList.indexOf(key);
    if(index != -1)
    {
        m_subitemWidget->setCurrentSubItem(index);
    }
    else
        return false;

    return true;
}

bool NetworkSubItem::haveUnSavedOptions()
{
    return false;
}

void NetworkSubItem::handleSubItemsChanged()
{
    m_interface->handlePluginSubItemInfoChanged(getID());
}

QStringList NetworkSubItem::subItemsList()
{
    QStringList subItemsList;
    auto wiredList = NetworkUtils::getManagedDeviceList(NetworkManager::Device::Ethernet);
    auto wirelessList = NetworkUtils::getManagedDeviceList(NetworkManager::Device::Wifi);
    for (int i = 0; i < wiredList.count(); i++)
    {
        QString subItemName = tr("Wired Network %1");
        QString subItemNameStr = subItemName.arg(i + 1);
        if (wiredList.count() == 1)
        {
            QString name = tr("Wired Network");
            subItemsList << name;
        }
        else
            subItemsList << subItemNameStr;
    }

    for (int i = 0; i < wirelessList.count(); i++)
    {
        QString subItemName = tr("Wireless Network %1");
        QString subItemNameStr = subItemName.arg(i + 1);

        if (wirelessList.count() == 1)
        {
            QString name = tr("Wireless Network");
            subItemsList << name;
        }
        else
            subItemsList << subItemNameStr;
    }

    subItemsList << tr("VPN");
    subItemsList << tr("Network Details");

    return subItemsList;
}
