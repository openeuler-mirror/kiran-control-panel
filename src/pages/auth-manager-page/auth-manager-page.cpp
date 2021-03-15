//
// Created by lxh on 2021/3/11.
//

// You may need to build the project (run Qt uic code generator) to get "ui_auth-manager-page.h" resolved

#include "auth-manager-page.h"
#include "ui_auth-manager-page.h"
#include "accounts-user-interface.h"
#include "biometric-item.h"
#include "biometric-input-dialog/fingerprint-input-dialog/fingerprint-input-dialog.h"

#include <QList>
#include <QJsonDocument>
#include <kiran-message-box.h>

AuthManagerPage::AuthManagerPage(QWidget *parent) :
        QWidget(parent), ui(new Ui::AuthManagerPage) {
    ui->setupUi(this);
    initUI();
}

AuthManagerPage::~AuthManagerPage() {
    delete ui;
}

void AuthManagerPage::setCurrentUser(const QString &userObj) {
    m_userObjPath = userObj;

    delete m_userInterface;
    m_userInterface = new UserInterface(m_userObjPath,QDBusConnection::systemBus(),this);
    connect(m_userInterface,&UserInterface::AuthItemChanged,[this](){
        updateInfo();
    });
    connect(m_userInterface,&UserInterface::propertyChanged,[this](QString path,QString propertyName,QVariant value){
        if( propertyName == "auth_modes" ){
            updateInfo();
        }
    });
    updateInfo();
}

void AuthManagerPage::initUI() {
    connect(ui->btn_save,&QPushButton::clicked,[this](){
        save();
    });
    connect(ui->btn_return,&QPushButton::clicked,[this](){
        Q_EMIT sigReturn();
    });
    //TODO:连接到每个CheckBox切换时判断是否存在勾选项
    connect(ui->check_passwdAuth,&QCheckBox::stateChanged,this,&AuthManagerPage::slotCheckAuthTypes);
    connect(ui->check_fingerAuth,&QCheckBox::stateChanged,this,&AuthManagerPage::slotCheckAuthTypes);
    connect(ui->check_faceAuth,&QCheckBox::stateChanged,this,&AuthManagerPage::slotCheckAuthTypes);
}

void AuthManagerPage::updateInfo() {
    //获取认证类型开关
    bool fingerAuth,faceAuth,passwdAuth;
    int authModes = m_userInterface->auth_modes();
    if( authModes == 0 ){
        authModes = ACCOUNTS_AUTH_MODE_PASSWORD;
    }
    ui->check_passwdAuth->setChecked( (authModes & ACCOUNTS_AUTH_MODE_PASSWORD) );
    ui->check_faceAuth->setChecked( (authModes & ACCOUNTS_AUTH_MODE_FINGERPRINT) );
    ui->check_fingerAuth->setChecked( (authModes & ACCOUNTS_AUTH_MODE_FACE) );


    //清空之前的生物特征值
    auto cleanLayoutFunc = [](QLayout* layout){
        QLayoutItem* childItem = nullptr;
        while( (childItem = layout->takeAt(0)) != 0){
            if(childItem->widget()){
                childItem->widget()->setParent(nullptr);
            }
            delete childItem;
        }
    };
    cleanLayoutFunc(ui->layout_fingerTemplate);
    cleanLayoutFunc(ui->layout_faceTemplate);

    //获取已录入的生物特征值,填充进界面
    /* 添加指纹信息进界面 */
    BiometricItem* item = nullptr;
    BiometricList fingerAuthItemsList;
    fingerAuthItemsList = getBiometricItemsFromBackend(ACCOUNTS_AUTH_MODE_FINGERPRINT);
    for(auto & fingerTemplate : fingerAuthItemsList){
        item = new BiometricItem(fingerTemplate.first,fingerTemplate.second,BiometricItem::BIOMETRIC_ITEM_NORMAL,this);
        connect(item,&BiometricItem::sigDeleteBiometricItem,[=](const QString &name){
            ui->layout_fingerTemplate->removeWidget(item);
        });
        ui->layout_fingerTemplate->addWidget(item);
    }
    item = new BiometricItem("add fingerprint","",BiometricItem::BIOMETRIC_ITEM_ADD,this);
    item->setItemAddEnabled(fingerAuthItemsList.isEmpty());
    ui->layout_fingerTemplate->addWidget(item);
    connect(item,&BiometricItem::sigAddBiometricItem,[this](){
        qInfo() << "enroll fingerprint";
        FingerprintInputDialog fingerPrint;
        fingerPrint.show();
        QEventLoop eventLoop;
        connect(&fingerPrint,&FingerprintInputDialog::sigClose,&eventLoop,&QEventLoop::quit);
        eventLoop.exec();
        //NOTE:add finger 图形，先不存储到后端
    });
    /* 添加人脸信息 */
    BiometricList faceAuthItemsList;
    faceAuthItemsList = getBiometricItemsFromBackend(ACCOUNTS_AUTH_MODE_FACE);
    for(auto & faceTemplate : faceAuthItemsList){
        item = new BiometricItem(faceTemplate.first,faceTemplate.second,BiometricItem::BIOMETRIC_ITEM_NORMAL,this);
        connect(item,&BiometricItem::sigDeleteBiometricItem,[=](const QString &name){
            ui->layout_faceTemplate->removeWidget(item);
        });
        ui->layout_faceTemplate->addWidget(item);
    }
    item = new BiometricItem("add face","",BiometricItem::BIOMETRIC_ITEM_ADD,this);
    item->setItemAddEnabled(faceAuthItemsList.isEmpty());
    ui->layout_faceTemplate->addWidget(item);
    connect(item,&BiometricItem::sigAddBiometricItem,[this](){
        qInfo() << "enroll face";
        //NOTE:add face只添加图形，先不存储到后端
    });
}

