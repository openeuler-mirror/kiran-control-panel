#include "mimetype-apppage.h"
#include <qt5-log-i.h>
#include <QBoxLayout>
#include <QListWidget>
#include "defaultapp-frame.h"
#include "kiran-setting-container.h"

MimeTypeAppPage::MimeTypeAppPage(EnumMimeType enumMimeType, QWidget* parent)
    : m_enumMimeType(enumMimeType), QWidget(parent), m_frame(new DefaultAppFrame)
{
    initUI();
}

MimeTypeAppPage::~MimeTypeAppPage()
{
    delete m_frame;
}

void MimeTypeAppPage::initUI()
{
    auto authTypeWidget = new QWidget(this);

    auto authTypeLayout = new QBoxLayout(QBoxLayout::TopToBottom, authTypeWidget);
    authTypeLayout->setSpacing(8);
    authTypeLayout->setContentsMargins(10, 0, 10, 0);

    authTypeLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto container = new SettingsContainer(this);
    authTypeLayout->addWidget(container);

    authTypeLayout->addStretch();

    QVector<XdgDesktopFile*> appInfos = m_frame->getApplications(m_enumMimeType);
    for (int index = 0; index < appInfos.size(); index++)
    {
        // 获取desktop的中文名和图标
        XdgDesktopFile* deskInfo = appInfos[index];

        auto settingsItem = new KiranSettingItem(this);
        settingsItem->setClickable(true);
        settingsItem->setLeftButtonVisible(true, deskInfo->icon(), false);

        if (index == 0)
        {
            settingsItem->setRightButtonVisible(true, QIcon(":/kcp-defaultapp-images/chosen.png"), false);
            m_beforeItem = settingsItem;
        }
        else
        {
            settingsItem->setRightButtonVisible(false, QIcon(":/kcp-defaultapp-images/chosen.png"), false);
        }
        settingsItem->setText(deskInfo->name());
        container->addKiranSettingItem(settingsItem);

        auto clickedSlot = std::bind(&MimeTypeAppPage::onClickedSlot, this, m_enumMimeType, deskInfo);
        connect(settingsItem, &KiranSettingItem::clicked, this, clickedSlot);

        m_mimeApp[QFileInfo(deskInfo->fileName()).fileName()] = settingsItem;
    }

}

void MimeTypeAppPage::onClickedSlot(EnumMimeType enumMimeType, XdgDesktopFile* desktopFile)
{
    // 切换选项
    m_beforeItem->setRightButtonVisible(false, QIcon(":/kcp-defaultapp-images/chosen.png"), false);

    for (auto iter = m_mimeApp.begin(); iter != m_mimeApp.end(); iter++)
    {
        auto appName = iter.key();
        auto settingsItem = iter.value();
        if (QFileInfo(desktopFile->fileName()).fileName() == appName)
        {
            m_beforeItem = settingsItem;
            settingsItem->setRightButtonVisible(true, QIcon(":/kcp-defaultapp-images/chosen.png"), false);
            m_frame->slotTextChanged(enumMimeType, desktopFile);
            break;
        }
    }
}
