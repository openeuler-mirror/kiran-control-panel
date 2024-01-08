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

#ifndef __LANGUAGE_MANAGER_H__
#define __LANGUAGE_MANAGER_H__

#include <QWidget>
#include <QtDBus>
#include "language-select-dialog.h"

struct LangInfo{
    QString locale;
    QString desc;
    QString langCode;
    QString languageName;
    LangInfo() :locale(), desc(), langCode(),languageName(){}
    bool operator==(const LangInfo& other) const
    {
        if (locale == other.locale && desc == other.desc && langCode == other.langCode && languageName == other.languageName)
            return true;
        else
            return false;
    }
};

class QGSettings;
class LangpackInstaller;
class LanguageManager : public QWidget
{
    Q_OBJECT

public:
    explicit LanguageManager(QWidget* parent = nullptr);
    ~LanguageManager();


public:
    inline QString getCurrentLanguage()
    {
        return m_currentLanguage;
    }

    inline QList<LangInfo> getShowLangugaeList()
    {
        return m_showLanguageList;
    }

    inline QList<LangInfo> getLanguageList()
    {
        return m_languageList;
    }
    inline LanguageSelectDialog* languageDailog()
    {
        return m_languageDialog;
    }
    void setCurrentLanguage(const QString& locale);
    void deleteShowLanguage(const QString& locale);
    bool isSupportedLanguage(const QString& locale);
    void initConnect();
    void initLanguageDBus();
    QList<LangInfo> parseSupportLang(const QString& langs);

private:
    void initGSettings();
    // 展示到主界面的语言列表
    void showLangugaeList();
    // 支持的所有语言列表
    void languageList();
    // 当前系统语言
    void currentLanguage();

signals:
    void showLanguageChanged();

private slots:
    void addShowLanguage(QString locale);

private:
    QString m_currentLanguage;
    QList<LangInfo> m_showLanguageList;
    QList<LangInfo> m_languageList;
    QGSettings* m_languageSettings = nullptr;
    LangpackInstaller* m_langpackInstaller = nullptr;
    LanguageSelectDialog* m_languageDialog = nullptr;
    QDBusInterface* m_languageDBus = nullptr;
};

#endif