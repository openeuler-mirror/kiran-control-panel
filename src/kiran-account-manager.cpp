#include "kiran-account-manager.h"
#include "account-itemwidget.h"
#include "accounts-global-info.h"
#include "create-user-page.h"
#include "user-info-page.h"
#include "select-avatar-page.h"
#include "listwidget-control.h"
#include "mask-widget.h"
#include "hard-worker.h"

#include <QIcon>
#include <QDebug>
#include <QtWidgets/QListWidgetItem>
#include <QHBoxLayout>
#include <QStackedWidget>

enum StackWidgetPageEnum {
    PAGE_CREATE_USER,
    PAGE_USER_INFO,
    PAGE_SELECT_AVATAR
};

KiranAccountManager::KiranAccountManager()
        : KiranTitlebarWindow(){
    m_workThread.start();
    m_hardworker = new HardWorker();
    m_hardworker->moveToThread(&m_workThread);
    initUI();
}

KiranAccountManager::~KiranAccountManager(){
    if( m_workThread.isRunning() ){
        m_workThread.quit();
        m_workThread.wait();
    }
    delete  m_hardworker;
};

void KiranAccountManager::setCurrentUser(const QString &userPath) {
    int findIdx = -1;
    for (int i = 0; i < m_tabList->count(); i++) {
        if (m_tabList->item(i)->data(Qt::UserRole) != userPath) {
            continue;
        }
        findIdx = i;
        break;
    }
    Q_ASSERT(findIdx != -1);
    m_tabList->setCurrentRow(findIdx);
}

void KiranAccountManager::appendSiderbarItem(const QString &userPath) {
    QListWidgetItem *item = nullptr;
    AccountItemWidget *itemWidget = nullptr;

    item = new QListWidgetItem(m_tabList);
    item->setSizeHint(QSize(286, 60));
    item->setData(Qt::UserRole, userPath);
    m_tabList->addItem(item);

    itemWidget = new AccountItemWidget(m_tabList);
    itemWidget->setUserDBusObjectPath(userPath);
    m_tabList->setItemWidget(item, itemWidget);
    connect(itemWidget, &AccountItemWidget::isSelectedChanged, [this, itemWidget](bool isSelected) {
        if (isSelected) {
            //更新用户信息页面
            m_page_userinfo->setCurrentShowUserPath(itemWidget->getUserDBusObjectPath());
            //切换到用户信息
            m_stackWidget->setCurrentIndex(PAGE_USER_INFO);
        }
    });
}

void KiranAccountManager::setDefaultSiderbarItem() {
    //设置默认侧边栏项
    if (m_tabList->count() > 1) {
        m_tabList->setCurrentRow(1);
    } else {
        m_tabList->setCurrentRow(0);
    }
}

void KiranAccountManager::initUI() {
    setObjectName("KiranAccountManager");
    setTitle(tr("User Manager"));
    setIcon(QIcon::fromTheme("user-admin"));

    //遮罩
    m_maskWidget = new MaskWidget(this);
    m_maskWidget->setVisible(false);

    //主布局
    auto contentLayout = new QHBoxLayout(getWindowContentWidget());
    contentLayout->setSpacing(0);
    contentLayout->setObjectName("AccountContentLayout");
    contentLayout->setContentsMargins(0, 0, 0, 0);

    //侧边栏
    auto siderbar = new QWidget(getWindowContentWidget());
    contentLayout->addWidget(siderbar);
    siderbar->setObjectName("siderWidget");
    siderbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    siderbar->setFixedWidth(323);

    auto vLayout = new QVBoxLayout(siderbar);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setObjectName("SiderbarVLayout");

    m_tabList = new ListWidgetControl(siderbar);
    m_tabList->setObjectName("tabList");
    m_tabList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tabList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabList->setUniformItemSizes(true);
    m_tabList->setSpacing(12);
    vLayout->addWidget(m_tabList);
    initUserList();

    ///内容区域
    m_stackWidget = new QStackedWidget(getWindowContentWidget());
    contentLayout->addWidget(m_stackWidget);
    m_stackWidget->setObjectName("StackWidget");

    m_page_createUser = new CreateUserPage(m_stackWidget);
    m_stackWidget->addWidget(m_page_createUser);
    initPageCreateUser();

    m_page_userinfo = new UserInfoPage(m_stackWidget);
    m_stackWidget->addWidget(m_page_userinfo);
    initPageUserInfo();

    m_page_selectAvatar = new SelectAvatarPage(m_stackWidget);
    m_stackWidget->addWidget(m_page_selectAvatar);
    initPageSelectAvatar();
    connectToInfoChanged();
    setDefaultSiderbarItem();

    resize(800, 600);
}

