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
#include <QString>
#include <QVector>
#include <QWidget>

class KiranSidebarItem;

namespace Ui
{
class Widget;
}

namespace Kiran
{
namespace Network
{
class EthernetPage;
class WirelessPage;
class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    QVector<QPair<QString, QString>> searchKeyEntries() const;
    bool jumpToSearchEntry(const QString &key);

signals:
    void sidebarChanged();

private:
    void init();

private slots:
    void updateSubPage();

private:
    Ui::Widget *ui;
};
}  // namespace Network
}  // namespace Kiran