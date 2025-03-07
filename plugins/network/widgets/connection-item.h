/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
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
#include <kiran-frame/kiran-frame.h>

class QMenu;
class QMouseEvent;
class QDBusPendingCall;
class QDBusPendingCallWatcher;
namespace Ui
{
class ConnectionItem;
}

namespace Kiran
{
namespace Network
{
class ConnectionItem : public KiranFrame
{
    Q_OBJECT

public:
    explicit ConnectionItem(QWidget* parent = 0);
    ~ConnectionItem();

    enum OpeartionType{
        OPERTION_ACTIVATE,
        OPERTION_DEACTIVATE,
        OPERTION_LAST
    };
    // 处理activateConnection/addAndActivateConnection/deactivateConnection阶段返回值
    void checkOpeartionResult(OpeartionType type, const QString& name, QDBusPendingCall& call);

signals:
    void requestDisconnect(QPrivateSignal);
    void requestEdit(QPrivateSignal);
    void requestActive(QPrivateSignal);

private slots:
    void processPendingCallFinished(QDBusPendingCallWatcher* watcher);

protected:
    // 设置连接名
    void setName(const QString& name);
    // 设置连接图标
    void setTypeIconVisible(bool visible);
    void setTypeIcon(const QString& iconName);
    // 设置是否可编辑
    void setEditable(bool editable);
    // 设置是否可断开
    void setDisconnectable(bool disconnectable);
    // 设置更多选项
    void setMoreOptionsEnable(bool enable);
    void setMoreOptions(QMenu* menu);
    // 更新激活状态，loading状态
    void updateStatus(bool isActivated, bool isLoading);
    // 发送操作通知
    void dbusNotifyFailed(OpeartionType type,const QString& connectionName,const QString& resason=QString());

private:
    void initUI();

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;

private:
    Ui::ConnectionItem* ui;
};
}  // namespace Network
}  // namespace Kiran
