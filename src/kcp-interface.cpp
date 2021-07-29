 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd. 
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */
 
#include "kcp-interface.h"
#include "kiran-account-manager.h"
#include "accounts-global-info.h"
#include "temporary-dir-manager.h"
#include "config.h"

#include <QStyle>
#include <qt5-log-i.h>

#define DEFAULT_THEME ":/kcp-account-themes/black_theme.qss"

int KcpInterface::init()
{
    if (!AccountsGlobalInfo::instance()->init())
    {
        KLOG_ERROR() << "load user info failed!";
        return -1;
    }
    if (!TemporaryDirManager::instance()->init(qAppName()))
    {
        KLOG_ERROR() << "init temporary dir manager failed!";
        return -1;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-account",
                            ".",
                            TRANSLATION_DIR_PATH,
                            ".qm"))
    {
        m_translator->deleteLater();
        m_translator = nullptr;
        KLOG_ERROR() << "load translator failed!";
    }
    else
    {
        qApp->installTranslator(m_translator);
    }

    return 0;
}

void KcpInterface::uninit()
{

}

QWidget* KcpInterface::getSubItemWidget(QString subItemName)
{
    QWidget* widget = nullptr;
    if(subItemName == "Account")
    {
        widget = new KiranAccountManager;
        QFile file(DEFAULT_THEME);
        if (file.open(QIODevice::ReadOnly))
        {
            QString stylesheet;
            stylesheet = file.readAll();
            file.close();
            widget->setStyleSheet(widget->styleSheet()+stylesheet);
        }
        else
        {
            qWarning() << "can't load stylesheet";
        }
    }
    m_currentWidget = widget;
    return m_currentWidget;
}

bool KcpInterface::haveUnsavedOptions()
{
    return false;
}

QStringList KcpInterface::visibleSubItems()
{
    return QStringList() << "Account";
}
