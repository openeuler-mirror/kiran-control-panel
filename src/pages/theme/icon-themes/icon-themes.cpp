/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "icon-themes.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/appearance-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QDir>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QVBoxLayout>
#include "../theme-widget-group.h"
#include "../theme-widget.h"
#include "dbus-interface/appearance-global-info.h"
#include "ui_icon-themes.h"

static QStringList icons{"accessories-calculator",
                         "preferences-system-notifications",
                         "web-browser",
                         "preferences-desktop-screensaver",
                         "utilities-terminal",
                         "user-info",
                         "preferences-desktop-wallpaper"};

IconThemes::IconThemes(QWidget *parent) : QWidget(parent),
                                          ui(new Ui::IconThemes)
{
    ui->setupUi(this);
}

IconThemes::~IconThemes()
{
    delete ui;
}

bool IconThemes::initUI()
{
    if (!getIconThemes(APPEARANCE_THEME_TYPE_ICON))
    {
        return false;
    }
    AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_ICON, m_currentIconTheme);
    createIconWidgets();

    return true;
}

void IconThemes::updateIconTheme(QString newIconTheme)
{
    QList<ThemeWidget *> widgets = m_iconThemeWidgetGroup->getThemeWidgetList();
    foreach (ThemeWidget *widget, widgets)
    {
        if (widget->getTheme() == newIconTheme)
        {
            widget->setSelectStatus(true, APPEARANCE_THEME_TYPE_ICON);
            emit sigSetIconTheme(true, newIconTheme);
        }
        else
        {
            widget->setSelectStatus(false, APPEARANCE_THEME_TYPE_ICON);
            emit sigSetIconTheme(false, newIconTheme);
        }
    }
    m_currentIconTheme = newIconTheme;
}

/**
 * @brief IconThemes::getIconThemes 获取图标主题信息，包括名字和路径
 * @param themeType 主题类型
 * @return true 成功
 *         false 失败
 */
bool IconThemes::getIconThemes(int themeType)
{
    QString iconThemesJson = nullptr;
    if (!AppearanceGlobalInfo::instance()->getAllThemes(themeType, iconThemesJson))
    {
        return false;
    }
    if (getJsonValueFromString(iconThemesJson) <= 0)
    {
        KLOG_ERROR() << "Can't convert json string or there is no icon themes!";
        return false;
    }
    return true;
}

int IconThemes::getJsonValueFromString(QString jsonString)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        KLOG_ERROR() << " please check the string " << jsonString.toLocal8Bit().data();
        return -1;
    }
    if (jsonDocument.isArray())
    {
        QJsonArray array = jsonDocument.array();
        int iconThemesNum = array.size();
        if (iconThemesNum < 1)
            return -1;
        for (int i = 0; i < iconThemesNum; i++)
        {
            QJsonValue value = array.at(i);
            if (value.type() == QJsonValue::Object)
            {
                QJsonObject themeInfoObj = value.toObject();
                if (themeInfoObj.contains("name"))
                {
                    QJsonValue themeValue = themeInfoObj.value("name");
                    if (themeValue.isString())
                    {
                        QString name = themeValue.toVariant().toString();
                        m_iconThemes.insert(i, name);
                    }
                }
                if (themeInfoObj.contains("path"))
                {
                    QJsonValue themeValue = themeInfoObj.value("path");
                    if (themeValue.isString())
                    {
                        QString path = themeValue.toVariant().toString();
                        m_iconThemesPath.insert(i, path);
                    }
                }
            }
        }
    }
    return m_iconThemes.size();
}

/**
 * @brief IconThemes::createIconWidgets 创建图标控件
 */
