/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-account is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
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
