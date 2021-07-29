 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
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
