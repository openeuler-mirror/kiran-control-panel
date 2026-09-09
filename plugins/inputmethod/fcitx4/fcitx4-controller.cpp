/**
 * Copyright (c) 2020 ~ 2026 KylinSec Co., Ltd.
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

#include "fcitx4-controller.h"
#include "fcitx4/fcitx4-key-formatters.h"
#include "im-key.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QDBusVariant>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSet>
#include <QVariantMap>

#include "logging-category.h"

const QString Fcitx4Controller::FCITX4_SERVICE = QStringLiteral("org.fcitx.Fcitx");
const QString Fcitx4Controller::FCITX4_PATH = QStringLiteral("/inputmethod");
const QString Fcitx4Controller::FCITX4_IFACE = QStringLiteral("org.fcitx.Fcitx.InputMethod");
const QString Fcitx4Controller::FCITX4_PROP_IMLIST = QStringLiteral("IMList");
const QString Fcitx4Controller::FCITX4_PROP_CURRENTIM = QStringLiteral("CurrentIM");

namespace
{
/// 协议键名 → fcitx4 config (section, key)
QPair<QString, QString> toConfigLocation(const QString& protocolKey)
{
    if (protocolKey == QLatin1String(ImKey::triggerKeys))
        return { QStringLiteral("Hotkey"), QStringLiteral("TriggerKey") };
    if (protocolKey == QLatin1String(ImKey::shareInputState))
        return { QStringLiteral("Program"), QStringLiteral("ShareStateAmongWindow") };
    if (protocolKey == QLatin1String(ImKey::imSwitchBackwardKey))
        return { QStringLiteral("Hotkey"), QStringLiteral("IMSwitchHotkey") };
    return {};
}

/// ShareStateAmongWindow：PerProgram ↔ Program
QString fcitx4EnumToProtocol(const QString& v)
{
    return v == QLatin1String("PerProgram") ? QStringLiteral("Program") : v;
}

QString protocolEnumToFcitx4(const QString& v)
{
    return v == QLatin1String("Program") ? QStringLiteral("PerProgram") : v;
}

/// IMSwitchHotkey 四预设（config.desc Enum）
const QString kImSwitchCandidates[] = {
    QStringLiteral("CTRL_SHIFT"),
    QStringLiteral("ALT_SHIFT"),
    QStringLiteral("CTRL_SUPER"),
    QStringLiteral("ALT_SUPER"),
};

bool isValidImSwitch(const QString& v)
{
    for (const auto& c : kImSwitchCandidates)
        if (c == v)
            return true;
    return false;
}
}  // namespace

Fcitx4Controller::Fcitx4Controller(QObject* parent)
    : QObject(parent)
{
    qDBusRegisterMetaType<Fcitx4ImItem>();
    qDBusRegisterMetaType<Fcitx4ImItemList>();

    m_watcher = new QDBusServiceWatcher(FCITX4_SERVICE,
                                        QDBusConnection::sessionBus(),
                                        QDBusServiceWatcher::WatchForRegistration
                                            | QDBusServiceWatcher::WatchForUnregistration,
                                        this);
    connect(m_watcher, &QDBusServiceWatcher::serviceRegistered,
            this, &Fcitx4Controller::onServiceRegistered);
    connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered,
            this, &Fcitx4Controller::onServiceUnregistered);

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(FCITX4_SERVICE)) {
        initInterface();
    }

    initPropertySubscription();
    ensureConfigWatcher();
}

Fcitx4Controller::~Fcitx4Controller()
{
    delete m_proxy;
}

void Fcitx4Controller::ensureConfigWatcher()
{
    if (!m_configWatcher) {
        m_configWatcher = new QFileSystemWatcher(this);
        connect(m_configWatcher, &QFileSystemWatcher::directoryChanged,
                this, &Fcitx4Controller::onConfigDirChanged);
    }

    // 目录可能尚未创建；服务注册后再补挂
    const QFileInfo dirInfo(QFileInfo(m_configFile.filePath()).absolutePath());
    if (!dirInfo.exists())
        return;
    const QString dir = dirInfo.absoluteFilePath();
    if (m_configWatcher->directories().contains(dir))
        return;
    m_configWatcher->addPath(dir);
}

void Fcitx4Controller::onConfigDirChanged(const QString& path)
{
    Q_UNUSED(path);
    if (isSelfConfigWrite())
        return;
    emit settingsChanged();
}

bool Fcitx4Controller::isSelfConfigWrite() const
{
    return m_configSelfWriteMs != 0
        && QDateTime::currentMSecsSinceEpoch() - m_configSelfWriteMs < 1000;
}

bool Fcitx4Controller::reloadConfig()
{
    if (!isAvailable())
        return false;
    QDBusPendingReply<> reply = m_proxy->ReloadConfig();
    reply.waitForFinished();
    if (!reply.isValid()) {
        qCWarning(qLcInputMethod) << "ReloadConfig failed:" << reply.error().message();
        return false;
    }
    return true;
}

bool Fcitx4Controller::probeAvailable()
{
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(FCITX4_SERVICE);
}

bool Fcitx4Controller::isAvailable() const
{
    return m_proxy && m_proxy->isValid();
}

void Fcitx4Controller::initInterface()
{
    if (m_proxy)
        return;

    m_proxy = new Fcitx4InputMethodProxy(FCITX4_SERVICE, FCITX4_PATH,
                                         QDBusConnection::sessionBus(), this);

    if (!m_proxy->isValid()) {
        qCWarning(qLcInputMethod) << "Fcitx4 proxy is invalid:"
                                  << m_proxy->lastError().message();
        delete m_proxy;
        m_proxy = nullptr;
    }
}

void Fcitx4Controller::initPropertySubscription()
{
    QDBusConnection::sessionBus().connect(FCITX4_SERVICE,
                                          FCITX4_PATH,
                                          QStringLiteral("org.freedesktop.DBus.Properties"),
                                          QStringLiteral("PropertiesChanged"),
                                          this,
                                          SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
}

void Fcitx4Controller::onServiceRegistered(const QString& service)
{
    Q_UNUSED(service);
    initInterface();
    ensureConfigWatcher();
    emit availabilityChanged(isAvailable());
}

void Fcitx4Controller::onServiceUnregistered(const QString& service)
{
    Q_UNUSED(service);
    if (m_proxy) {
        delete m_proxy;
        m_proxy = nullptr;
    }
    emit availabilityChanged(false);
}

void Fcitx4Controller::onPropertiesChanged(const QString& interfaceName,
                                           const QVariantMap& changedProperties,
                                           const QStringList& invalidatedProperties)
{
    if (interfaceName != FCITX4_IFACE)
        return;

    const bool listChanged = changedProperties.contains(FCITX4_PROP_IMLIST)
        || invalidatedProperties.contains(FCITX4_PROP_IMLIST);
    const bool currentChanged = changedProperties.contains(FCITX4_PROP_CURRENTIM)
        || invalidatedProperties.contains(FCITX4_PROP_CURRENTIM);

    if (listChanged || currentChanged)
        emit settingsChanged();
}

Fcitx4ImItemList Fcitx4Controller::readImList()
{
    if (!isAvailable())
        return {};
    return m_proxy->iMList();
}

ImEntry Fcitx4Controller::toImEntry(const Fcitx4ImItem& item) const
{
    ImEntry entry;
    entry.uniqueName = item.uniqueName;
    entry.name = item.name;
    entry.langCode = item.langCode;
    // fcitx4 keyboard 项 uniqueName 以 fcitx-keyboard- 为前缀
    entry.isKeyboard = item.uniqueName.startsWith(QLatin1String("fcitx-keyboard-"));
    // IMList 无 icon：非键盘项派生 fcitx-<uniqueName>；键盘项留空由 UI 回退
    entry.iconName = entry.isKeyboard
        ? QString()
        : QLatin1String("fcitx-") + item.uniqueName;
    return entry;
}

QVector<ImEntry> Fcitx4Controller::enabledEntries()
{
    QVector<ImEntry> result;
    const Fcitx4ImItemList raw = readImList();
    if (raw.isEmpty())
        return result;

    for (const auto& item : raw) {
        if (!item.enabled)
            continue;
        result.append(toImEntry(item));
    }

    return result;
}

QVector<ImEntry> Fcitx4Controller::availableEntries()
{
    QVector<ImEntry> result;
    const Fcitx4ImItemList raw = readImList();
    for (const auto& item : raw) {
        if (!item.enabled)
            result.append(toImEntry(item));
    }
    return result;
}

bool Fcitx4Controller::setEnabledEntries(const QVector<ImEntry>& entries)
{
    if (!isAvailable())
        return false;

    // 以最新全量 IMList 为底稿，写回时不丢未启用项
    const Fcitx4ImItemList raw = readImList();
    if (raw.isEmpty()) {
        qCWarning(qLcInputMethod) << "Fcitx4: empty IMList base, abort write (daemon down?)";
        return false;
    }

    QSet<QString> enabledNames;
    for (const auto& e : entries)
        enabledNames.insert(e.uniqueName);

    Fcitx4ImItemList writeList;
    // 1) 启用项：按入参顺序，name/langCode 以底稿为准
    for (const auto& e : entries) {
        Fcitx4ImItem item;
        item.uniqueName = e.uniqueName;
        item.name = e.name;
        item.langCode = e.langCode;
        item.enabled = true;
        for (const auto& rawItem : raw) {
            if (rawItem.uniqueName == e.uniqueName) {
                item.name = rawItem.name;
                item.langCode = rawItem.langCode;
                break;
            }
        }
        writeList.append(item);
    }
    // 2) 未启用项：保底稿相对顺序，强制 enabled=false
    for (const auto& rawItem : raw) {
        if (!enabledNames.contains(rawItem.uniqueName)) {
            Fcitx4ImItem item = rawItem;
            item.enabled = false;
            writeList.append(item);
        }
    }

    // 手动 Properties.Set 以便同步等待回执（proxy setter 会丢弃错误）
    QDBusMessage msg = QDBusMessage::createMethodCall(FCITX4_SERVICE, FCITX4_PATH,
                                                      QStringLiteral("org.freedesktop.DBus.Properties"),
                                                      QStringLiteral("Set"));
    msg << QString(FCITX4_IFACE) << QString(FCITX4_PROP_IMLIST)
        << QVariant::fromValue(QDBusVariant(QVariant::fromValue(writeList)));
    QDBusReply<void> reply = QDBusConnection::sessionBus().call(msg, QDBus::Block);
    if (!reply.isValid()) {
        qCWarning(qLcInputMethod) << "Fcitx4: set IMList failed:" << reply.error().message();
        return false;
    }

    return true;
}

void Fcitx4Controller::openAdvancedSettings()
{
    if (!isAvailable())
        return;

    QDBusPendingReply<> reply = m_proxy->Configure();
    reply.waitForFinished();
    if (!reply.isValid())
        qCWarning(qLcInputMethod) << "Configure failed:" << reply.error().message();
}

KeyFormatter* Fcitx4Controller::createKeyFormatter() const
{
    return new Fcitx4KeyFormatter();
}

KeyEntryList Fcitx4Controller::keyList(const QString& key)
{
    const auto loc = toConfigLocation(key);
    if (loc.second.isEmpty())
        return {};

    if (!m_configFile.load())
        return {};
    bool ok = false;
    const QString raw = m_configFile.value(loc.first, loc.second, &ok);
    if (!ok || raw.isEmpty())
        return {};

    Fcitx4KeyFormatter formatter;
    KeyEntryList entries;
    if (!formatter.decodeList(raw, &entries)) {
        qCWarning(qLcInputMethod) << "Fcitx4: unparseable key value in config:" << loc.first
                                  << loc.second << raw;
        return {};
    }
    return entries;
}

bool Fcitx4Controller::setKeyList(const QString& key, const KeyEntryList& list)
{
    const auto loc = toConfigLocation(key);
    if (loc.second.isEmpty())
        return false;

    QString raw;
    if (!list.isEmpty()) {
        Fcitx4KeyFormatter formatter;
        if (!formatter.encodeList(list, &raw)) {
            qCWarning(qLcInputMethod) << "Fcitx4: reject unrepresentable key entry";
            return false;
        }
    }

    if (!m_configFile.load())
        return false;
    if (!m_configFile.setValue(loc.first, loc.second, raw))
        return false;
    if (!m_configFile.save()) {
        qCWarning(qLcInputMethod) << "Fcitx4: config save failed (concurrent edit?)" << m_configFile.filePath();
        return false;
    }

    m_configSelfWriteMs = QDateTime::currentMSecsSinceEpoch();
    return reloadConfig();
}

QString Fcitx4Controller::globalEnum(const QString& key)
{
    const auto loc = toConfigLocation(key);
    if (loc.second.isEmpty())
        return {};

    const bool isImSwitch = (key == QLatin1String(ImKey::imSwitchBackwardKey));
    if (!m_configFile.load())
        return isImSwitch ? QStringLiteral("CTRL_SHIFT") : QStringLiteral("No");

    bool ok = false;
    const QString raw = m_configFile.value(loc.first, loc.second, &ok);
    if (isImSwitch) {
        const QString v = ok ? raw : QStringLiteral("CTRL_SHIFT");
        return isValidImSwitch(v) ? v : QString();
    }
    return fcitx4EnumToProtocol(ok ? raw : QStringLiteral("No"));
}

bool Fcitx4Controller::setGlobalEnum(const QString& key, const QString& value)
{
    const auto loc = toConfigLocation(key);
    if (loc.second.isEmpty())
        return false;

    const bool isImSwitch = (key == QLatin1String(ImKey::imSwitchBackwardKey));
    QString raw;
    if (isImSwitch) {
        if (!isValidImSwitch(value)) {
            qCWarning(qLcInputMethod) << "Fcitx4: reject unknown imSwitch value" << value;
            return false;
        }
        raw = value;
    } else {
        if (value != QLatin1String("All") && value != QLatin1String("Program")
            && value != QLatin1String("No")) {
            qCWarning(qLcInputMethod) << "Fcitx4: reject unknown enum value" << value << "for" << key;
            return false;
        }
        raw = protocolEnumToFcitx4(value);
    }

    if (!m_configFile.load())
        return false;
    if (!m_configFile.setValue(loc.first, loc.second, raw))
        return false;
    if (!m_configFile.save()) {
        qCWarning(qLcInputMethod) << "Fcitx4: config save failed (concurrent edit?)" << m_configFile.filePath();
        return false;
    }
    m_configSelfWriteMs = QDateTime::currentMSecsSinceEpoch();
    return reloadConfig();
}
