/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-display is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 */

#include "kcp-interface.h"

#include "config.h"
#include "kiran-display-configuration.h"

#include <qt5-log-i.h>
#include <QApplication>
#include <QDBusInterface>
#include <QPushButton>
#include <QTranslator>

#define KCP_SUBITEM_DISPLAY_ID "Display"

KcpInterface::KcpInterface()
{
}

KcpInterface::~KcpInterface()
{
}

int KcpInterface::init()
{
    m_translator = new QTranslator;

    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-display",
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
    if (m_translator)
    {
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QWidget* KcpInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;

    if (id == KCP_SUBITEM_DISPLAY_ID)
    {
        widget = new KiranDisplayConfiguration();
    }

    return widget;
}

bool KcpInterface::haveUnsavedOptions()
{
    return false;
}

QStringList KcpInterface::visibleSubItems()
{
    return QStringList() << KCP_SUBITEM_DISPLAY_ID;
}