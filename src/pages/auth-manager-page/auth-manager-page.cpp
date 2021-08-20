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

 
#include "auth-manager-page.h"
#include "ksd_accounts_user_proxy.h"
#include "biometric-input-dialog/face-input-dialog/face-input-dialog.h"
#include "biometric-input-dialog/fingerprint-input-dialog/fingerprint-input-dialog.h"
#include "biometric-item.h"
#include "ui_auth-manager-page.h"

#include <kiran-message-box.h>
#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <widget-property-helper.h>
#include <QCheckBox>
#include <QJsonDocument>
#include <QList>

using namespace Kiran;

AuthManagerPage::AuthManagerPage(QWidget *parent) : QWidget(parent), ui(new Ui::AuthManagerPage)
{
    ui->setupUi(this);
    initUI();
}

AuthManagerPage::~AuthManagerPage()
{
    delete ui;
}

void AuthManagerPage::setCurrentUser(const QString &userObj)
{
    m_userObjPath = userObj;

    delete m_userProxy;
    m_userProxy = new KSDAccountsUserProxy(ACCOUNTS_DBUS_NAME,m_userObjPath,QDBusConnection::systemBus(),this);
    connect(m_userProxy, &KSDAccountsUserProxy::AuthItemChanged, this, &AuthManagerPage::slotUserAuthItemChanged);
    connect(m_userProxy, &KSDAccountsUserProxy::dbusPropertyChanged, this, &AuthManagerPage::slotUserPropertyChanged);

    auto funcBlockCheckBoxsSignal = [this](bool block) {
        QList<KiranSwitchButton *> authModeSwtichs = {m_fingerAuthSwitch, m_faceAuthSwitch, m_passwdAuthSwitch};
        foreach (auto authModeSwitch,authModeSwtichs)
        {
            authModeSwitch->blockSignals(block);
        }
    };

    ///NOTE:阻塞信号，忽略掉对至少存在一个认证模式的检查
    funcBlockCheckBoxsSignal(true);
    updateInfo();
    funcBlockCheckBoxsSignal(false);
}

void AuthManagerPage::initUI()
{
    WidgetPropertyHelper::setButtonType(ui->btn_save, BUTTON_Default);

    m_fingerAuthSwitch = new KiranSwitchButton(this);
    ui->layout_fingerAuth->addWidget(m_fingerAuthSwitch);
    connect(m_fingerAuthSwitch, &KiranSwitchButton::toggled, this, &AuthManagerPage::slotCheckAuthTypes, Qt::DirectConnection);

    m_faceAuthSwitch = new KiranSwitchButton(this);
    ui->layout_faceAuth->addWidget(m_faceAuthSwitch);
    connect(m_faceAuthSwitch, &KiranSwitchButton::toggled, this, &AuthManagerPage::slotCheckAuthTypes, Qt::DirectConnection);

    m_passwdAuthSwitch = new KiranSwitchButton(this);
    ui->layout_passwdAuth->addWidget(m_passwdAuthSwitch);
    connect(m_passwdAuthSwitch, &KiranSwitchButton::toggled, this, &AuthManagerPage::slotCheckAuthTypes, Qt::DirectConnection);

    connect(ui->btn_save, &QPushButton::clicked, [this]() {
        if (m_userProxy)
        {
            disconnect(m_userProxy, &KSDAccountsUserProxy::AuthItemChanged, this, &AuthManagerPage::slotUserAuthItemChanged);
            disconnect(m_userProxy, &KSDAccountsUserProxy::dbusPropertyChanged, this, &AuthManagerPage::slotUserPropertyChanged);
        }

        save();

        if (m_userProxy)
        {
            connect(m_userProxy, &KSDAccountsUserProxy::AuthItemChanged, this, &AuthManagerPage::slotUserAuthItemChanged);
            connect(m_userProxy, &KSDAccountsUserProxy::dbusPropertyChanged, this, &AuthManagerPage::slotUserPropertyChanged);
        }

        updateInfo();
    });
    connect(ui->btn_return, &QPushButton::clicked, [this]() {
        Q_EMIT sigReturn();
    });

#ifndef FACE_AUTH
    ui->widget_faceAuth->setVisible(false);
    ui->widget_faceTemplate->setVisible(false);
#endif
}

