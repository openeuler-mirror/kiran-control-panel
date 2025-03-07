#include "connection-settings-page.h"
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WiredSetting>
#include <QScrollBar>
#include "device-manager.h"
#include "kiran-tips/kiran-tips.h"
#include "logging-category.h"
#include "page-manager.h"
#include "ui_connection-settings-page.h"

#define RETURN_WITH_ERROR_TIPS(text, widget) \
    {                                        \
        m_tips->setText(text);               \
        m_tips->showTipAroundWidget(widget); \
        return;                              \
    }

using namespace NetworkManager;

namespace Kiran
{
namespace Network
{
enum BoundDeviceRole
{
    BOUND_ROLE_HARDWARE_ADDRESS = Qt::UserRole + 1,
    BOUND_ROLE_INTERFACE_NAME,
    BOUND_ROLE_LAST
};

ConnectionSettingsPage::ConnectionSettingsPage(QWidget *parent) : QWidget(parent),
                                                                  ui(new ::Ui::ConnectionSettingsPage)
{
    ui->setupUi(this);
    init();
}

ConnectionSettingsPage::~ConnectionSettingsPage()
{
    delete ui;
}

void ConnectionSettingsPage::reset()
{
    m_connectionSettings.clear();
    m_existingConnection.clear();

    ui->scrollArea->verticalScrollBar()->setValue(0);

    ui->edit_name->clear();
    ui->switch_autoConnect->setChecked(false);
    ui->ipv4->reset();
    ui->ipv6->reset();
    ui->combo_device->clear();

    ui->btn_save->setFocus();
}

/// @brief 处理外部请求跳转，编辑/创建 有线网络配置
/// @param devicePath 操作的设备路径，主要用于生成新配置时默认绑定该设备
/// @param connectionPath 网络配置路径，编辑网络配置时传入，新建网络配置传空
void ConnectionSettingsPage::editConnectionSettings(const QString &devicePath,
                                                    const QString &connectionPath)
{
    reset();

    auto device = findNetworkInterface(devicePath);
    if (!device)
    {
        KLOG_WARNING(qLcNetwork) << "find network interface failed, devicePath:" << devicePath;
        return;
    }

    if (connectionPath.isEmpty())
    {
        // 创建新连接配置,生成默认值
        m_connectionSettings = QSharedPointer<NetworkManager::ConnectionSettings>::create(ConnectionSettings::Wired);
        auto settingsUuid = m_connectionSettings->createNewUuid();

        // uuid以及连接名
        m_connectionSettings->setUuid(settingsUuid);
        m_connectionSettings->setId(newConnectionName());

        // 默认自动连接
        m_connectionSettings->setAutoconnect(true);

        // 设备绑定
        auto wiredDevice = qSharedPointerDynamicCast<WiredDevice>(device);
        auto hardwareAddress = wiredDevice->permanentHardwareAddress();
        if (hardwareAddress.isEmpty())
        {
            hardwareAddress = wiredDevice->hardwareAddress();
        }

        m_connectionSettings->setInterfaceName(device->interfaceName());
        auto wiredSettings = m_connectionSettings->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();
        wiredSettings->setMacAddress(QByteArray::fromHex(hardwareAddress.toUtf8()));

        KLOG_INFO(qLcNetwork) << "create temporarily connection, name:" << m_connectionSettings->id();
    }
    else
    {
        // 编辑已有连接
        auto connection = NetworkManager::findConnection(connectionPath);
        if (!connection || !connection->settings())
        {
            KLOG_WARNING(qLcNetwork) << "edit connection failed, can't find connection:" << connectionPath;
            return;
        }
        m_existingConnection = connection;
        m_connectionSettings = m_existingConnection->settings();
    }

    KLOG_INFO(qLcNetwork) << "edit connection settings:" << m_connectionSettings->id();
    loadSettingsInfo();
}

void ConnectionSettingsPage::init()
{
    connect(ui->btn_return, &QPushButton::clicked, this, [this]()
            { emit PM_INSTANCE->requestWiredDevicePage(); });
    connect(ui->btn_save, &QPushButton::clicked, this, &ConnectionSettingsPage::save);

    m_tips = new KiranTips(this);
    m_tips->setShowPosition(KiranTips::POSITION_BOTTM);
    m_tips->setAnimationEnable(true);

    ui->ipv4->setIpSettingsType(IpSettingsWidget::TYPE_IPV4);
    ui->ipv4->setTipsWidget(m_tips);

    ui->ipv6->setIpSettingsType(IpSettingsWidget::TYPE_IPV6);
    ui->ipv6->setTipsWidget(m_tips);
}

/// @brief 加载ConnectionSettings至界面
void ConnectionSettingsPage::loadSettingsInfo()
{
    if (!m_connectionSettings)
    {
        KLOG_WARNING(qLcNetwork) << "ConnectionSettings pointer invalid, can't load settings info";
        return;
    }

    ui->edit_name->setText(m_connectionSettings->id());
    ui->switch_autoConnect->setChecked(m_connectionSettings->autoconnect());

    auto ipv4Settings = m_connectionSettings->setting(Setting::SettingType::Ipv4).dynamicCast<Ipv4Setting>();
    ui->ipv4->loadSettings(ipv4Settings);

    auto ipv6Settings = m_connectionSettings->setting(Setting::SettingType::Ipv6).dynamicCast<Ipv6Setting>();
    ui->ipv6->loadSettings(ipv6Settings);

    auto wiredSettings = m_connectionSettings->setting(Setting::SettingType::Wired).dynamicCast<WiredSetting>();
    auto boundAddress = wiredSettings->macAddress().toHex(':').toUpper();
    auto boundInterface = m_connectionSettings->interfaceName();

    loadDeviceComboBox();
    setCurrentBoundDevice(boundInterface, boundAddress);
}

void ConnectionSettingsPage::loadDeviceComboBox()
{
    ui->combo_device->clear();
    for (auto devicePath : DM_INSTANCE->managedWiredDeivces())
    {
        auto device = NetworkManager::findNetworkInterface(devicePath);
        if (!device)
        {
            continue;
        }

        auto wiredDevice = qSharedPointerDynamicCast<WiredDevice>(device);
        wiredDevice->interfaceName();

        auto hardwareAddress = wiredDevice->permanentHardwareAddress();
        if (hardwareAddress.isEmpty())
        {
            hardwareAddress = wiredDevice->hardwareAddress();
        }
        ui->combo_device->addItem(QString("%1(%2)").arg(wiredDevice->interfaceName()).arg(hardwareAddress));
        ui->combo_device->setItemData(ui->combo_device->count() - 1, hardwareAddress, BOUND_ROLE_HARDWARE_ADDRESS);
        ui->combo_device->setItemData(ui->combo_device->count() - 1, wiredDevice->interfaceName(), BOUND_ROLE_INTERFACE_NAME);
    }
    ui->combo_device->addItem(tr("Unspecified device"));
}

void ConnectionSettingsPage::setCurrentBoundDevice(const QString &boundInterface, const QString &boundAddress)
{
    // 检查配置绑定设备信息
    int deviceBoundIndex = -1;

    // 存在绑定信息
    if (!boundInterface.isEmpty() || !boundAddress.isEmpty())
    {
        // 检查全匹配
        for (int i = 0; i < ui->combo_device->count(); i++)
        {
            auto item = ui->combo_device->itemData(i);
            auto deviceAddr = ui->combo_device->itemData(i, BOUND_ROLE_HARDWARE_ADDRESS);
            auto deviceName = ui->combo_device->itemData(i, BOUND_ROLE_INTERFACE_NAME);
            // 配置中绑定信息全匹配设备
            if ((!boundInterface.isEmpty() && deviceName == boundInterface) &&
                (!boundAddress.isEmpty() && deviceAddr == boundAddress))
            {
                deviceBoundIndex = i;
                break;
            }
        }
        // 没匹配上新增一项，并选中
        if (deviceBoundIndex == -1)
        {
            QString tempBoundDevice;
            if (!boundInterface.isEmpty())
            {
                tempBoundDevice = boundInterface;
            }
            if (!boundAddress.isEmpty())
            {
                tempBoundDevice += '(' + boundAddress + ')';
            }
            ui->combo_device->insertItem(0, tempBoundDevice);
            ui->combo_device->setItemData(0, boundAddress, BOUND_ROLE_HARDWARE_ADDRESS);
            ui->combo_device->setItemData(0, boundInterface, BOUND_ROLE_INTERFACE_NAME);
            deviceBoundIndex = 0;
        }
    }

    // 没匹配，不绑定设备
    if (deviceBoundIndex == -1)
    {
        deviceBoundIndex = ui->combo_device->count() - 1;
    }

    ui->combo_device->setCurrentIndex(deviceBoundIndex);
}

bool ConnectionSettingsPage::macValid(const QString &mac)
{
    if (mac.isEmpty())
    {
        return true;
    }
    bool matched = QRegExp("^([0-9A-Fa-f]{2}[:]){5}([0-9A-Fa-f]{2})$").exactMatch(mac);
    return matched;
}

void ConnectionSettingsPage::save()
{
    // 检查输入有效性
    if (ui->edit_name->text().isEmpty())
    {
        RETURN_WITH_ERROR_TIPS(tr("Please fill in the connection configuration name"), ui->edit_name);
    }

    if (!ui->ipv4->checkValid())
    {
        return;
    }

    if (!ui->ipv6->checkValid())
    {
        return;
    }

    // 保存配置
    auto wiredSettings = m_connectionSettings->setting(Setting::Wired).dynamicCast<WiredSetting>();
    m_connectionSettings->setId(ui->edit_name->text());
    m_connectionSettings->setAutoconnect(ui->switch_autoConnect->isChecked());

    ui->ipv4->save();
    ui->ipv6->save();

    const auto boundHWAddr = ui->combo_device->currentData(BOUND_ROLE_HARDWARE_ADDRESS).toString();
    const auto boundHWName = ui->combo_device->currentData(BOUND_ROLE_INTERFACE_NAME).toString();
    wiredSettings->setMacAddress(QByteArray::fromHex(boundHWAddr.toUtf8()));
    m_connectionSettings->setInterfaceName(boundHWName);

    // 提交更新
    if (m_existingConnection)
    {
        // 已有配置修改
        auto reply = m_existingConnection->update(m_connectionSettings->toMap());
        reply.waitForFinished();
        KLOG_INFO(qLcNetwork) << "connection setting saved" << m_connectionSettings->id() << "result:" << !reply.isError();
    }
    else
    {
        // 新增配置
        auto reply = NetworkManager::addConnection(m_connectionSettings->toMap());
        reply.waitForFinished();
        KLOG_INFO(qLcNetwork) << "connection setting added" << m_connectionSettings->id() << "result:" << !reply.isError();
    }

    emit PM_INSTANCE->requestWiredDevicePage();
}

QString ConnectionSettingsPage::newConnectionName()
{
    int wiredSettingsCount = 0;

    NetworkManager::Connection::List connList = listConnections();
    for (auto conn : connList)
    {
        if (conn->settings()->connectionType() == ConnectionSettings::Wired)
        {
            wiredSettingsCount++;
        }
    }

    QString connectionID = QString(tr("Wired Connection %1")).arg(wiredSettingsCount + 1);
    return connectionID;
}

}  // namespace Network
}  // namespace Kiran