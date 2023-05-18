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
#include "auth-plugin.h"
#include "auth-subitem.h"
#include "config.h"

#include <kiran-log/qt5-log-i.h>
#include <QCoreApplication>
#include <QTranslator>

#include "pages/driver/driver-page.h"
#include "pages/face/face-page.h"
#include "pages/finger/finger-page.h"
#include "pages/iris/iris-page.h"
#include "pages/prefs/prefs-page.h"
#include "pages/ukey/ukey-page.h"

#include "utils/kiran-auth-dbus-proxy.h"

AuthPlugin::AuthPlugin(QObject* parent)
    : QObject(parent)
{
}

AuthPlugin ::~AuthPlugin()
{
}

int AuthPlugin::init(KiranControlPanel::PanelInterface* interface)
{
    m_panelInterface = interface;
    m_translator = new QTranslator;

    if (m_translator->load(QLocale(),
                           "kiran-cpanel-authentication",
                           ".",
                           TRANSLATE_PREFIX,
                           ".qm"))
    {
        qApp->installTranslator(m_translator);
    }
    else
    {
        m_translator->deleteLater();
        m_translator = nullptr;
        KLOG_ERROR() << "load translator failed!";
    }

    KiranAuthDBusProxy::globalInit();

    initItems();
    return 0;
}

void AuthPlugin::uninit()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        m_translator->deleteLater();
        m_translator = nullptr;
    }

    KiranAuthDBusProxy::globalDeinit();
}

QVector<KiranControlPanel::SubItemPtr> AuthPlugin::getSubItems()
{
    return m_subitems;
}

void AuthPlugin::initItems()
{
    auto FingerprintPageCreator = []() -> QWidget*
    {
        return new FingerPage(KiranAuthDBusProxy::getInstance(), FingerPage::FINGER_TYPE_FINGER_PRINT);
    };
    auto FingerveinPageCreator = []() -> QWidget*
    {
        return new FingerPage(KiranAuthDBusProxy::getInstance(), FingerPage::FINGER_TYPE_FINGER_VEIN);
    };
    auto UKeyPageCreator = []() -> QWidget*
    {
        return new UKeyPage(KiranAuthDBusProxy::getInstance());
    };
    auto IrisPageCreator = []() -> QWidget*
    {
        return new IrisPage(KiranAuthDBusProxy::getInstance());
    };
    auto FacePageCreator = []() -> QWidget*
    {
        return new FacePage(KiranAuthDBusProxy::getInstance());
    };
    auto DriverPageCreator = []() -> QWidget*
    {
        return new DriverPage(KiranAuthDBusProxy::getInstance());
    };
    auto PrefsPageCreator = []() -> QWidget*
    {
        return new PrefsPage(KiranAuthDBusProxy::getInstance());
    };

    struct SubItemStruct
    {
        QString id;
        QString name;
        QString category;
        QString desc;
        QString icon;
        int weight;
        std::function<QWidget*(void)> func;
    };

    QList<SubItemStruct> subitemInfos = {
        {"Fingerprint",
         tr("Fingerprint"),
         "authentication-manager",
         "",
         "kcp-authentication-fingerprint",
         99,
         FingerprintPageCreator},
        {"FingerVein",
         tr("FingerVein"),
         "authentication-manager",
         "",
         "kcp-authentication-fingervein",
         98,
         FingerveinPageCreator},
        {"UKey",
         tr("UKey"),
         "authentication-manager",
         "",
         "kcp-authentication-ukey",
         98,
         UKeyPageCreator},
        {"Iris",
         tr("Iris"),
         "authentication-manager",
         "",
         "kcp-authentication-iris",
         98,
         IrisPageCreator},
        {"Face",
         tr("Face"),
         "authentication-manager",
         "",
         "kcp-authentication-face",
         98,
         FacePageCreator},
        {"Driver",
         tr("Driver Manager"),
         "authentication-manager",
         "",
         "kcp-authentication-driver",
         97,
         DriverPageCreator},
        {"Prefs",
         tr("Prefs"),
         "authentication-manager",
         "",
         "kcp-authentication-prefs",
         96,
         PrefsPageCreator},
    };

    for (const SubItemStruct& subitemInfo : subitemInfos)
    {
        auto subitem = new AuthSubItem(subitemInfo.func);
        subitem->setID(subitemInfo.id);
        subitem->setName(subitemInfo.name);
        subitem->setCategory(subitemInfo.category);
        subitem->setDesc(subitemInfo.desc);
        subitem->setIcon(subitemInfo.icon);
        subitem->setWeight(subitemInfo.weight);
        m_subitems.append(KiranControlPanel::SubItemPtr(subitem));
    }
}