void KiranAccountManager::initUserList() {
    QListWidgetItem *item;
    AccountItemWidget *itemWidget;

    ///创建用户按钮
    item = new QListWidgetItem(m_tabList);
    item->setSizeHint(QSize(296, 60));
    m_tabList->addItem(item);

    itemWidget = new AccountItemWidget(m_tabList, true);
    m_tabList->setItemWidget(item, itemWidget);

    connect(itemWidget, &AccountItemWidget::isSelectedChanged, [this](bool isSelected) {
        if (isSelected) {
            //重置创建用户页面
            m_page_createUser->reset();
            //切换到创建用户
            m_stackWidget->setCurrentIndex(PAGE_CREATE_USER);
        }
    });

    //加载非系统用户
    QList<QString> userObjList;
    userObjList = AccountsGlobalInfo::instance()->getUserList();
    for (auto &iter : userObjList) {
        appendSiderbarItem(iter);
    }

    //QListWidget selectionChanged更新AccountItemWidget的Selected状态，更新样式
    connect(m_tabList, &QListWidget::itemSelectionChanged, [this]() {
        QList<QListWidgetItem *> selectedItems = m_tabList->selectedItems();
        if (selectedItems.empty()) {
            return;
        }
        QListWidgetItem *activatedItem = selectedItems.at(0);
        AccountItemWidget *currentItem = qobject_cast<AccountItemWidget *>(m_tabList->itemWidget(activatedItem));
        currentItem->setSelected(true);
        for (int i = 0; i < m_tabList->count(); i++) {
            QListWidgetItem *item = m_tabList->item(i);
            if (item != activatedItem) {
                AccountItemWidget *tabItem = dynamic_cast<AccountItemWidget *>(m_tabList->itemWidget(item));
                if (tabItem != nullptr)
                    tabItem->setSelected(false);
            }
        }
    });
}

void KiranAccountManager::initPageCreateUser() {
    //用户头像点击
    connect(m_page_createUser, &CreateUserPage::sigSetIconForNewUser, [this](QString iconPath) {
        //设置选择头像模式，为了缓存选择头像之前的页面，方便之后的返回
        m_page_selectAvatar->setMode(SelectAvatarPage::SELECT_AVATAR_FOR_NEW_USER);
        //设置头像，切换到选择头像页面
        m_page_selectAvatar->setCurrentAvatar(iconPath);
        m_stackWidget->setCurrentIndex(PAGE_SELECT_AVATAR);
    });
    connect(m_page_createUser, &CreateUserPage::sigRequestSetCurrentUser, [this](const QString &userPath) {
        //保证在设置当前行时,新用户已在侧边栏创建节点
        QTimer::singleShot(0, this, [=]() {
            int findIdx = -1;
            for (int i = 0; i < m_tabList->count(); i++) {
                if (m_tabList->item(i)->data(Qt::UserRole) != userPath) {
                    continue;
                }
                findIdx = i;
                break;
            }
            Q_ASSERT(findIdx != -1);
            m_tabList->setCurrentRow(findIdx);
        });
    });
    connect(m_page_createUser,&CreateUserPage::sigCreateUser,
            m_hardworker,&HardWorker::doCreateUser);
    connect(m_hardworker,&HardWorker::createUserisDnoe,
            m_page_createUser,&CreateUserPage::handlerCreateNewUserIsDone);
    //TODO:处理创建用户页面传出的繁忙信号，设置遮罩
}

