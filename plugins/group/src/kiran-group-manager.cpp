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
#include "add-users-page/add-users-page.h"
#include "create-group-page/create-group-page.h"
#include "def.h"
#include "group-info-page/group-info-page.h"
#include "groups-global-info.h"
#include "hard-worker.h"
#include "kiran-color-block.h"

#include <kiran-sidebar-item.h>
#include <kiran-sidebar-widget.h>
#include <qt5-log-i.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
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

    m_pageGroupInfo = new GroupInfoPage(m_stackWidget);
    m_stackWidget->insertWidget(PAGE_GROUP_INFO, m_pageGroupInfo);
    initPageGroupInfo();

    m_pageAddUsers = new AddUsersPage(m_stackWidget);
    m_stackWidget->insertWidget(PAGE_ADD_USERS, m_pageAddUsers);
    initPageAddUsers();

    m_stackWidget->setCurrentIndex(PAGE_CREATE_GROUP);

    // FIXME:由于在调用添加、删除、修改用户组属性等接口时，dbus后台会发送多次信号，导致前端显示不正确，后续看是否需要监听dbus信号
    //  connectToInfoChange();

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
                }
                else
                {
                    QString groupObjPath = item->data(ITEM_GROUP_OBJ_PATH_ROLE).toString();
                    // 更新用户组信息页面
                    m_pageGroupInfo->setCurrentShowGroupPath(groupObjPath);
                    //切换到用户组信息
                    m_stackWidget->setCurrentIndex(PAGE_GROUP_INFO);
                } });

    /// 创建用户组按钮
    m_createGroupItem = new QListWidgetItem(tr("Create new group"), m_tabList);
    m_createGroupItem->setIcon(QIcon::fromTheme("krsvg-create-group-icon"));
    m_tabList->addItem(m_createGroupItem);

    QList<QString> groupObjList;
    groupObjList = GroupsGlobalInfo::instance()->getGroupList();
    for (auto &iter : groupObjList)
    {
        KSDGroupAdminListProxy interface(GROUP_ADMIN_DBUS_NAME, iter, QDBusConnection::systemBus());
        if (isNoSystemGroup(interface.gid()))
        {
            auto item = new KiranSidebarItem(interface.groupName(), m_tabList);
            item->setIcon(QIcon::fromTheme("krsvg-group-icon"));
            item->setData(ITEM_GROUP_OBJ_PATH_ROLE, iter);
            m_tabList->addItem(item);
        }
    }
}

bool KiranGroupManager::isNoSystemGroup(qulonglong gid)
{
    return (gid >= 1000 && gid < 65534);
}

void KiranGroupManager::appendSidebarItem(const QString &groupPath)
{
    KLOG_INFO() << "current Thread:" << QThread::currentThreadId();

    KSDGroupAdminListProxy interface(GROUP_ADMIN_DBUS_NAME, groupPath, QDBusConnection::systemBus());
    if (isNoSystemGroup(interface.gid()))
    {
        auto item = new KiranSidebarItem(interface.groupName(), m_tabList);
        item->setIcon(QIcon::fromTheme("krsvg-group-icon"));
        item->setData(ITEM_GROUP_OBJ_PATH_ROLE, groupPath);
        m_tabList->addItem(item);
        m_tabList->setCurrentItem(item);

        // 更新用户组信息页面
        m_pageGroupInfo->setCurrentShowGroupPath(groupPath);
        // 切换到用户组信息
        m_stackWidget->setCurrentIndex(PAGE_GROUP_INFO);
    }
}

void KiranGroupManager::deleteSidebarItem(const QString &groupName)
{
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

            //  更新用户组信息页面
            m_pageGroupInfo->setCurrentShowGroupPath(groupPath);
            // 切换到用户组信息
            m_stackWidget->setCurrentIndex(PAGE_GROUP_INFO);
        }
    }
}

