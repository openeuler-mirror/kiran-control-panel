/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#pragma once

#include <QLabel>
#include <QList>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <NetworkManagerQt/Device>

class DeviceList : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceList(QWidget *parent = nullptr);

    DeviceList(NetworkManager::Device::Type type, QWidget *parent = nullptr);
    ~DeviceList();

    void init(NetworkManager::Device::Type type);

    QWidget *itemWidget(int row);
    QList<QWidget *> itemWidgetList();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initUI();
    void initConnect();

    void addWidget(QWidget *widget);
    void removeWidget(QWidget *widget);

private slots:
    void addDevice(const QString &devicePath);
    void removeDevice(const QString &devicePath);

private:
    QStringList m_managedDevicePaths;
    int m_deviceType = NetworkManager::Device::UnknownType;

    QScrollArea *m_scrollArea = nullptr;
    QVBoxLayout *m_verticalLayout = nullptr;
    QWidget *m_scrollAreaWidgetContents = nullptr;
    QVBoxLayout *m_widgetContentsLayout = nullptr;
    QList<QWidget *> m_itemWidgetList;
    QLabel *m_title = nullptr;
    QWidget *m_titleWidget = nullptr;
    QSpacerItem *m_horizontalSpacer = nullptr;
    QHBoxLayout *m_horizontalLayout = nullptr;
    QSpacerItem *m_verticalSpacer = nullptr;
};
