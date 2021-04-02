#include "select-avatar-page.h"
#include "avatar-item-button.h"
#include "config.h"
#include "flowlayout.h"
#include "scrollarea-container.h"
#include "tools/avatar-editor-wrapper.h"

#include <widget-property-helper.h>
#include <QButtonGroup>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#define SYSTEM_AVATAR_OBJ_NAME "avatar_button_system"
#define USER_AVATAR_OBJ_NAME "avatar_button_user"
#define ADD_AVATAR_OBJ_NAME "avatar_button_add"
#define CUSTOM_AVATAR_OBJ_NAME "avatar_button_custom"

SelectAvatarPage::SelectAvatarPage(QWidget *parent) : QWidget(parent),
                                                      m_mode(CHANGE_AVATAR_FOR_USER)
{
    initUI();
}

SelectAvatarPage::~SelectAvatarPage() = default;

QString SelectAvatarPage::currentSelectAvatar() const
{
    auto button = qobject_cast<AvatarItemButton *>(m_buttonGroup->checkedButton());
    if (button == nullptr)
    {
        return QString();
    }
    else
    {
        return button->iconPath();
    }
}

void SelectAvatarPage::setMode(SelectAvatarPage::SelectAvatarMode mode)
{
    m_mode = mode;
}

SelectAvatarPage::SelectAvatarMode SelectAvatarPage::mode() const
{
    return m_mode;
}

void SelectAvatarPage::setCurrentAvatar(const QString &iconPath)
{
    QList<AvatarItemButton *> buttons       = m_scrollArea->findChildren<AvatarItemButton *>(SYSTEM_AVATAR_OBJ_NAME);
    AvatarItemButton *        currentAvatar = nullptr;

    //删除之前用户添加头像
    removeUserAvatar();

    //是否是系统头像
    for (AvatarItemButton *button : buttons)
    {
        if (button->iconPath() == iconPath)
        {
            currentAvatar = button;
            break;
        }
    }

    //当前头像路径未匹配上系统头像路径,添加新的头像
    if (currentAvatar != nullptr)
    {
        currentAvatar->setChecked(true);
    }
    else
    {
        QPixmap pixmap(iconPath);
        if (!pixmap.isNull())
        {
            addAvatar(iconPath, AVATAR_USER, true);
        }
        else
        {
            qWarning() << "load" << iconPath << "failed";
        }
    }

    //保证添加按钮在最后
    moveAddButtonToEnd();
}

void SelectAvatarPage::initUI()
{
    QPushButton *btn  = nullptr;
    QLayoutItem *item = nullptr;

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(24, 40, 24, 0);
    m_mainLayout->setSpacing(0);

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_mainLayout->addWidget(m_scrollArea);

    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);
    m_flowLayout = new FlowLayout(0, 24, 24);
    m_flowLayout->setMargin(0);
    ///NOTE:为什么直接使用QWidget不行?,为什么需要重写QWidget::sizeHint方法
    auto container = new ScrollAreaContainer(m_scrollArea);
    container->setLayout(m_flowLayout);
    m_scrollArea->setWidget(container);

    m_btnLayout = new QHBoxLayout;
    m_btnLayout->setSpacing(0);
    m_btnLayout->setMargin(0);
    m_btnLayout->setContentsMargins(0, 40, 0, 40);
    m_mainLayout->addItem(m_btnLayout);

    item = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_btnLayout->addItem(item);

    btn = new QPushButton(this);
    btn->setObjectName("btn_confirm");
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btn->setFixedSize(252, 60);
    btn->setText(tr("Confirm"));
    Kiran::WidgetPropertyHelper::setButtonType(btn, Kiran::BUTTON_Default);
    m_btnLayout->addWidget(btn);
    connect(btn, &QPushButton::clicked, [this]() {
        sigReturnToPrevPage(m_mode, true);
    });

    item = new QSpacerItem(76, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    m_btnLayout->addItem(item);

    btn = new QPushButton(this);
    btn->setObjectName("btn_return");
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btn->setFixedSize(252, 60);
    btn->setText(tr("Return"));
    m_btnLayout->addWidget(btn);
    connect(btn, &QPushButton::clicked, [this]() {
        sigReturnToPrevPage(m_mode, false);
    });

    item = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_btnLayout->addItem(item);

    loadAvatar();
    m_addButton = addAvatar(":/images/add_icon.png", AVATAR_ADD, false);
    connect(m_addButton, &AvatarItemButton::clicked, [this]() {
        //1.选择图片
        QString fileName = QFileDialog::getOpenFileName(this, tr("select picture"),
                                                        QDir::homePath(),
                                                        tr("image files(*.bmp *.jpg *.png *.tif *.gif"
                                                           " *.pcx *.tga *.exif *.fpx *.svg *.psd *.cdr *.pcd"
                                                           " *.dxf *.ufo *.eps *.ai *.raw *.WMF *.webp)"));
        if (fileName.isEmpty())
        {
            return;
        }

        //2.弹出头像编辑
        QString dstImagePath;
        if (AvatarEditorWrapper::exec(fileName, dstImagePath))
        {
            addAvatar(dstImagePath, AVATAR_CUSTOM, false);
            moveAddButtonToEnd();
        }
    });
}

//加载/usr/share/kiran-account-manager/account-icons/下*.face显示
void SelectAvatarPage::loadAvatar()
{
    QDir          dir(BUILDIN_AVATAR_DIR_PATH);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files);
    for (auto &iter : fileInfoList)
    {
        addAvatar(iter.absoluteFilePath(), AVATAR_SYSTEM, false);
    }
}

AvatarItemButton *SelectAvatarPage::addAvatar(const QString &iconPath, AvatarTypeEnum type, bool isChecked)
{
    AvatarItemButton *btn = new AvatarItemButton(m_scrollArea);
    switch (type)
    {
    case AVATAR_SYSTEM:
        btn->setObjectName(SYSTEM_AVATAR_OBJ_NAME);
        break;
    case AVATAR_USER:
        btn->setObjectName(USER_AVATAR_OBJ_NAME);
        break;
    case AVATAR_ADD:
        btn->setObjectName(ADD_AVATAR_OBJ_NAME);
        break;
    case AVATAR_CUSTOM:
        btn->setObjectName(CUSTOM_AVATAR_OBJ_NAME);
        break;
    }
    btn->setIcon(iconPath);
    btn->setFixedSize(80, 80);
    if (type != AVATAR_ADD)
    {
        btn->setCheckable(true);
        btn->setChecked(isChecked);
        m_buttonGroup->addButton(btn);
    }
    m_flowLayout->addWidget(btn);
    return btn;
}

void SelectAvatarPage::removeUserAvatar()
{
    //删除非系统头像
    QList<AvatarItemButton *> userAvatars = m_scrollArea->findChildren<AvatarItemButton *>(USER_AVATAR_OBJ_NAME);
    //删除用户添加的头像
    QList<AvatarItemButton *> customAvatars = m_scrollArea->findChildren<AvatarItemButton *>(CUSTOM_AVATAR_OBJ_NAME);

    QList<AvatarItemButton *> deleteAvatars;
    deleteAvatars.append(userAvatars);
    deleteAvatars.append(customAvatars);

    for (auto avatarButton : deleteAvatars)
    {
        m_flowLayout->removeWidget(avatarButton);
        avatarButton->deleteLater();
    }
}

void SelectAvatarPage::moveAddButtonToEnd()
{
    m_flowLayout->removeWidget(m_addButton);
    m_flowLayout->addWidget(m_addButton);
}