void AuthManagerPage::updateInfo()
{
    KLOG_INFO() << "load biometrics , update ui";
    //获取认证类型开关
    bool fingerAuth, faceAuth, passwdAuth;
    int authModes = m_userProxy->auth_modes();
    if (authModes == 0)
    {
        authModes = ACCOUNTS_AUTH_MODE_PASSWORD;
    }
    m_passwdAuthSwitch->setChecked(authModes & ACCOUNTS_AUTH_MODE_PASSWORD);
    m_fingerAuthSwitch->setChecked(authModes & ACCOUNTS_AUTH_MODE_FINGERPRINT);
    m_faceAuthSwitch->setChecked(authModes & ACCOUNTS_AUTH_MODE_FACE);

    //清空之前的生物特征值
    auto cleanLayoutFunc = [](QLayout *layout) {
        QLayoutItem *childItem = nullptr;
        while ((childItem = layout->takeAt(0)) != 0)
        {
            if (childItem->widget())
            {
                childItem->widget()->hide();
                childItem->widget()->setParent(nullptr);
                childItem->widget()->deleteLater();
            }
            delete childItem;
        }
    };
    cleanLayoutFunc(ui->layout_fingerTemplate);
    cleanLayoutFunc(ui->layout_faceTemplate);

    //获取已录入的生物特征值,填充进界面
    /* 添加指纹信息进界面 */
    BiometricItem *item = nullptr;
    BiometricList fingerAuthItemsList;
    fingerAuthItemsList = getBiometricItemsFromBackend(ACCOUNTS_AUTH_MODE_FINGERPRINT);
    for (auto &fingerTemplate : fingerAuthItemsList)
    {
        auto newItem = newBiometricItem(fingerTemplate.first, fingerTemplate.second);
        ui->layout_fingerTemplate->addWidget(newItem);
    }
    m_addFingerItem = new BiometricItem(tr("add fingerprint"), "", BiometricItem::BIOMETRIC_ITEM_ADD, this);
    m_addFingerItem->setItemAddEnabled(fingerAuthItemsList.isEmpty());
    ui->layout_fingerTemplate->addWidget(m_addFingerItem);
    connect(m_addFingerItem, &BiometricItem::sigAddBiometricItem, this, &AuthManagerPage::slotAddBiometricsItem);

    /* 添加人脸信息 */
    BiometricList faceAuthItemsList;
    faceAuthItemsList = getBiometricItemsFromBackend(ACCOUNTS_AUTH_MODE_FACE);
    for (auto &faceTemplate : faceAuthItemsList)
    {
        auto newItem = newBiometricItem(faceTemplate.first, faceTemplate.second);
        ui->layout_faceTemplate->addWidget(newItem);
    }
    m_addFaceItem = new BiometricItem(tr("add face"), "", BiometricItem::BIOMETRIC_ITEM_ADD, this);
    m_addFaceItem->setItemAddEnabled(faceAuthItemsList.isEmpty());
    ui->layout_faceTemplate->addWidget(m_addFaceItem);
    connect(m_addFaceItem, &BiometricItem::sigAddBiometricItem, this, &AuthManagerPage::slotAddBiometricsItem);
}

