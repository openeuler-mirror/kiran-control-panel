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

#include <QGSettings>
#include <QFileInfo>
#include <QTextStream>
#include <QFile>
#include <QSignalBlocker>
#include <kiran-message-box.h>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <unistd.h>
#include <qt5-log-i.h>
#include "config.h"
#include "language-manager.h"
#include "langpack-installer.h"

#define KIRAN_LANGUAGE_SCHEMA_ID "com.kylinsec.kiran.language"
#define KEY_LANGUAGE_LIST "language-list"
#define LOCALE_CONF "/etc/locale.conf"
#define SUPPORT_LANGUAGE_FILE   "support_language.json"


LanguageManager::LanguageManager(QWidget* parent) : QWidget(parent), m_languageDialog(new LanguageSelectDialog(this)), m_langpackInstaller(new LangpackInstaller(this))
{
    m_languageDialog->setHidden(true);
    initLanguageDBus();
    initGSettings();
    languageList();
    currentLanguage();
    showLangugaeList();
    initConnect();
}

LanguageManager::~LanguageManager()
{
    if (m_languageSettings != nullptr)
    {
        delete m_languageSettings;
        m_languageSettings = nullptr;
    }

    if (m_languageDBus != nullptr)
    {
        delete m_languageDBus;
        m_languageDBus = nullptr;
    }
}

void LanguageManager::initLanguageDBus()
{
    if (!m_languageDBus)
    {
        QString objpath = QString("/org/freedesktop/locale1");
        m_languageDBus = new QDBusInterface("org.freedesktop.locale1",
                                              objpath,
                                              "org.freedesktop.locale1",
                                              QDBusConnection::systemBus());
    }
}

void LanguageManager::initGSettings()
{
    if (QGSettings::isSchemaInstalled(KIRAN_LANGUAGE_SCHEMA_ID))
    {
        m_languageSettings = new QGSettings(KIRAN_LANGUAGE_SCHEMA_ID, QByteArray(), this);
    }

    if (m_languageSettings)
    {
        connect(m_languageSettings, &QGSettings::changed, this, [=](const QString& key) {

            QStringList languageList = m_languageSettings->get(KEY_LANGUAGE_LIST).toStringList();
            if (!languageList.contains(m_currentLanguage))
            {
                languageList.append(m_currentLanguage);
            }

            QList<LangInfo> showLanguageList;
            for (const auto& locale : languageList)
            {
                for (const auto& langInfo : m_languageList)
                {
                    if (langInfo.locale == locale)
                    {
                        showLanguageList.append(langInfo);
                        break;
                    }
                }
            }

            m_showLanguageList = showLanguageList;
            emit showLanguageChanged();
        });
    }
}


void LanguageManager::initConnect()
{
    connect(m_languageDialog, &LanguageSelectDialog::languageAddBtnClicked, this, &LanguageManager::addShowLanguage);
}

void LanguageManager::currentLanguage()
{
    QString defaultLanguage = "en_US.UTF-8";
    if (!m_languageDBus)
    {
        m_currentLanguage = defaultLanguage;
        return;
    }

    QStringList currentLanguageList = m_languageDBus->property("Locale").toStringList();
    m_currentLanguage = currentLanguageList[0];
    if (m_currentLanguage.isEmpty() || !m_currentLanguage.contains("LANG="))
    {
        m_currentLanguage = defaultLanguage;
        return;
    }
    m_currentLanguage = m_currentLanguage.mid(m_currentLanguage.indexOf("=") + 1);
}

QList<LangInfo> LanguageManager::parseSupportLang(const QString& langs)
{
    QList<LangInfo> langInfoList;
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(langs.toUtf8().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        KLOG_ERROR() << " please check the string " << langs.toUtf8().data();
        return langInfoList;
    }

    if (jsonDocument.isObject())
    {
        QJsonObject obj = jsonDocument.object();
        if (obj.contains("SupportLanguage"))
        {
            QJsonValue langValue = obj.value("SupportLanguage");
            if (langValue.isArray())
            {
                QJsonArray array = langValue.toArray();
                for (int i = 0; i < array.size(); i++)
                {
                    QJsonValue value = array.at(i);
                    if (value.type() == QJsonValue::Object)
                    {
                        LangInfo info;
                        QJsonObject langObj = value.toObject();
                        if (langObj.contains("Locale"))
                        {
                            info.locale = langObj.value("Locale").toString();
                        }
                        if (langObj.contains("Description"))
                        {
                            info.desc = langObj.value("Description").toString();
                        }
                        if (langObj.contains("LangCode"))
                        {
                            info.langCode = langObj.value("LangCode").toString();
                        }
                        if (langObj.contains("LanguageName"))
                        {
                            info.languageName = langObj.value("LanguageName").toString();
                        }
                        langInfoList.append(info);
                    }
                }
            }
        }
    }
    return langInfoList;
}


