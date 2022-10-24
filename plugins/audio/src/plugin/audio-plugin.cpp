/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#include "audio-plugin.h"
#include "config.h"
#include "input-page.h"
#include "output-page.h"

#include <qt5-log-i.h>
#include <QCoreApplication>
#include <QFile>

AudioPlugin::AudioPlugin()
{
}

AudioPlugin::~AudioPlugin()
{
}

int AudioPlugin::init()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator(qApp);
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-audio",
                            ".",
                            TRANSLATION_DIR_INSTALL_PATH,
                            ".qm"))
    {
        KLOG_ERROR() << "can't load translator";
        delete m_translator;
        m_translator = nullptr;
    }
    else
    {
        qApp->installTranslator(m_translator);
    }

    return 0;
}

void AudioPlugin::uninit()
{
}

QWidget* AudioPlugin::getSubItemWidget(QString subItemName)
{
    QWidget* widget = nullptr;
    if (subItemName == "OutputPage")
    {
        widget = new OutputPage;
    }
    if (subItemName == "InputPage")
    {
        widget = new InputPage;
    }
    return widget;
}

bool AudioPlugin::haveUnsavedOptions()
{
    return false;
}

QStringList AudioPlugin::visibleSubItems()
{
    return QStringList() << "OutputPage"
                         << "InputPage";
}