void AuthManagerPage::save() {
    bool uiPasswdAuth,uiFingerAuth,uiFaceAuth;
    uiPasswdAuth = ui->check_passwdAuth->isChecked();
    uiFingerAuth = ui->check_fingerAuth->isChecked();
    uiFaceAuth = ui->check_faceAuth->isChecked();
    /* 检查验证选项至少保证一个打开 */
    if( !uiPasswdAuth && !uiFingerAuth && !uiFaceAuth ){
        KiranMessageBox::message(this,
                                 tr("error"),
                                 tr("please ensure that at least one authentication option exists"),
                                 KiranMessageBox::Yes);
        return;
    }

    bool backendPasswdAuth,backendFingerAuth,backendFaceAuth;
    int authModes = m_userInterface->auth_modes();
    backendPasswdAuth = authModes&ACCOUNTS_AUTH_MODE_PASSWORD;
    backendFingerAuth = authModes&ACCOUNTS_AUTH_MODE_FINGERPRINT;
    backendFaceAuth = authModes&ACCOUNTS_AUTH_MODE_FACE;

    /* 从界面上获取生物识别特征值 */
    BiometricList uiFingerTemplates,uiFaceTemplates;
    uiFingerTemplates = getBiometricItemsFromUI(ACCOUNTS_AUTH_MODE_FINGERPRINT);
    uiFaceTemplates = getBiometricItemsFromUI(ACCOUNTS_AUTH_MODE_FACE);

    /* 从后端获取生物识别特征值 */
    BiometricList backendFingerTemplates,backendFaceTemplates;
    backendFingerTemplates = getBiometricItemsFromUI(ACCOUNTS_AUTH_MODE_FINGERPRINT);
    backendFaceTemplates = getBiometricItemsFromUI(ACCOUNTS_AUTH_MODE_FACE);

    /* 保存验证选项 */
    if( uiPasswdAuth!=backendPasswdAuth ){
        m_userInterface->EnableAuthMode(ACCOUNTS_AUTH_MODE_PASSWORD,uiPasswdAuth);
    }
    if( uiFingerAuth!=backendFingerAuth ){
        m_userInterface->EnableAuthMode(ACCOUNTS_AUTH_MODE_FINGERPRINT,uiFingerAuth);
    }
    if( uiFaceAuth!=backendFaceAuth ){
        m_userInterface->EnableAuthMode(ACCOUNTS_AUTH_MODE_FACE,uiFaceAuth);
    }
    auto funcCompareBiometricItems = [](const BiometricList& newItems,const BiometricList& oldItems,
                                        BiometricList& deletedItems,BiometricList& addItems ){
        deletedItems.clear();
        addItems.clear();

        for(auto &item : newItems){
            if( !oldItems.contains(item) ){
                addItems.append(item);
            }
        }

        for(auto &item : oldItems){
            if( !newItems.contains(item) ){
                deletedItems.append(item);
            }
        }
    };
    BiometricList deletedItems,addItems;
    funcCompareBiometricItems(uiFingerTemplates,backendFingerTemplates,deletedItems,addItems);
    for( auto &item:deletedItems ){
        m_userInterface->DelAuthItem(ACCOUNTS_AUTH_MODE_FINGERPRINT,item.first);
    }
    for( auto &item:addItems ){
        m_userInterface->AddAuthItem(ACCOUNTS_AUTH_MODE_FINGERPRINT,item.first,item.second);
    }

    funcCompareBiometricItems(uiFaceTemplates,backendFaceTemplates,deletedItems,addItems);
    for( auto &item:deletedItems ){
        m_userInterface->DelAuthItem(ACCOUNTS_AUTH_MODE_FACE,item.first);
    }
    for( auto &item:addItems ){
        m_userInterface->AddAuthItem(ACCOUNTS_AUTH_MODE_FACE,item.first,item.second);
    }
}

