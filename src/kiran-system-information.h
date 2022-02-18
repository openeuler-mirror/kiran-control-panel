/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#ifndef KIRANSYSTEMINFORMATION_H
#define KIRANSYSTEMINFORMATION_H

#include <QListWidgetItem>
#include <QStackedWidget>
#include <QWidget>
#include "custom-widgets/information-list-item.h"

namespace Ui
{
class kiranSystemInformation;
}

class kiranSystemInformation : public QWidget
{
    Q_OBJECT

public:
    explicit kiranSystemInformation(QWidget* parent = 0);
    ~kiranSystemInformation();

private:
    void initUI();
    InformationListItem* createInformationItem(const QString text, const QString iconPath);
    QWidget* createSystemInfoPage(void);
    QWidget* createHardwareInfoPage(void);

signals:
    void sigClose();

public slots:
    void changeWidgetWhenItemClicked(QListWidgetItem* currentItem);

private:
    enum
    {
        pageSystemInfo,
        PageHardwareInfo
    };
    enum
    {
        itemSystemInfo,
        itemHardwareInfo
    };
    Ui::kiranSystemInformation* ui;
    QStackedWidget* stackedWidget;

    InformationListItem* systemInfomationItem;
    InformationListItem* hardwareInformationItem;

    QWidget* systemInfoWidget;
    QWidget* hardwareInfoWidget;
};

#endif  // KIRANSYSTEMINFORMATION_H
