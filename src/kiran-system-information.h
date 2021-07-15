/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http: //www.gnu.org/licenses/>.
 */

#ifndef KIRANSYSTEMINFORMATION_H
#define KIRANSYSTEMINFORMATION_H

#include <QWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include "information-list-item.h"

namespace Ui {
class kiranSystemInformation;
}

class kiranSystemInformation : public QWidget
{
    Q_OBJECT

public:
    explicit kiranSystemInformation(QWidget *parent = 0);
    ~kiranSystemInformation();

private:
    void initUI();
    InformationListItem *createInformationItem(const QString text , const QString iconPath);
    QWidget* createSystemInfoPage(void);
    QWidget* createHardwareInfoPage(void);

signals:
    void sigClose();

public slots:
    void changeWidgetWhenItemClicked(QListWidgetItem* currentItem);

private:
    enum {pageSystemInfo, PageHardwareInfo};
    enum {itemSystemInfo, itemHardwareInfo};
    Ui::kiranSystemInformation *ui;
    QStackedWidget *stackedWidget;

    InformationListItem* systemInfomationItem;
    InformationListItem* hardwareInformationItem;

    QWidget* systemInfoWidget;
    QWidget* hardwareInfoWidget;

};

#endif // KIRANSYSTEMINFORMATION_H