#include <unistd.h>
void AuthManagerPage::save()
{
    bool uiPasswdAuth, uiFingerAuth, uiFaceAuth;
    uiPasswdAuth = m_passwdAuthSwitch->isChecked();
    uiFingerAuth = m_fingerAuthSwitch->isChecked();
    uiFaceAuth = m_faceAuthSwitch->isChecked();
    /* 检查验证选项至少保证一个打开 */
    if (!uiPasswdAuth && !uiFingerAuth && !uiFaceAuth)
    {
        KiranMessageBox::message(this,
                                 tr("error"),
                                 tr("please ensure that at least one authentication option exists"),
                                 KiranMessageBox::Yes);
        return;
    }

    bool backendPasswdAuth, backendFingerAuth, backendFaceAuth;
    int authModes = m_userProxy->auth_modes();
    backendPasswdAuth = authModes & ACCOUNTS_AUTH_MODE_PASSWORD;
    backendFingerAuth = authModes & ACCOUNTS_AUTH_MODE_FINGERPRINT;
    backendFaceAuth = authModes & ACCOUNTS_AUTH_MODE_FACE;

    /* 保存验证选项 */
    //NOTE:先设置开的认证选项，不能把所有的认证方式关闭(所有的认证方式关闭时会带上密码认证选项)
    QMap<AccountsAuthMode, bool> authModeMap = {
        {ACCOUNTS_AUTH_MODE_PASSWORD, uiPasswdAuth},
        {ACCOUNTS_AUTH_MODE_FINGERPRINT, uiFingerAuth},
        {ACCOUNTS_AUTH_MODE_FACE, uiFaceAuth}};
    for (auto iterator = authModeMap.begin(); iterator != authModeMap.end(); ++iterator)
    {
        if (iterator.value())
        {
            auto reply = m_userProxy->EnableAuthMode(iterator.key(), iterator.value());
            reply.waitForFinished();
            if (reply.isError())
            {
                KLOG_ERROR() << "set auth mode [" << iterator.key() << "]"
                             << ":" << iterator.value() << ",failed!" << reply.error();
            }
        }
    }
    for (auto iterator = authModeMap.begin(); iterator != authModeMap.end(); ++iterator)
    {
        if (!iterator.value())
        {
            auto reply = m_userProxy->EnableAuthMode(iterator.key(), iterator.value());
            reply.waitForFinished();
            if (reply.isError())
            {
                KLOG_ERROR() << "set auth mode [" << iterator.key() << "]"
                             << ":" << iterator.value() << ",failed!" << reply.error();
            }
        }
    }

    /* 从界面上获取生物识别特征值 */
    BiometricList uiFingerTemplates, uiFaceTemplates;
    uiFingerTemplates = getBiometricItemsFromUI(ACCOUNTS_AUTH_MODE_FINGERPRINT);
    uiFaceTemplates = getBiometricItemsFromUI(ACCOUNTS_AUTH_MODE_FACE);

    /* 从后端获取生物识别特征值 */
    BiometricList backendFingerTemplates, backendFaceTemplates;
    backendFingerTemplates = getBiometricItemsFromBackend(ACCOUNTS_AUTH_MODE_FINGERPRINT);
    backendFaceTemplates = getBiometricItemsFromBackend(ACCOUNTS_AUTH_MODE_FACE);

    auto funcCompareBiometricItems = [](const BiometricList &newItems, const BiometricList &oldItems,
                                        BiometricList &deletedItems, BiometricList &addItems) {
        deletedItems.clear();
        addItems.clear();
        for (auto &item : newItems)
        {
            if (!oldItems.contains(item))
            {
                addItems.append(item);
            }
        }
        for (auto &item : oldItems)
        {
            if (!newItems.contains(item))
            {
                deletedItems.append(item);
            }
        }
    };

    BiometricList deletedItems, addItems;
    ///对比ui中存储的和后端存储的指纹生物特征差异，更新
    funcCompareBiometricItems(uiFingerTemplates, backendFingerTemplates, deletedItems, addItems);
    if (!deletedItems.isEmpty() || !addItems.isEmpty())
    {
        KLOG_DEBUG() << "finger biometric item will update:"
                     << "\n"
                     << "\tneed delete item:" << deletedItems << "\n"
                     << "\tneed add item:   " << addItems;
    }
    for (auto &item : deletedItems)
    {
        auto reply = m_userProxy->DelAuthItem(ACCOUNTS_AUTH_MODE_FINGERPRINT, item.first);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_ERROR() << "delete finger item" << item.first << "failed," << reply.error();
        }
    }
    for (auto &item : addItems)
    {
        auto reply = m_userProxy->AddAuthItem(ACCOUNTS_AUTH_MODE_FINGERPRINT, item.first, item.second);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_ERROR() << "add finger item" << item.first << "failed," << reply.error();
        }
    }

    ///对比ui中存储的和后端存储的人脸生物特征差异，更新
    funcCompareBiometricItems(uiFaceTemplates, backendFaceTemplates, deletedItems, addItems);
    if (!deletedItems.isEmpty() || !addItems.isEmpty())
    {
        KLOG_DEBUG() << "face biometric item update:"
                     << "\n"
                     << "\tneed delete item:" << deletedItems << "\n"
                     << "\tneed add item:   " << addItems;
    }
    for (auto &item : deletedItems)
    {
        auto reply = m_userProxy->DelAuthItem(ACCOUNTS_AUTH_MODE_FACE, item.first);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_ERROR() << "delete face item" << item.first << "failed," << reply.error();
        }
    }
    for (auto &item : addItems)
    {
        auto reply = m_userProxy->AddAuthItem(ACCOUNTS_AUTH_MODE_FACE, item.first, item.second);
        reply.waitForFinished();
        if (reply.isError())
        {
            KLOG_ERROR() << "add face item" << item.first << "failed," << reply.error();
        }
    }
}

