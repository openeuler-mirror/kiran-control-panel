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

    void    setUserDBusObjectPath(const QString &objPath);
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
    bool                   m_isSelected;
    bool                   m_isCreateAccountItem;
    bool                   m_isLocked;
    QString                m_objectPath;
};

#endif  // ACCOUNTITEMWIDGET_H