void KiranAccountManager::initPageUserInfo() {
    //用户信息页面处理头像点击
    connect(m_page_userinfo, &UserInfoPage::sigUserChangeIcon, [this](const QString &iconPath) {
        m_page_selectAvatar->setMode(SelectAvatarPage::CHANGE_AVATAR_FOR_USER);
        m_page_selectAvatar->setCurrentAvatar(iconPath);
        m_stackWidget->setCurrentIndex(PAGE_SELECT_AVATAR);
    });
}

void KiranAccountManager::initPageSelectAvatar() {
    //选择头像页面处理返回
    connect(m_page_selectAvatar, &SelectAvatarPage::sigReturnToPrevPage,
            [this](SelectAvatarPage::SelectAvatarMode mode, bool isConfirm) {
                switch (mode) {
                    case SelectAvatarPage::SELECT_AVATAR_FOR_NEW_USER:
                        if (isConfirm) {
                            m_page_createUser->setAvatarIconPath(m_page_selectAvatar->currentSelectAvatar());
                        }
                        m_stackWidget->setCurrentIndex(PAGE_CREATE_USER);
                        break;
                    case SelectAvatarPage::CHANGE_AVATAR_FOR_USER:
                        if (isConfirm) {
                            m_page_userinfo->setAvatarIconPath(m_page_selectAvatar->currentSelectAvatar());
                        }
                        m_stackWidget->setCurrentIndex(PAGE_USER_INFO);
                        break;
                }
            });
}

void KiranAccountManager::connectToInfoChanged() {
    //处理用户新增、删除
    connect(AccountsGlobalInfo::instance(), &AccountsGlobalInfo::UserAdded,
            [this](const QDBusObjectPath &obj) {
                qInfo() << "siderbar add item:" << obj.path();
                appendSiderbarItem(obj.path());
            });

    connect(AccountsGlobalInfo::instance(), &AccountsGlobalInfo::UserDeleted,
            [this](const QDBusObjectPath &obj) {
                qInfo() << "siderbar delete item:" << obj.path();
                int findIdx = -1;
                bool setSiderbarItem = false;

                QString userPath = obj.path();
                for (int i = 0; i < m_tabList->count(); i++) {
                    if (m_tabList->item(i)->data(Qt::UserRole) == userPath) {
                        findIdx = i;
                        break;
                    }
                }
                if (findIdx == -1) {
                    return;
                }
                QList<QListWidgetItem *> selectedItems = m_tabList->selectedItems();
                if (selectedItems.size() != 0) {
                    QListWidgetItem *activatedItem = selectedItems.at(0);
                    if (activatedItem->data(Qt::UserRole) == userPath) {
                        setSiderbarItem = true;
                    }
                }
                QListWidgetItem *widgetItem = m_tabList->takeItem(findIdx);
                delete widgetItem;
                setDefaultSiderbarItem();
            });

    //处理用户属性变更
    connect(AccountsGlobalInfo::instance(), &AccountsGlobalInfo::UserPropertyChanged,
            [this](QString userPath, QString propertyName, QVariant value) {
                //侧边栏
                if ((propertyName == "locked") || (propertyName == "icon_file")) {
                    for (int i = 0; i < m_tabList->count(); i++) {
                        QListWidgetItem *item = m_tabList->item(i);
                        QString itemUserPath = item->data(Qt::UserRole).toString();
                        if (itemUserPath != userPath) {
                            continue;
                        }
                        AccountItemWidget *itemWidget = qobject_cast<AccountItemWidget *>(
                                m_tabList->itemWidget(item));
                        itemWidget->updateInfo();
                        break;
                    }
                }
                //用户详情页面
                QString currentUserPath = m_page_userinfo->getCurrentShowUserPath();
                if (userPath == currentUserPath) {
                    m_page_userinfo->updateInfo();
                }
            });
}

void KiranAccountManager::showMask() {
    this->stackUnder(m_maskWidget);
    m_maskWidget->show();
}

void KiranAccountManager::hideMask() {
    m_maskWidget->hide();
}