/// @brief 每个认证开关关闭时验证是否存在其他认证方式，若不存在则重新打开开关
/// @param checked
void AuthManagerPage::slotCheckAuthTypes(bool checked)
{
    if (checked)
    {
        return;
    }

    if (m_faceAuthSwitch->isChecked() ||
        m_fingerAuthSwitch->isChecked() ||
        m_passwdAuthSwitch->isChecked())
    {
        return;
    }

    KiranMessageBox::message(this,
                             tr("error"),
                             tr("please ensure that at least one authentication option exists"),
                             KiranMessageBox::Yes);
    KiranSwitchButton *switchButton = qobject_cast<KiranSwitchButton *>(sender());
    switchButton->setChecked(true);
}

BiometricList AuthManagerPage::getBiometricItemsFromUI(AccountsAuthMode mode)
{
    //从布局中取出每项保存的数据
    auto funcGetBiometricItems = [](QLayout *layout, BiometricList &biometricItems) {
        for (int i = 0; i < layout->count(); i++)
        {
            auto item = layout->itemAt(i);
            if (!item->widget())
            {
                continue;
            }
            BiometricItem *biometricItem = qobject_cast<BiometricItem *>(item->widget());
            if (!biometricItem)
            {
                continue;
            }
            /* 如果是添加按钮忽略，不需要取出数据 */
            if (biometricItem->getItemType() == BiometricItem::BIOMETRIC_ITEM_ADD)
            {
                continue;
            }
            QPair<QString, QString> biometricPair(biometricItem->getBiometricItemName(),
                                                  biometricItem->getBiometricItemDataID());
            biometricItems.append(biometricPair);
        }
    };
    BiometricList res;
    if (mode == ACCOUNTS_AUTH_MODE_FINGERPRINT)
    {
        funcGetBiometricItems(ui->layout_fingerTemplate, res);
    }
    else if (mode == ACCOUNTS_AUTH_MODE_FACE)
    {
        funcGetBiometricItems(ui->layout_faceTemplate, res);
    }
    return res;
}

BiometricList AuthManagerPage::getBiometricItemsFromBackend(AccountsAuthMode mode)
{
    /* 解析获取生物特征列表函数 */
    auto funcParseAuthItmes = [](const QString &jsonStr, BiometricList &authItems) -> bool {
        auto errorPtr = new QSharedPointer<QJsonParseError>(new QJsonParseError);
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), errorPtr->data());
        if (errorPtr->data()->error != QJsonParseError::NoError)
        {
            //            KLOG_DEBUG() << "parse json doc failed," << errorPtr->data()->errorString() << ",json doc:" << jsonStr;
            return false;
        }
        if (!doc.isArray())
        {
            //            KLOG_WARNING() << "format error, is not json array";
            return false;
        }
        authItems.clear();
        if (doc.isEmpty())
        {
            return true;
        }
        QJsonArray array = doc.array();
        for (auto iter : array)
        {
            QJsonObject obj = iter.toObject();
            if (!obj.contains("data_id") || !obj.contains("name"))
            {
                KLOG_WARNING() << "format error,leak data_id/name element.";
                authItems.clear();
                return false;
            }
            KLOG_INFO() << obj.value("name").toString();
            QPair<QString, QString> nameIdPair = QPair<QString, QString>(obj.value("name").toString(),
                                                                         obj.value("data_id").toString());
            authItems.append(nameIdPair);
        }
        return true;
    };
    BiometricList res;
    QDBusPendingReply<QString> reply = m_userProxy->GetAuthItems(mode);
    reply.waitForFinished();
    funcParseAuthItmes(reply.value(), res);
    return res;
}

