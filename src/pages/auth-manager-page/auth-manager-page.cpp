//
// Created by lxh on 2021/3/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_auth-manager-page.h" resolved

#include "auth-manager-page.h"
#include "ui_auth-manager-page.h"
#include "accounts-user-interface.h"
#include "biometric-item.h"
#include "biometric-input-dialog/fingerprint-input-dialog/fingerprint-input-dialog.h"
#include "biometric-input-dialog/face-input-dialog/face-input-dialog.h"

#include <QList>
#include <QJsonDocument>
#include <kiran-message-box.h>
#include <kiran-switch-button.h>

AuthManagerPage::AuthManagerPage (QWidget *parent) :
        QWidget(parent), ui(new Ui::AuthManagerPage)
{
    ui->setupUi(this);
    initUI();
}

AuthManagerPage::~AuthManagerPage ()
{
    delete ui;
}

void AuthManagerPage::setCurrentUser (const QString &userObj)
{
    m_userObjPath = userObj;

    delete m_userInterface;
    m_userInterface = new UserInterface(m_userObjPath, QDBusConnection::systemBus(), this);
    connect(m_userInterface, &UserInterface::AuthItemChanged, [this] () {
        updateInfo();
    });
    connect(m_userInterface, &UserInterface::propertyChanged,
            [this] (QString path, QString propertyName, QVariant value) {
                if (propertyName == "auth_modes")
                {
                    updateInfo();
                }
            });
    updateInfo();
}

void AuthManagerPage::initUI ()
{
    m_fingerAuthSwitch = new KiranSwitchButton(this);
    ui->layout_fingerAuth->addWidget(m_fingerAuthSwitch);
    connect(m_fingerAuthSwitch, &KiranSwitchButton::toggled, this, &AuthManagerPage::slotCheckAuthTypes);

    m_faceAuthSwitch = new KiranSwitchButton(this);
    ui->layout_faceAuth->addWidget(m_faceAuthSwitch);
    connect(m_faceAuthSwitch, &KiranSwitchButton::toggled, this, &AuthManagerPage::slotCheckAuthTypes);

    m_passwdAuthSwitch = new KiranSwitchButton(this);
    ui->layout_passwdAuth->addWidget(m_passwdAuthSwitch);
    connect(m_passwdAuthSwitch,&KiranSwitchButton::toggled, this, &AuthManagerPage::slotCheckAuthTypes);

    connect(ui->btn_save, &QPushButton::clicked, [this] () {
        save();
    });
    connect(ui->btn_return, &QPushButton::clicked, [this] () {
        Q_EMIT sigReturn();
    });
}

