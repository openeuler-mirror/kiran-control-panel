/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "account-plugin.h"
#include "accounts-global-info.h"
#include "kcp-account-config.h"
#include "temporary-dir-manager.h"

#include <qt5-log-i.h>
#include <QApplication>
#include <QLocale>

AccountPlugin::AccountPlugin(QObject* parent)
    : QObject(parent)
{
}

AccountPlugin ::~AccountPlugin()
{
}

int AccountPlugin::init(KiranControlPanel::PanelInterface* interface)
{
    m_panelInterface = interface;

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

    m_translator = new QTranslator(this);
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

    m_subitem.reset(new AccountSubItem(interface, this));
    return 0;
}

void AccountPlugin::uninit()
{
    if (m_translator != nullptr)
    {
        qApp->removeTranslator(m_translator);
        m_translator->deleteLater();
        m_translator = nullptr;
    }
}

QVector<KiranControlPanel::SubItemPtr> AccountPlugin::getSubItems()
{
    return {m_subitem};
}