void AuthManagerPage::slotItemDeleteClicked()
{
    auto item = qobject_cast<BiometricItem *>(sender());
    delete item;
    m_addFingerItem->setItemAddEnabled(ui->layout_fingerTemplate->count() <= 1);
    m_addFaceItem->setItemAddEnabled(ui->layout_faceTemplate->count() <= 1);
}

BiometricItem *AuthManagerPage::newBiometricItem(const QString &name, const QString &dataID)
{
    BiometricItem *item = new BiometricItem(name, dataID, BiometricItem::BIOMETRIC_ITEM_NORMAL, this);
    connect(item, &BiometricItem::sigDeleteBiometricItem, this, &AuthManagerPage::slotItemDeleteClicked);
    return item;
}

void AuthManagerPage::slotAddBiometricsItem()
{
    auto item = qobject_cast<BiometricItem *>(sender());

    if (item == m_addFingerItem)
    {
        FingerprintInputDialog fingerPrint;
        fingerPrint.show();
        QEventLoop eventLoop;
        connect(&fingerPrint, &FingerprintInputDialog::sigClose, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

        QString fingerDataID = fingerPrint.getFingerDataID();
        if (fingerDataID.isEmpty())
        {
            return;
        }

        QString fingerName = generateBiometricsItemName(ACCOUNTS_AUTH_MODE_FINGERPRINT);
        auto newItem = newBiometricItem(fingerName, fingerDataID);
        ui->layout_fingerTemplate->insertWidget(ui->layout_fingerTemplate->count() - 1, newItem);
        m_addFingerItem->setItemAddEnabled(ui->layout_fingerTemplate->count() <= 1);
    }
    else if (item == m_addFaceItem)
    {
        FaceInputDialog faceInputDialog;
        faceInputDialog.show();
        QEventLoop eventLoop;
        connect(&faceInputDialog, &FaceInputDialog::sigClose, &eventLoop, &QEventLoop::quit);
        eventLoop.exec();

        QString faceDataID = faceInputDialog.getFaceDataID();
        if (faceDataID.isEmpty())
        {
            return;
        }

        QString faceName = generateBiometricsItemName(ACCOUNTS_AUTH_MODE_FACE);
        auto newItem = newBiometricItem(faceName, faceDataID);
        ui->layout_faceTemplate->insertWidget(ui->layout_faceTemplate->count() - 1, newItem);
        m_addFaceItem->setItemAddEnabled(ui->layout_fingerTemplate->count() <= 1);
    }
}

QString AuthManagerPage::generateBiometricsItemName(AccountsAuthMode mode)
{
    QSet<uint> set;
    QVBoxLayout *layout = mode == ACCOUNTS_AUTH_MODE_FINGERPRINT ? ui->layout_fingerTemplate : ui->layout_faceTemplate;

    for (int i = 0; i < layout->count(); i++)
    {
        auto layoutItem = layout->itemAt(i);
        if (!layoutItem->widget())
        {
            continue;
        }
        BiometricItem *item = qobject_cast<BiometricItem *>(layoutItem->widget());
        if (!item || item->getItemType() == BiometricItem::BIOMETRIC_ITEM_ADD)
        {
            continue;
        }
        QString name = item->getBiometricItemName();
        QStringList splitRes = name.split("_");
        if (splitRes.size() < 2)
        {
            continue;
        }
        bool isOK = false;
        uint idx = splitRes.at(1).toUInt(&isOK);
        if (!isOK)
        {
            continue;
        }
        set.insert(idx);
    }

    uint idx = 1;
    forever
    {
        if (!set.contains(idx))
        {
            break;
        }
        else
        {
            idx++;
        }
    };
    QString prefix = mode == ACCOUNTS_AUTH_MODE_FINGERPRINT ? AuthManagerPage::tr("fingerprint_") : AuthManagerPage::tr("face_");
    QString res = prefix + QString::number(idx);
    return res;
}

void AuthManagerPage::slotUserPropertyChanged(QString propertyName, QVariant value)
{
    if (propertyName == "auth_modes")
    {
        KLOG_DEBUG() << "user auth_modes property changed!"
                     << "\n"
                     << "";
        updateInfo();
    }
}

void AuthManagerPage::slotUserAuthItemChanged()
{
    updateInfo();
}
