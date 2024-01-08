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


#ifndef __LANGUAGE_PAGE_H__
#define __LANGUAGE_PAGE_H__

#include <QWidget>

class LanguageManager;
class QPushButton;
class SettingsContainer;
class KiranSettingItem;
class LangInfo;
class LanguagePage : public QWidget
{
    Q_OBJECT

public:
    explicit LanguagePage(QWidget* parent = nullptr);
    ~LanguagePage();

private:
    void initUI();
    void initAddBtn();
    void initConnect();
    void loadLangaugeItem();
    void initLanguageSelectDialogUI();
    QString showLanguageName(QString languageCode);
    KiranSettingItem* createLanguageItem(const LangInfo& langInfo);

private slots:
    void toggleLanguageItem();
    void deleteLanguageItem(KiranSettingItem* languageItem);
    void handleLanguageChanged();

private:
    LanguageManager* m_languageManager = nullptr;

    SettingsContainer* m_languageContainer = nullptr;
    QPushButton* m_languageButton = nullptr;
    KiranSettingItem* m_languageItem = nullptr;
};

#endif