void IconThemes::createIconWidgets()
{
    m_iconThemeWidgetGroup = new ThemeWidgetGroup(this);
    QVBoxLayout *vLayout = new QVBoxLayout(ui->widget_icon);
    vLayout->setMargin(0);
    vLayout->setSpacing(4);
    for (int i = 0; i < m_iconThemes.size(); i++)
    {
        if (m_iconThemes.at(i).startsWith("Kiran", Qt::CaseInsensitive))
        {
            QString path = m_iconThemesPath.at(i) + "/apps/scalable/";
            QDir appsDir = QDir(path);
            QStringList iconList = appsDir.entryList(QDir::Files);
            QStringList showIconsList;
            if (appsDir.exists())
            {
                for (int i = 0; i < icons.size(); i++)
                {
                    if (iconList.contains(icons.at(i) + ".svg"))
                        showIconsList.append(path + icons.at(i) + ".svg");
                    else
                    {
                        KLOG_INFO() << "not contain " << icons.at(i);
                        foreach (QString icon, iconList)
                        {
                            if (icon.startsWith(icons.at(i)))
                            {
                                showIconsList.append(path + icon);
                                break;
                            }
                        }
                    }
                }
                if (!showIconsList.isEmpty())
                {
                    //new theme-widget
                    ThemeWidget *themeWidget = new ThemeWidget(QSize(48, 48), m_currentIconTheme,
                                                               m_iconThemes.at(i), showIconsList);
                    vLayout->addWidget(themeWidget, Qt::AlignRight);

                    if (m_iconThemes.at(i) == m_currentIconTheme)
                        m_iconThemeWidgetGroup->setCurrentWidget(themeWidget);
                    m_iconThemeWidgetGroup->addWidget(themeWidget);
                    themeWidget->setTheme(m_iconThemes.at(i));
                }
            }
            else
                continue;
        }
        else if (m_iconThemes.at(i).startsWith("Adwaita", Qt::CaseInsensitive))
        {
            QString path = m_iconThemesPath.at(i) + "/48x48/apps/";
            QDir appsDir = QDir(path);
            QStringList iconList = appsDir.entryList(QDir::Files);
            QStringList showIconsList;
            if (appsDir.exists())
            {
                for (int i = 0; i < icons.size(); i++)
                {
                    if (iconList.contains(icons.at(i) + ".png"))
                        showIconsList.append(path + icons.at(i) + ".png");
                    else
                    {
                        KLOG_INFO() << "not contain " << icons.at(i) << ".png";
                        foreach (QString icon, iconList)
                        {
                            if (icon.startsWith(icons.at(i)))
                            {
                                showIconsList.append(path + icon);
                                break;
                            }
                        }
                    }
                }
                //new theme-widget
                if (!showIconsList.isEmpty())
                {
                    ThemeWidget *themeWidget = new ThemeWidget(QSize(48, 48), m_currentIconTheme,
                                                               m_iconThemes.at(i), showIconsList);
                    vLayout->addWidget(themeWidget, Qt::AlignRight);

                    if (m_iconThemes.at(i) == m_currentIconTheme)
                        m_iconThemeWidgetGroup->setCurrentWidget(themeWidget);
                    m_iconThemeWidgetGroup->addWidget(themeWidget);
                    themeWidget->setTheme(m_iconThemes.at(i));
                }
            }
            else
                continue;
        }
    }
    connect(m_iconThemeWidgetGroup, &ThemeWidgetGroup::themeWidgetChange,
            [=](ThemeWidget *preWidget, ThemeWidget *currWidget) {
                if (currWidget->getTheme() == m_currentIconTheme)
                {
                    emit sigSetIconTheme(false, m_currentIconTheme);
                    return;
                }
                if (AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_ICON,
                                                               currWidget->getTheme()))
                {
                    KLOG_INFO() << "set icon theme successful";
                    emit sigSetIconTheme(true, currWidget->getTheme());
                }
                else
                {
                    KiranMessageBox::message(nullptr,
                                             tr("Faild"),
                                             tr("Set icon themes failed!"),
                                             KiranMessageBox::Ok);
                    emit sigSetIconTheme(false, m_currentIconTheme);
                }
            });
}