void AuthManagerPage::updateInfo ()
{
    qInfo() << "load biometrics , update ui";
    //获取认证类型开关
    bool fingerAuth, faceAuth, passwdAuth;
    int authModes = m_userInterface->auth_modes();
    if (authModes == 0)
    {
        authModes = ACCOUNTS_AUTH_MODE_PASSWORD;
    }
    m_passwdAuthSwitch->setCheckable(authModes & ACCOUNTS_AUTH_MODE_PASSWORD);
    m_fingerAuthSwitch->setCheckable(authModes & ACCOUNTS_AUTH_MODE_FINGERPRINT);
    m_faceAuthSwitch->setCheckable(authModes & ACCOUNTS_AUTH_MODE_FACE);

    //清空之前的生物特征值
    auto cleanLayoutFunc = [] (QLayout *layout) {
        QLayoutItem *childItem = nullptr;
        while ((childItem = layout->takeAt(0)) != 0)
        {
            if (childItem->widget())
            {
                childItem->widget()->setParent(nullptr);
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

void AuthManagerPage::save ()
{
    qInfo() << "save..";
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
    int authModes = m_userInterface->auth_modes();
    backendPasswdAuth = authModes & ACCOUNTS_AUTH_MODE_PASSWORD;
    backendFingerAuth = authModes & ACCOUNTS_AUTH_MODE_FINGERPRINT;
    backendFaceAuth = authModes & ACCOUNTS_AUTH_MODE_FACE;
    /* 保存验证选项 */
    if (uiPasswdAuth != backendPasswdAuth)
    {
        qInfo() << "update passwd auth mode: " << uiPasswdAuth;
        m_userInterface->EnableAuthMode(ACCOUNTS_AUTH_MODE_PASSWORD, uiPasswdAuth);
    }
    if (uiFingerAuth != backendFingerAuth)
    {
        qInfo() << "update finger auth mode: " << uiFingerAuth;
        m_userInterface->EnableAuthMode(ACCOUNTS_AUTH_MODE_FINGERPRINT, uiFingerAuth);
    }
    if (uiFaceAuth != backendFaceAuth)
    {
        qInfo() << "update face auth mode: " << uiFaceAuth;
        m_userInterface->EnableAuthMode(ACCOUNTS_AUTH_MODE_FACE, uiFaceAuth);
    }

    /* 从界面上获取生物识别特征值 */
    BiometricList uiFingerTemplates, uiFaceTemplates;
    uiFingerTemplates = getBiometricItemsFromUI(ACCOUNTS_AUTH_MODE_FINGERPRINT);
    uiFaceTemplates = getBiometricItemsFromUI(ACCOUNTS_AUTH_MODE_FACE);

    /* 从后端获取生物识别特征值 */
    BiometricList backendFingerTemplates, backendFaceTemplates;
    backendFingerTemplates = getBiometricItemsFromBackend(ACCOUNTS_AUTH_MODE_FINGERPRINT);
    backendFaceTemplates = getBiometricItemsFromBackend(ACCOUNTS_AUTH_MODE_FACE);


    auto funcCompareBiometricItems = [] (const BiometricList &newItems, const BiometricList &oldItems,
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
    funcCompareBiometricItems(uiFingerTemplates, backendFingerTemplates, deletedItems, addItems);
    qInfo() << "finger biometric items:";
    qInfo() << "need delete item:" << deletedItems;
    qInfo() << "need add item:   " << addItems;
    for (auto &item:deletedItems)
    {
        m_userInterface->DelAuthItem(ACCOUNTS_AUTH_MODE_FINGERPRINT, item.first);
    }
    for (auto &item:addItems)
    {
        m_userInterface->AddAuthItem(ACCOUNTS_AUTH_MODE_FINGERPRINT, item.first, item.second);
    }

    funcCompareBiometricItems(uiFaceTemplates, backendFaceTemplates, deletedItems, addItems);
    qInfo() << "face biometric items:";
    qInfo() << "need delete item:" << deletedItems;
    qInfo() << "need add item:   " << addItems;
    for (auto &item:deletedItems)
    {
        m_userInterface->DelAuthItem(ACCOUNTS_AUTH_MODE_FACE, item.first);
    }
    for (auto &item:addItems)
    {
        m_userInterface->AddAuthItem(ACCOUNTS_AUTH_MODE_FACE, item.first, item.second);
    }
}

/// @brief 每个认证开关关闭时验证是否存在其他认证方式，若不存在则重新打开开关
/// @param checked
void AuthManagerPage::slotCheckAuthTypes (bool checked)
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

BiometricList AuthManagerPage::getBiometricItemsFromUI (AccountsAuthMode mode)
{
    //从布局中取出每项保存的数据
    auto funcGetBiometricItems = [] (QLayout *layout, BiometricList &biometricItems) {
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

BiometricList AuthManagerPage::getBiometricItemsFromBackend (AccountsAuthMode mode)
{
    /* 解析获取生物特征列表函数 */
    auto funcParseAuthItmes = [] (const QString &jsonStr, BiometricList &authItems) -> bool {
        auto errorPtr = new QSharedPointer<QJsonParseError>(new QJsonParseError);
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toLatin1(), errorPtr->data());
        if (errorPtr->data()->error != QJsonParseError::NoError)
        {
            qDebug() << "parse json doc failed," << errorPtr->data()->errorString();
            return false;
        }
        if (!doc.isArray())
        {
            qWarning() << "format error, is not json array";
            return false;
        }
        authItems.clear();
        if (doc.isEmpty())
        {
            return true;
        }
        QJsonArray array = doc.array();
        for (auto iter:array)
        {
            QJsonObject obj = iter.toObject();
            if (!obj.contains("data_id") || !obj.contains("name"))
            {
                qWarning() << "format error,leak data_id/name element.";
                authItems.clear();
                return false;
            }
            qInfo() << obj.value("name").toString();
            QPair<QString, QString> nameIdPair = QPair<QString, QString>(obj.value("name").toString(),
                                                                         obj.value("data_id").toString());
            authItems.append(nameIdPair);
        }
        return true;
    };
    BiometricList res;
    QDBusPendingReply<QString> reply = m_userInterface->GetAuthItems(mode);
    reply.waitForFinished();
    funcParseAuthItmes(reply.value(), res);
    return res;
}

void AuthManagerPage::slotItemDeleteClicked ()
{
    auto item = qobject_cast<BiometricItem *>(sender());
    delete item;
    m_addFingerItem->setItemAddEnabled(ui->layout_fingerTemplate->count() <= 1);
    m_addFaceItem->setItemAddEnabled(ui->layout_faceTemplate->count() <= 1);
}

BiometricItem *AuthManagerPage::newBiometricItem (const QString &name, const QString &dataID)
{
    BiometricItem *item = new BiometricItem(name, dataID, BiometricItem::BIOMETRIC_ITEM_NORMAL, this);
    connect(item, &BiometricItem::sigDeleteBiometricItem, this, &AuthManagerPage::slotItemDeleteClicked);
    return item;
}

void AuthManagerPage::slotAddBiometricsItem ()
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

QString AuthManagerPage::generateBiometricsItemName (AccountsAuthMode mode)
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
