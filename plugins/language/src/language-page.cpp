/**
 * Copyright (c) 2022 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <style-property.h>
#include <qt5-log-i.h>
#include "kiran-setting-container.h"
#include "kiran-setting-item.h"
#include "language-page.h"
#include "language-manager.h"

LanguagePage::LanguagePage(QWidget* parent) : QWidget(parent),
                                                m_languageManager(new LanguageManager(this))
{
    initUI();
    initLanguageSelectDialogUI();
    initConnect();
}

LanguagePage::~LanguagePage()
{
}

void LanguagePage::initConnect()
{
    connect(m_languageManager, &LanguageManager::showLanguageChanged, this, &LanguagePage::handleLanguageChanged);
}

// 加载每一项languageItem
void LanguagePage::loadLangaugeItem()
{
    QString currentLanguage = m_languageManager->getCurrentLanguage();
    QList<LangInfo> showLanguageList = m_languageManager->getShowLangugaeList();

    for (const auto& showLanguage : showLanguageList)
    {
        auto languageItem = createLanguageItem(showLanguage);
        
        if (currentLanguage == showLanguage.locale)
        {
            languageItem->setMidButtonVisible(true, QIcon(":/kiran-control-panel/images/indicator-selected.png"));
            m_languageItem = languageItem;
        }
        m_languageContainer->addItem(languageItem);
        connect(languageItem, &KiranSettingItem::clicked, this, &LanguagePage::toggleLanguageItem);

        auto deleteLanguageItemSlot = std::bind(&LanguagePage::deleteLanguageItem, this, languageItem);
        connect(languageItem, &KiranSettingItem::rightButtonClicked, this, deleteLanguageItemSlot);
    }
    initAddBtn();
}

void LanguagePage::initUI()
{
    auto mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(24, 14, 14, 14);
    mainLayout->setSpacing(0);

    auto label = new QLabel(tr("Language Select(Reboot to take effect)"), this);
    mainLayout->addWidget(label);

    auto languageWidget = new QWidget(this);
    mainLayout->addWidget(languageWidget);

    auto languageLayout = new QBoxLayout(QBoxLayout::TopToBottom, languageWidget);
    languageLayout->setSpacing(8);
    languageLayout->setContentsMargins(0, 0, 10, 0);

    languageLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_languageContainer = new SettingsContainer(this);
    languageLayout->addWidget(m_languageContainer);

    languageLayout->addStretch();

    loadLangaugeItem();
}

void LanguagePage::initAddBtn()
{
    m_languageButton = new QPushButton(this);
    Kiran::StylePropertyHelper::setButtonType(m_languageButton, Kiran::BUTTON_Default);
    m_languageButton->setIcon(QIcon(":/kcp-application/images/addition.svg"));
    m_languageContainer->addItem(m_languageButton);
    connect(m_languageButton, &QPushButton::clicked, this, [=]() {
        m_languageManager->languageDailog()->show();
    });
}

void LanguagePage::deleteLanguageItem(KiranSettingItem* languageItem)
{
    QString locale = languageItem->getUserData().toString();
    QString currentLocale = m_languageManager->getCurrentLanguage();
    if (languageItem == nullptr || locale == currentLocale)
    {
        KLOG_DEBUG() << "can't delete currentLanguageitem:" << locale;
        return;
    }

    m_languageContainer->removeItem(languageItem);
    m_languageManager->deleteShowLanguage(languageItem->getUserData().toString());
}

void LanguagePage::toggleLanguageItem()
{
    KiranSettingItem* languageItem = qobject_cast<KiranSettingItem*>(sender());
    QString currentLocale = languageItem->getUserData().toString();
    // 取消切换前的语言选中状态
    m_languageItem->setMidButtonVisible(false, QIcon(":/kiran-control-panel/images/indicator-selected.png"));
    QString beforeLocale = m_languageItem->getUserData().toString();
    // 添加切换后的语言选中状态
    m_languageItem = languageItem;
    languageItem->setMidButtonVisible(true, QIcon(":/kiran-control-panel/images/indicator-selected.png"));
    m_languageManager->setCurrentLanguage(currentLocale);
}

QString LanguagePage::showLanguageName(QString languageCode)
{
    QString languageName;
    if (languageCode == "zh_CN.UTF-8")
    {
        languageName = QString("简体中文") + QString("-%1").arg(tr("Simplified Chinese"));
    }
    else if (languageCode == "en_US.UTF-8")
    {
        languageName = QString("English") + QString("-%1").arg(tr("English"));
    }
    else if (languageCode == "bo_CN")
    {
        languageName = QString("বাংলা") + QString("-%1").arg(tr("Tibetan"));
    }
    else if (languageCode == "ky_KG")
    {
        languageName = QString("Кыргызча") + QString("-%1").arg(tr("Kirgiz"));
    }
    else if (languageCode == "mn_MN")
    {
        languageName = QString("Монгол") + QString("-%1").arg(tr("Mongolian"));
    }
    else if (languageCode == "kk_KZ")
    {
        languageName = QString("Қазақша") + QString("-%1").arg(tr("Kazakh"));
    }
    else if (languageCode == "ug_CN")
    {
        languageName = QString("Uyghurche") + QString("-%1").arg(tr("Uighur"));
    }
    else
    {
        languageName = QString("languageCode") + QString("-%1").arg("languageCode");
    }
    return languageName;
}

KiranSettingItem* LanguagePage::createLanguageItem(const LangInfo& langInfo)
{
    auto languageItem = new KiranSettingItem(this);
    languageItem->setClickable(true);
    languageItem->setLeftButtonVisible(false, QIcon());
    languageItem->setRightButtonVisible(true, QIcon(":/kiran-control-panel/images/trash.svg"));
    languageItem->setSwitcherVisible(false);
    languageItem->setMidButtonVisible(false, QIcon(":/kiran-control-panel/images/indicator-selected.png"));
    languageItem->setUserData(langInfo.locale);

    QString textLable = showLanguageName(langInfo.locale);
    languageItem->setText(textLable);

    return languageItem;
}

void LanguagePage::handleLanguageChanged()
{
    // 重新加载
    m_languageContainer->clear();
    loadLangaugeItem();
}


void LanguagePage::initLanguageSelectDialogUI()
{
    QList<LangInfo> languageList = m_languageManager->getLanguageList();
    for (const auto& langInfo : languageList)
    {
        m_languageManager->languageDailog()->addLanguageItem(showLanguageName(langInfo.locale), langInfo.locale);
    }
}