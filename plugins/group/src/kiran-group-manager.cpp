/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-group is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     wangshichang <shichang@isrc.iscas.ac.cn>
 */
#include "kiran-group-manager.h"
#include "accounts-global-info.h"
#include "config.h"
#include "create-group-page/create-group-page.h"
#include "group-info-page/group-info-page.h"
#include "groups-global-info.h"
#include "hard-worker.h"
#include "kiran-color-block.h"
#include "mask-widget.h"

#include <kiran-sidebar-widget.h>
#include <qt5-log-i.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QScrollArea>
#include <QStackedWidget>
#include <QtWidgets/QListWidgetItem>

#define ITEM_GROUP_OBJ_PATH_ROLE Qt::UserRole + 1

KiranGroupManager::KiranGroupManager(QWidget *parent)
    : QWidget(parent)
{
    m_workThread.start();
    m_hardworker = new HardWorker();
    m_hardworker->moveToThread(&m_workThread);

    KLOG_INFO() << "WorkThread:" << m_workThread.currentThreadId();
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();
    initUI();
}

KiranGroupManager::~KiranGroupManager()
{
    if (m_workThread.isRunning())
    {
        m_workThread.quit();
        m_workThread.wait();
    }
    delete m_hardworker;
}

void KiranGroupManager::initUI()
{
    /*初始化界面主布局*/
    auto contentLayout = new QHBoxLayout(this);
    contentLayout->setSpacing(0);
    contentLayout->setObjectName("GroupContentLayout");
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(4);

    /* 侧边栏 */
    auto siderbar = new KiranColorBlock(this);
    contentLayout->addWidget(siderbar);
    siderbar->setObjectName("siderWidget");
    siderbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    siderbar->setFixedWidth(272);

    auto vLayout = new QVBoxLayout(siderbar);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setObjectName("SiderbarVLayout");

    m_tabList = new KiranSidebarWidget(siderbar);
    m_tabList->setInvertIconPixelsEnable(true);
    m_tabList->viewport()->setAutoFillBackground(false);
    m_tabList->setFrameShape(QFrame::NoFrame);
    m_tabList->setObjectName("tabList");
    m_tabList->setIconSize(QSize(40, 40));
    vLayout->addWidget(m_tabList);
    initGroupList();

    /*内容区域*/
    KiranColorBlock *stackedColorBlock = new KiranColorBlock(this);
    QHBoxLayout *stackedColorBlockLayout = new QHBoxLayout(stackedColorBlock);

    m_stackWidget = new QStackedWidget(this);
    m_stackWidget->setObjectName("StackWidget");
    stackedColorBlockLayout->addWidget(m_stackWidget);
    contentLayout->addWidget(stackedColorBlock);

    m_pageCreateGroup = new CreateGroupPage(m_stackWidget);
    m_stackWidget->insertWidget(PAGE_CREATE_GROUP, m_pageCreateGroup);
    initPageCreateGroup();

    m_page_groupInfo = new GroupInfoPage(m_stackWidget);
    m_stackWidget->insertWidget(PAGE_GROUP_INFO, m_page_groupInfo);
    initPageGroupInfo();

    m_currentItemData = "";
    QTimer::singleShot(0, this, &KiranGroupManager::setDefaultSiderbarItem);
}

void KiranGroupManager::initGroupList()
{
    connect(m_tabList, &KiranSidebarWidget::itemSelectionChanged, [this]()
            {
                QList<QListWidgetItem *> selecteds = m_tabList->selectedItems();
                if (selecteds.size() != 1)
                {
                    return;
                }
                QListWidgetItem *item = selecteds.at(0);
                if (item == m_createGroupItem)
                {
                    //重置创建用户组页面
                    m_pageCreateGroup->reset();
                    //切换到创建用户组
                    m_stackWidget->setCurrentIndex(PAGE_CREATE_GROUP);
                    m_currentItemData = "";
                }
                else
                {
                    QString groupObjPath = item->data(ITEM_GROUP_OBJ_PATH_ROLE).toString();
                    //更新用户组信息页面
                    m_page_groupInfo->setCurrentShowGroupPath(groupObjPath);
                    //切换到用户组信息
                    m_stackWidget->setCurrentIndex(PAGE_GROUP_INFO);
                    m_currentItemData = groupObjPath;
                } });

    /// 创建用户组按钮
    m_createGroupItem = new QListWidgetItem(tr("Create new group"), m_tabList);
    m_tabList->addItem(m_createGroupItem);

    m_createGroupItem->setIcon(QIcon(":/kcp-group-images/create_group_icon.png"));

    QList<QString> groupObjList;
    groupObjList = GroupsGlobalInfo::instance()->getGroupList();
    for (auto &iter : groupObjList)
    {
        if (isNoSystemGroup(iter))
        {
            KSDGroupAdminListProxy interface(GROUP_ADMIN_DBUS_NAME, iter, QDBusConnection::systemBus());

            auto item = new QListWidgetItem(interface.groupName(), m_tabList);
            item->setIcon(QPixmap(":/kcp-group-images/group_icon_small.png"));
            item->setData(Kiran::ItemStatusColor_Role, QColor("#fa4949"));
            item->setData(ITEM_GROUP_OBJ_PATH_ROLE, iter);
            m_tabList->addItem(item);
        }
    }
}

