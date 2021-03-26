#include "account-itemwidget.h"
#include "ui_account-itemwidget.h"
#include "accounts-user-interface.h"

#include <QStyle>

AccountItemWidget::AccountItemWidget (QWidget *paren, bool isCreateAccountItem) :
        QWidget(paren),
        ui(new Ui::AccountItemWidget),
        m_isSelected(false),
        m_isCreateAccountItem(isCreateAccountItem),
        m_isLocked(false)
{
    ui->setupUi(this);
    if (m_isCreateAccountItem)
    {
        ui->avatar->setImage(":/images/add_icon.png");
        ui->label_account->setText(tr("Create new account"));
        ui->label_status->setVisible(false);
    }
    updateStatusDesc();
}

AccountItemWidget::~AccountItemWidget ()
{
    delete ui;
}

bool AccountItemWidget::isSelected () const
{
    return m_isSelected;
}

bool AccountItemWidget::isCreateAccountItem () const
{
    return m_isCreateAccountItem;
}

bool AccountItemWidget::isLocked () const
{
    return m_isLocked;
}

void AccountItemWidget::setUserDBusObjectPath (const QString &objPath)
{
    m_objectPath = objPath;
    updateInfo();
}

QString AccountItemWidget::getUserDBusObjectPath ()
{
    return m_objectPath;
}

void AccountItemWidget::updateInfo ()
{
    UserInterface userInterface(m_objectPath, QDBusConnection::systemBus());
    ui->label_account->setText(userInterface.user_name());
    setIsLocked(userInterface.locked());
    ui->avatar->setImage(userInterface.icon_file());
}

void AccountItemWidget::setSelected (bool isSelected)
{
    if (m_isSelected == isSelected)
    {
        return;
    }
    m_isSelected = isSelected;
    style()->polish(ui->indicator);
    style()->polish(ui->label_status);
    emit isSelectedChanged(m_isSelected);
}

void AccountItemWidget::setIsLocked (bool isLocked)
{
    if (m_isLocked == isLocked)
    {
        return;
    }
    m_isLocked = isLocked;
    updateStatusDesc();
    style()->polish(ui->label_status);
}

void AccountItemWidget::updateStatusDesc ()
{
    if (m_isLocked)
    {
        ui->label_status->setText(tr("disable"));
    }
    else
    {
        ui->label_status->setText(tr("enable"));
    }
}
