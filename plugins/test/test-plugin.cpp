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

#include "test-plugin.h"
#include <QDebug>
#include <QRandomGenerator> 

int TestPlugin::init(KcpInterface* interface)
{
    m_controlPanelInterface = interface;

    for (int i = 0; i < 4; i++)
    {
        QSharedPointer<TestSubItem> pointer(new TestSubItem());
        QString subitemID = QString("TestSubItem_%1").arg(i);

        pointer->setID(subitemID);
        pointer->setName(subitemID);
        pointer->setIcon("firefox");
        pointer->setWeight(i);
        pointer->setCategory(i % 2 == 0 ? "about-system" : "audio");

        m_subitems.append(pointer);
    }

    m_timer.setInterval(3000);
    m_timer.setSingleShot(false);
    m_timer.start();
    connect(&m_timer, &QTimer::timeout, this, &TestPlugin::handleTimerTimeout);
    return 0;
}

void TestPlugin::uninit()
{
}

QVector<KiranControlPanel::SubItemPtr> TestPlugin::getSubItems()
{
    return m_subitems;
}

#include <QDateTime>
void TestPlugin::handleTimerTimeout()
{
    for (int i = 0; i < 1; i++)
    {
        QSharedPointer<TestSubItem> pointer(new TestSubItem());
// sonarqube block off
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        int rand = QRandomGenerator::global()->generate();
#else
        int rand = qrand();
#endif
// sonarqube block on    
        QString subitemID = QString("%1_%2").arg(QDateTime::currentDateTime().toString()).arg(rand);
        pointer->setID(subitemID);
        pointer->setName(subitemID);
        pointer->setIcon("firefox");
        pointer->setWeight(10);
        pointer->setCategory(rand % 2 ? "about-system" : "audio");
        m_subitems.append(pointer);
    }
    m_controlPanelInterface->handlePluginSubItemChanged();
}