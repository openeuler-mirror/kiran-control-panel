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

#pragma once

#include <QDBusServiceWatcher>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

#include "fcitx4-config-file.h"
#include "fcitx4-dbus-types.h"
#include "fcitx4inputmethodproxy.h"
#include "im-entry.h"
#include "im-list-provider.h"
#include "key-sequence-edit/key-formatter.h"

class QFileSystemWatcher;

/// fcitx4 D-Bus 门面（org.fcitx.Fcitx /inputmethod）
/// 实现 ImListProvider，供「添加输入法」对话框使用。
class Fcitx4Controller : public QObject, public ImListProvider
{
    Q_OBJECT
public:
    explicit Fcitx4Controller(QObject* parent = nullptr);
    ~Fcitx4Controller() override;

    /// 即时探测：会话总线是否已注册 fcitx4 服务（仅诊断/日志）
    static bool probeAvailable();

    bool isAvailable() const override;

    /// 已启用列表（IMList 中 enabled=true，保持顺序）
    QVector<ImEntry> enabledEntries() override;

    /// 可添加候选（IMList 中 enabled=false）
    QVector<ImEntry> availableEntries() override;

    /// 写回已启用列表（含顺序、增删）
    bool setEnabledEntries(const QVector<ImEntry>& entries);

    /// 打开高级设置（Configure → fcitx-configtool）
    void openAdvancedSettings();

    /// 创建快捷键编解码器；调用方取得所有权
    KeyFormatter* createKeyFormatter() const;

    KeyEntryList keyList(const QString& key);
    bool setKeyList(const QString& key, const KeyEntryList& list);
    QString globalEnum(const QString& key);
    bool setGlobalEnum(const QString& key, const QString& value);

signals:
    void availabilityChanged(bool available);
    void settingsChanged();

private slots:
    void onServiceRegistered(const QString& service);
    void onServiceUnregistered(const QString& service);
    void onPropertiesChanged(const QString& interfaceName,
                             const QVariantMap& changedProperties,
                             const QStringList& invalidatedProperties);
    void onConfigDirChanged(const QString& path);

private:
    void initInterface();
    void initPropertySubscription();
    void ensureConfigWatcher();
    /// 写 config 后通知 daemon 重载
    bool reloadConfig();
    /// 自身写盘抑制窗口（1s），防 config 目录监听自触发
    bool isSelfConfigWrite() const;
    /// 读全量 IMList
    Fcitx4ImItemList readImList();
    /// IMList 行 → ImEntry（派生 iconName / isKeyboard）
    ImEntry toImEntry(const Fcitx4ImItem& item) const;

private:
    QDBusServiceWatcher* m_watcher = nullptr;
    Fcitx4InputMethodProxy* m_proxy = nullptr;
    QFileSystemWatcher* m_configWatcher = nullptr;
    Fcitx4ConfigFile m_configFile;
    /// 自身写盘时间戳（epoch ms）
    qint64 m_configSelfWriteMs = 0;

    static const QString FCITX4_SERVICE;
    static const QString FCITX4_PATH;
    static const QString FCITX4_IFACE;
    static const QString FCITX4_PROP_IMLIST;
    static const QString FCITX4_PROP_CURRENTIM;
};
