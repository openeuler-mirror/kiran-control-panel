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

#include <QString>
#include <QIcon>
#include <KDesktopFile>
#include <KConfigGroup>
#include <QFileInfo>
#include <qt5-log-i.h>
#include "autostart-app.h"

#define DESKTOP_ENTRY_HIDDEN "Hidden"
#define DESKTOP_ENTRY_NODISPLAY "NoDisplay"
#define DESKTOP_ENTRY_ONLYSHOWIN "OnlyShowIn"
#define DESKTOP_ENTRY_NOTSHOWIN "NotShowIn"

AutostartApp::AutostartApp(QString autoStartAppFilePath) : m_filePath(autoStartAppFilePath)
{
    parseAutoStartApp(autoStartAppFilePath);
}

AutostartApp::~AutostartApp()
{

}


void AutostartApp::parseAutoStartApp(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    KDesktopFile kDesktopFile(filePath);

    QString iconName, onlyShowIn, notShowIn;

    // 读取变量
    KConfigGroup desktopGroup = kDesktopFile.desktopGroup();
    m_desktopName = fileInfo.fileName();
    m_name = kDesktopFile.readName();
    m_hidden = desktopGroup.readEntry(DESKTOP_ENTRY_HIDDEN, false);
    m_noDisplay = desktopGroup.readEntry(DESKTOP_ENTRY_NODISPLAY, false);
    onlyShowIn = desktopGroup.readEntry(DESKTOP_ENTRY_ONLYSHOWIN);
    notShowIn = desktopGroup.readEntry(DESKTOP_ENTRY_NOTSHOWIN);
    iconName = kDesktopFile.readIcon();

    bool mshow = true;
    if (!onlyShowIn.isEmpty() && !onlyShowIn.contains("KIRAN"))
    {
        mshow = false;
    }
    if (!notShowIn.isEmpty() && notShowIn.contains("KIRAN"))
    {
        mshow = false;
    }

    m_shown = mshow;

    if (!iconName.isEmpty())
    {
        m_icon = QIcon::fromTheme(iconName, QIcon(QString(":/kcp-application/images/desktop-file.svg")));
    }
    else
    {
        m_icon = QIcon(QString(":/kcp-application/images/desktop-file.svg"));
    }

    if (m_name.isEmpty())
    {
        KLOG_WARNING() << filePath << " don't have the key [Name]";
    }

    return;
}


void AutostartApp::setAutoStartAppStatus(bool status)
{
    KDesktopFile desktopFile(m_filePath);
    KConfigGroup desktopGroup = desktopFile.desktopGroup();

    // 重置Hidden字段
    desktopGroup.writeEntry("Hidden", !status);
    desktopGroup.sync();

    m_hidden = !status;

    return;
}

QString AutostartApp::getDesktopName()
{
    return m_desktopName;
}

QString AutostartApp::getName()
{
    return m_name;
}

bool AutostartApp::getShown()
{
    return m_shown;
}

bool AutostartApp::getHidden()
{
    return m_hidden;
}

bool AutostartApp::getNoDisplay()
{
    return m_noDisplay;
}

QIcon AutostartApp::getIcon()
{
    return m_icon;
}

QString AutostartApp::getFilePath()
{
    return m_filePath;
}