bool KiranGroupManager::isNoSystemGroup(const QString &groupPath)
{
    auto groupProxy = new KSDGroupAdminListProxy(GROUP_ADMIN_DBUS_NAME,
                                                 groupPath,
                                                 QDBusConnection::systemBus(),
                                                 this);

    if (!AccountsGlobalInfo::instance()->checkUserNameAvaliable(groupProxy->groupName()))
    {
        return false;
    }

    bool ret = (groupProxy->gid() >= 1000 && groupProxy->gid() < 65534);

    delete groupProxy;

    return ret;
}

void KiranGroupManager::appendSidebarItem(QString groupPath, QString errMsg)
{
    if (!errMsg.isEmpty())
    {
        return;
    }
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();
    KSDGroupAdminListProxy interface(GROUP_ADMIN_DBUS_NAME, groupPath, QDBusConnection::systemBus());

    QString iconFile = ":/kcp-group-images/group_icon_small.png";

    auto item = new QListWidgetItem(interface.groupName(), m_tabList);
    item->setIcon(QPixmap(iconFile));
    item->setData(Kiran::ItemStatusColor_Role, QColor("#fa4949"));
    item->setData(ITEM_GROUP_OBJ_PATH_ROLE, groupPath);

    m_tabList->addItem(item);
    m_tabList->setCurrentItem(item);

    // 更新用户组信息页面
    m_page_groupInfo->setCurrentShowGroupPath(groupPath);
    // 切换到用户组信息
    m_stackWidget->setCurrentIndex(PAGE_GROUP_INFO);
    m_currentItemData = groupPath;
}

void KiranGroupManager::deleteSidebarItem(QString groupName, QString errMsg)
{
    if (!errMsg.isEmpty())
    {
        return;
    }
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();
    auto itemCount = m_tabList->count();

    for (int i = 0; i < itemCount; i++)
    {
        auto item = m_tabList->item(i);
        if (item->text() == groupName)
        {
            delete item;
            break;
        }
    }
    setDefaultSiderbarItem();
    // 重置创建用户组页面
    m_pageCreateGroup->reset();
    // 切换到创建用户组
    m_stackWidget->setCurrentIndex(PAGE_CREATE_GROUP);
    m_currentItemData = "";
}

void KiranGroupManager::updateSidebarItem(QString groupPath, QString errMsg)
{
    if (!errMsg.isEmpty())
    {
        return;
    }
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();
    auto itemCount = m_tabList->count();
    for (int i = 0; i < itemCount; i++)
    {
        auto item = m_tabList->item(i);
        if (item->data(ITEM_GROUP_OBJ_PATH_ROLE) == groupPath)
        {
            KSDGroupAdminListProxy interface(GROUP_ADMIN_DBUS_NAME, groupPath, QDBusConnection::systemBus());
            item->setText(interface.groupName());
            m_tabList->setCurrentItem(item);
            // 更新用户组信息页面
            m_page_groupInfo->setCurrentShowGroupPath(groupPath);
            // 切换到用户组信息
            m_stackWidget->setCurrentIndex(PAGE_GROUP_INFO);
            m_currentItemData = groupPath;
        }
    }
}

void KiranGroupManager::initPageCreateGroup()
{
    connect(m_pageCreateGroup, &CreateGroupPage::sigCreateGroup,
            m_hardworker, &HardWorker::doCreateGroup);
    connect(m_hardworker, &HardWorker::sigCreateGroupDone,
            m_pageCreateGroup, &CreateGroupPage::handlerCreateNewGroupIsDone);
    connect(m_hardworker, &HardWorker::sigCreateGroupDone,
            this, &KiranGroupManager::appendSidebarItem);

    connect(m_pageCreateGroup, &CreateGroupPage::sigAddUserToGroup,
            m_hardworker, &HardWorker::doAddUserToGroup);
    connect(m_hardworker, &HardWorker::sigAddUserToGroupDone,
            m_pageCreateGroup, &CreateGroupPage::handlerAddUserToGroupIsDone);
}

void KiranGroupManager::initPageGroupInfo()
{
    connect(m_page_groupInfo, &GroupInfoPage::sigRemoveMember,
            m_hardworker, &HardWorker::doRemoveMemberFromGroup);
    connect(m_hardworker, &HardWorker::sigRemoveMemberFromGroupDone,
            m_page_groupInfo, &GroupInfoPage::handlerRemoveMemberIsDone);

    connect(m_page_groupInfo, &GroupInfoPage::sigDeleteGroup,
            m_hardworker, &HardWorker::doDeleteGroup);
    connect(m_hardworker, &HardWorker::sigDeleteGroupDone,
            m_page_groupInfo, &GroupInfoPage::handlerDeleteGroupIsDone);
    connect(m_hardworker, &HardWorker::sigDeleteGroupDone,
            this, &KiranGroupManager::deleteSidebarItem);

    connect(m_page_groupInfo, &GroupInfoPage::sigChangeGroupName,
            m_hardworker, &HardWorker::doChangeGroupName);
    connect(m_hardworker, &HardWorker::sigChangeGroupNameDone,
            m_page_groupInfo, &GroupInfoPage::handlerChangeGroupNameIsDone);
    connect(m_hardworker, &HardWorker::sigChangeGroupNameDone,
            this, &KiranGroupManager::updateSidebarItem);

    connect(m_page_groupInfo, &GroupInfoPage::sigAddUserToGroup,
            m_hardworker, &HardWorker::doAddUserToGroup);
    connect(m_hardworker, &HardWorker::sigAddUserToGroupDone,
            m_page_groupInfo, &GroupInfoPage::handlerAddUserToGroupIsDone);
}

void KiranGroupManager::setDefaultSiderbarItem()
{
    m_tabList->setCurrentRow(0);
}

QSize KiranGroupManager::sizeHint() const
{
    return {780, 657};
}
