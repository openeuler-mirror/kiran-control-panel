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
#ifndef TRAYPAGE_H
#define TRAYPAGE_H

#include <QComboBox>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
class ConnectionLists;
class TrayPage : public QWidget
{
    Q_OBJECT

public:
    explicit TrayPage(QWidget *parent = nullptr);
    ~TrayPage();

    void init();
    void initUI();
    void initConnection();
    void setMultiDeviceWidgetVisible(bool visible);
    void setDeviceLabel(const QString &label);

public:
    QVBoxLayout *m_verticalLayout;
    QWidget *m_multiDevicewidget;
    QVBoxLayout *m_verticalDeviceWidgetLayout;
    QLabel *m_deviceLabel;
    QComboBox *m_deviceComboBox;
    QScrollArea *m_scrollArea;
    QWidget *m_scrollAreaWidgetContents;
    QVBoxLayout *m_verticalScrollAreaWidgetContentsLayout;
    ConnectionLists *m_connectionLists;

private:
};

#endif  // TRAYPAGE_H