void KiranGroupManager::initPageCreateGroup()
{
    // 创建用户组
    connect(m_pageCreateGroup, &CreateGroupPage::requestCreateGroup,
            m_hardworker, &HardWorker::doCreateGroup);
    connect(m_hardworker, &HardWorker::sigCreateGroupDone,
            m_pageCreateGroup, &CreateGroupPage::onCreateGroupDone);
    connect(m_hardworker, &HardWorker::sigCreateGroupDone,
            this, &KiranGroupManager::appendSidebarItem);

    // 添加用户到用户组
    connect(m_pageCreateGroup, &CreateGroupPage::requestAddUserToGroup,
            m_hardworker, &HardWorker::doAddUserToGroup);
    connect(m_hardworker, &HardWorker::sigAddUserToGroupDone,
            m_pageCreateGroup, &CreateGroupPage::onAddUserToGroupDone);
}

void KiranGroupManager::initPageGroupInfo()
{
    connect(m_pageGroupInfo, &GroupInfoPage::requestAddUsersPage, [this](QString groupPath)
            { m_stackWidget->setCurrentIndex(PAGE_ADD_USERS);
            m_pageAddUsers->updateUsersList(groupPath); });

    // 从用户组移除用户
    connect(m_pageGroupInfo, &GroupInfoPage::requestRemoveMember,
            m_hardworker, &HardWorker::doRemoveMemberFromGroup);
    connect(m_hardworker, &HardWorker::sigRemoveMemberFromGroupDone,
            m_pageGroupInfo, &GroupInfoPage::onRemoveMemberFromGroupDone);

    // 添加用户到用户组
    connect(m_hardworker, &HardWorker::sigAddUserToGroupDone,
            m_pageGroupInfo, &GroupInfoPage::onAddUserToGroupDone);

    // 删除用户组
    connect(m_pageGroupInfo, &GroupInfoPage::requestDeleteGroup,
            m_hardworker, &HardWorker::doDeleteGroup);
    connect(m_hardworker, &HardWorker::sigDeleteGroupDone,
            m_pageGroupInfo, &GroupInfoPage::onDeleteGroupDone);
    connect(m_hardworker, &HardWorker::sigDeleteGroupDone,
            this, &KiranGroupManager::deleteSidebarItem);

    // 更改用户组名
    connect(m_pageGroupInfo, &GroupInfoPage::requestChangeGroupName,
            m_hardworker, &HardWorker::doChangeGroupName);
    connect(m_hardworker, &HardWorker::sigChangeGroupNameDone,
            m_pageGroupInfo, &GroupInfoPage::onChangeGroupNameDone);
    connect(m_hardworker, &HardWorker::sigChangeGroupNameDone,
            this, &KiranGroupManager::updateSidebarItem);
}

void KiranGroupManager::initPageAddUsers()
{
    connect(m_pageAddUsers, &AddUsersPage::requestGroupInfoPage, [this]()
            { m_stackWidget->setCurrentIndex(PAGE_GROUP_INFO); });

    // 添加用户到用户组
    connect(m_pageAddUsers, &AddUsersPage::requestAddUserToGroup,
            m_hardworker, &HardWorker::doAddUserToGroup);
    connect(m_hardworker, &HardWorker::sigAddUserToGroupDone,
            m_pageAddUsers, &AddUsersPage::onAddUserToGroupDone);
}

void KiranGroupManager::connectToInfoChange()
{
    connect(GroupsGlobalInfo::instance(), &GroupsGlobalInfo::GroupAdded, this, &KiranGroupManager::onGroupAdded);
    connect(GroupsGlobalInfo::instance(), &GroupsGlobalInfo::GroupDeleted, this, &KiranGroupManager::onGroupDeleted);
    connect(GroupsGlobalInfo::instance(), &GroupsGlobalInfo::GroupPropertyChanged, this, &KiranGroupManager::onGroupPropertyChanged);
}

void KiranGroupManager::setDefaultSiderbarItem()
{
    m_tabList->setCurrentRow(0);
}

QSize KiranGroupManager::sizeHint() const
{
    return {780, 657};
}

void KiranGroupManager::onGroupAdded(const QString &groupPath)
{
    KLOG_DEBUG() << "on group added, add group" << groupPath << "to sidebar";
}

void KiranGroupManager::onGroupDeleted(const QString &groupPath)
{
    KLOG_DEBUG() << "on group deleted, delete group" << groupPath << "from sidebar";
}

void KiranGroupManager::onGroupPropertyChanged(QString groupPath, QString propertyName, QVariant value)
{
    KLOG_DEBUG() << "on group property changed, changed property:" << groupPath << propertyName << value;
}