void LanguageManager::languageList()
{
    QList<LangInfo> langInfoList;
    QString supportLanguageFile = QString("%1%2").arg(KIRAN_SUPPORT_LANGUAGE_DIR).arg(SUPPORT_LANGUAGE_FILE);
    QString systemLanguageFile = QString("%1%2").arg(KIRAN_SYSTEM_SUPPORT_LANGUAGE_DIR).arg(supportLanguageFile);
    
    if (!QFileInfo::exists(supportLanguageFile) && !QFileInfo::exists(systemLanguageFile))
    {
        KLOG_WARNING() << SUPPORT_LANGUAGE_FILE << "isn't exists!";
        return;
    }

    QString languageFile = QFileInfo::exists(supportLanguageFile) ? supportLanguageFile : systemLanguageFile;
    QFile file(languageFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        KLOG_DEBUG() << SUPPORT_LANGUAGE_FILE << " can't open.";
        return;
    }

    QTextStream out(&file);
    langInfoList = parseSupportLang(out.readAll());
    m_languageList = langInfoList;
}

void LanguageManager::showLangugaeList()
{
    QList<LangInfo> showLanguageList;
    if (m_languageSettings)
    {
        QStringList languageList = m_languageSettings->get(KEY_LANGUAGE_LIST).toStringList();
        if (!languageList.contains(m_currentLanguage))
        {
            languageList.append(m_currentLanguage);
            QSignalBlocker blocker(m_languageSettings);
            m_languageSettings->set(KEY_LANGUAGE_LIST, languageList);
        }

        for (const auto& locale : languageList)
        {
            bool flag = false;
            for (const auto& langInfo : m_languageList)
            {
                if (langInfo.locale == locale)
                {
                    flag = true;
                    showLanguageList.append(langInfo);
                    break;
                }
            }
            
            // 若gsettings中的locale在在support_language.json中不存在，则报错
            if (!flag)
            {
                KiranMessageBox::message(nullptr,
                                 tr("Error"),
                                 tr("%1 inexistence in system").arg(locale),
                                 KiranMessageBox::Ok);
            }
        }
        
        m_showLanguageList = showLanguageList;
    }
}


bool LanguageManager::isSupportedLanguage(const QString& locale)
{
    for (const auto& info : m_languageList)
    {
       if(info.locale == locale)
           return true;
    }
    return false;
}

void LanguageManager::addShowLanguage(QString locale)
{
    if (m_languageSettings)
    {
        bool flag = false;
        for (const auto& info : m_showLanguageList)
        {
            if (info.locale == locale)
            {
                flag = true;
                break;
            }
        }

        if (!flag)
        {
            for (const auto& info : m_languageList)
            {
                if (info.locale == locale)
                {
                    m_showLanguageList.append(info);
                    break;
                }
            }

            QStringList languageList = m_languageSettings->get(KEY_LANGUAGE_LIST).toStringList();
            languageList.append(locale);
            m_languageSettings->set(KEY_LANGUAGE_LIST, languageList);
        }
    }
}

void LanguageManager::deleteShowLanguage(const QString& locale)
{
    if (m_languageSettings)
    {
        bool flag = false;
        LangInfo deleteLangInfo;
        for (const auto& info : m_showLanguageList)
        {
            if (info.locale == locale)
            {
                flag = true;
                deleteLangInfo = info;
                break;
            }
        }

        if (flag)
        {
            m_showLanguageList.removeAll(deleteLangInfo);

            QStringList languageList = m_languageSettings->get(KEY_LANGUAGE_LIST).toStringList();
            languageList.removeAll(locale);
            QSignalBlocker blocker(m_languageSettings);
            m_languageSettings->set(KEY_LANGUAGE_LIST, languageList);
        }
    }
}

void LanguageManager::setCurrentLanguage(const QString& locale)
{
    // 在support_language.json中是否存在
    if (!isSupportedLanguage(locale))
    {
        KiranMessageBox::message(nullptr,
                                 tr("set locale failed"),
                                 tr("%1 inexistence in system").arg(locale),
                                 KiranMessageBox::Ok);
        return;
    }

    // 在gsettings文件中是否存在，不存在则添加
    addShowLanguage(locale);

    // 调用LangpackInstaller接口检查langCode是否存在，不存在则下载
    QString langCode;
    for (const auto& info : m_showLanguageList)
    {
        if (info.locale == locale)
        {
            langCode = info.langCode;
        }
    }

    m_langpackInstaller = new LangpackInstaller;
    if (!m_langpackInstaller->checkLangpackInstalled(langCode))
    {
        m_langpackInstaller->installLangpack(langCode);
    }

    // 不论下载langCode是否成功，都将locale设置为系统语言
    if (!m_languageDBus)
    {
        return;
    }
    QStringList localeList;
    localeList << locale;
    m_languageDBus->call("SetLocale", localeList, true);
    m_currentLanguage = locale;
}