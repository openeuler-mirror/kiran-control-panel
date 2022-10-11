/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
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

#ifndef ACCOUNTITEMWIDGET_H
#define ACCOUNTITEMWIDGET_H

#include <QDBusObjectPath>
#include <QVariant>
#include <QWidget>

namespace Ui
{
class AccountItemWidget;
}

class QDBusObjectPath;

class AccountsUser;

class AccountItemWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isSelected READ isSelected WRITE setSelected NOTIFY isSelectedChanged)
    Q_PROPERTY(bool isLocked READ isLocked WRITE setIsLocked)

public:
    enum AccountStatus
    {
        STATUS_ENABLE,
        STATUS_DISABLE
    };

    Q_ENUM(AccountStatus);

public:
    explicit AccountItemWidget(QWidget *paren = nullptr, bool isCreateAccountItem = false);
    ~AccountItemWidget();

    bool isSelected() const;
    bool isCreateAccountItem() const;
    bool isLocked() const;

    void setUserDBusObjectPath(const QString &objPath);
    QString getUserDBusObjectPath();

    void updateInfo();

public slots:
    void setSelected(bool isSelected);
    void setIsLocked(bool isLocked);

signals:
    void isSelectedChanged(bool isSelected);

protected:
    void updateStatusDesc();

private:
    Ui::AccountItemWidget *ui;
    bool m_isSelected;
    bool m_isCreateAccountItem;
    bool m_isLocked;
    QString m_objectPath;
};

#endif  // ACCOUNTITEMWIDGET_H