/// @brief 每个认证开关关闭时验证是否存在其他认证方式，若不存在则重新打开开关
/// @param state
void AuthManagerPage::slotCheckAuthTypes(int state) {
    if( state != Qt::Unchecked ){
        return;
    }

    if( ui->check_faceAuth->isChecked()||
        ui->check_fingerAuth->isChecked()||
        ui->check_passwdAuth->isChecked()){
        return;
    }

    KiranMessageBox::message(this,
                             tr("error"),
                             tr("please ensure that at least one authentication option exists"),
                             KiranMessageBox::Yes);
    QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());

    checkbox->setCheckState(Qt::Checked);
}

BiometricList AuthManagerPage::getBiometricItemsFromUI(AccountsAuthMode mode) {
    //从布局中取出每项保存的数据
    auto funcGetBiometricItems = [](QLayout* layout,BiometricList& biometricItems ){
        for(int i=0;i<layout->count();i++){
            auto item = layout->itemAt(i);
            if( !item->widget() ){
                continue;
            }
            BiometricItem* biometricItem = qobject_cast<BiometricItem*>(item->widget());
            if(!biometricItem){
                continue;
            }
            /* 如果是添加按钮忽略，不需要取出数据 */
            if(biometricItem->getItemType() == BiometricItem::BIOMETRIC_ITEM_ADD){
                continue;
            }
            QPair<QString,QString> biometricPair(biometricItem->getBiometricItemName(),
                                                 biometricItem->getBiometricItemDataID());
            biometricItems.append(biometricPair);
        }
    };
    BiometricList res;
    if( mode==ACCOUNTS_AUTH_MODE_FINGERPRINT ){
        funcGetBiometricItems(ui->layout_fingerTemplate,res);
    }else if( mode==ACCOUNTS_AUTH_MODE_FACE ){
        funcGetBiometricItems(ui->layout_faceTemplate,res);
    }
    return res;
}

BiometricList AuthManagerPage::getBiometricItemsFromBackend(AccountsAuthMode mode) {
    /* 解析获取生物特征列表函数 */
    auto funcParseAuthItmes = [](const QString& jsonStr,BiometricList& authItems ) -> bool{
        auto errorPtr =  new QSharedPointer<QJsonParseError>(new QJsonParseError);
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toLatin1(),errorPtr->data());
        if( errorPtr->data()->error!=QJsonParseError::NoError ){
            qDebug() << "parse json doc failed," <<  errorPtr->data()->errorString();
            return false;
        }
        if( !doc.isArray() ){
            qWarning() << "format error, is not json array";
            return false;
        }
        authItems.clear();
        if( doc.isEmpty() ){
            return true;
        }
        QJsonArray array = doc.array();
        for(auto iter:array){
            QJsonObject obj = iter.toObject();
            if( !obj.contains("data_id") || !obj.contains("name") ){
                qWarning() << "format error,leak data_id/name element.";
                authItems.clear();
                return false;
            }
            qInfo() << obj.value("name").toString();
            QPair<QString,QString> nameIdPair = QPair<QString,QString>(obj.value("name").toString() ,
                                                                       obj.value("data_id").toString() );
            authItems.append(nameIdPair);
        }
        return true;
    };
    BiometricList res;
    QDBusPendingReply<QString> reply = m_userInterface->GetAuthItems(mode);
    reply.waitForFinished();
    funcParseAuthItmes(reply.value(),res);
    return res;
}
