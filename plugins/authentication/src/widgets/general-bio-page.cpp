#include "general-bio-page.h"
#include "auth-setting-container.h"
#include "auth-setting-item.h"
#include "checkpasswd-dialog.h"
#include "input-dialog/input-dialog.h"
#include "utils/kiran-auth-dbus-proxy.h"

#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <style-property.h>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>

#define MAX_FEATURE_NUMBER 1000

GeneralBioPage::GeneralBioPage(KiranAuthDBusProxy* proxy, KADAuthType authType, QWidget* parent)
    : QWidget(parent),
      m_proxy(proxy),
      m_authType(authType)
{
    initUI();

    refreshFeature();
    refreshDeviceComboBox();
}

GeneralBioPage::~GeneralBioPage()
{
}

void GeneralBioPage::setFeatureNamePrefix(const QString& prefix)
{
    m_featureNamePrefix = prefix;
}

QString GeneralBioPage::autoGenerateFeatureName()
{
    if (m_featureNamePrefix.isEmpty())
    {
        KLOG_WARNING() << "feature manager: generate feature name prefix is not set!";
    }

    for (int i = 0; i <= 10; ++i)
    {
        auto featureNumber = QRandomGenerator::global()->bounded(1, MAX_FEATURE_NUMBER);
        auto temp = QString("%1 %2").arg(m_featureNamePrefix).arg(featureNumber);

        if (!m_featureNameSet.contains(temp))
        {
            return temp;
        }
    }

    return QString();
}

void GeneralBioPage::addFeature(const QString& featureName, const QString& featureIID)
{
    auto featureItem = new AuthSettingItem;
    featureItem->setUserData(featureIID);
    featureItem->setText(featureName);
    featureItem->setLeftButtonVisible(true, ":/kcp-authentication/images/rename.svg");
    featureItem->setRightButtonVisible(true, ":/kcp-authentication/images/trash.svg");

    m_featureContainer->addAuthSettingItem(featureItem);

    connect(featureItem, &AuthSettingItem::leftButtonClicked, this, &GeneralBioPage::onFeatureRenameClicked);
    connect(featureItem, &AuthSettingItem::rightButtonClicked, this, &GeneralBioPage::onFeatureTrashClicked);
}

int GeneralBioPage::addDeviceItem(const QString& deviceName, const QString& deviceID)
{
    m_comboDefaultDevice->addItem(deviceName, deviceID);
    return m_comboDefaultDevice->count() - 1;
}

void GeneralBioPage::setDefaultDeviceLabelDesc(const QString& text)
{
    m_labelDefaultDevice->setText(text);
}

void GeneralBioPage::setDeviceFeatureListDesc(const QString& text)
{
    m_labelFeatureList->setText(text);
}

void GeneralBioPage::refreshFeature()
{
    m_featureContainer->clear();
    m_featureNameSet.clear();

    auto identifications = m_proxy->getUserIdentifications(m_authType);
    for (auto iter : identifications)
    {
        m_featureNameSet << iter.name;
        addFeature(iter.name, iter.IID);
    }
}

void GeneralBioPage::refreshDeviceComboBox()
{
    QSignalBlocker blocker(m_comboDefaultDevice);

    m_comboDefaultDevice->clear();

    auto devices = m_proxy->getDevicesByType(m_authType);
    auto defaultDeviceID = m_proxy->getDefaultDeviceID(m_authType);

    for (auto device : devices)
    {
        auto idx = addDeviceItem(device.name, device.id);
        if (!defaultDeviceID.isEmpty() && defaultDeviceID == device.id)
        {
            m_comboDefaultDevice->setCurrentIndex(idx);
        }
    }
}

int GeneralBioPage::getDeviceCount()
{
    return m_comboDefaultDevice->count();
}

void GeneralBioPage::onDefaultDeviceComboBoxCurrentIndexChanged(int idx)
{
    auto combo = qobject_cast<QComboBox*>(sender());
    auto name = combo->itemText(idx);
    auto id = combo->itemData(idx).toString();

    m_proxy->setDefaultDeviceID(m_authType, id);
}

void GeneralBioPage::onFeatureRenameClicked()
{
    auto settingItem = qobject_cast<AuthSettingItem*>(sender());
    auto iid = settingItem->getUserData().toString();
    auto name = settingItem->getText();

    InputDialog renameDialog(this);
    renameDialog.setTitle(tr("Rename Feature"));
    renameDialog.setDesc(tr("Please enter the renamed feature name"));
    renameDialog.setInputMode(QLineEdit::Normal,32);
    if (!renameDialog.exec())
    {
        return;
    }

    QString newName = renameDialog.getText();
    m_proxy->renameIdentification(iid, newName);
    refreshFeature();
}

void GeneralBioPage::onFeatureTrashClicked()
{
    auto settingItem = qobject_cast<AuthSettingItem*>(sender());
    auto iid = settingItem->getUserData().toString();
    auto name = settingItem->getText();

    QString text;
    if (m_authType == KAD_AUTH_TYPE_UKEY)
    {
        text = QString(tr("Are you sure you want to delete the feature called %1, "
                          "Ensure that the Ukey device is inserted; "
                          "otherwise the information stored in the Ukey will not be deleted"))
                   .arg(name);
    }
    else
    {
        text = QString(tr("Are you sure you want to delete the feature called %1")).arg(name);
    }

    if (KiranMessageBox::message(this, tr("tips"), text, KiranMessageBox::Yes | KiranMessageBox::No) != KiranMessageBox::Yes)
    {
        return;
    }

    m_proxy->deleteIdentification(iid);
    refreshFeature();
}

void GeneralBioPage::onEnrollFeatureClicked()
{
    CheckpasswdDialog dialog;
    if (!dialog.exec())
    {
        return;
    }
    dialog.hide();
    
    auto passwd = dialog.getText();
    if (!dialog.checkPasswd(passwd))
    {
        KiranMessageBox::message(this, tr("Error"),
                                 tr(" Failed to enroll feature because the password verification failed！"),
                                 KiranMessageBox::Ok);
        return;
    }

    emit enrollFeature();
}

void GeneralBioPage::initUI()
{
    auto featureManagerLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    featureManagerLayout->setSpacing(0);
    featureManagerLayout->setContentsMargins(0, 0, 0, 0);

    m_labelDefaultDevice = new QLabel(tr("default device"));
    featureManagerLayout->addWidget(m_labelDefaultDevice);

    featureManagerLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_comboDefaultDevice = new QComboBox(this);
    featureManagerLayout->addWidget(m_comboDefaultDevice);
    connect(m_comboDefaultDevice, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralBioPage::onDefaultDeviceComboBoxCurrentIndexChanged);

    featureManagerLayout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_labelFeatureList = new QLabel(tr("feature list"));
    featureManagerLayout->addWidget(m_labelFeatureList);

    featureManagerLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_featureContainer = new AuthSettingContainer(this);
    featureManagerLayout->addWidget(m_featureContainer, 1);

    featureManagerLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto addButton = new QPushButton(this);
    featureManagerLayout->addWidget(addButton);
    addButton->setIcon(QPixmap(":/kcp-keyboard/images/addition.svg"));
    Kiran::StylePropertyHelper::setButtonType(addButton, Kiran::BUTTON_Default);
    connect(addButton, &QPushButton::clicked, this, &GeneralBioPage::onEnrollFeatureClicked);

    featureManagerLayout->addStretch();
}
