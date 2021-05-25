/**
 * @file change-host-name-widget.c
 * @brief 修改用户名接口
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "change-host-name-widget.h"
#include "ui_change-host-name-widget.h"
#include "system-info-dbus.h"
#include <kiranwidgets-qt5/kiran-message-box.h>

#include <QDebug>
#include <QDesktopWidget>

ChangeHostNameWidget::ChangeHostNameWidget() :
    KiranTitlebarWindow(),
    ui(new Ui::ChangeHostNameWidget)
{
    ui->setupUi(getWindowContentWidget());
    initUI();
    connect(ui->btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btn_save, SIGNAL(clicked()), this, SLOT(setNewHostName()));
    connect(ui->lineEdit_input_name , SIGNAL(textChanged(QString)), SLOT(setInputEditStatus()));
}

ChangeHostNameWidget::~ChangeHostNameWidget()
{
    delete ui;
}

void ChangeHostNameWidget::initUI()
{
    setTitle(tr("Host Name"));
    setIcon(QIcon(":/images/kylin-about.png"));
    setButtonHints(TitlebarMinimizeButtonHint|TitlebarCloseButtonHint);
    setContentWrapperMarginBottom(0);
    setResizeable(false);
    ui->btn_save->setEnabled(false);

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    this->move(screenGeometry.x()+(screenGeometry.width()-this->width())/2,
           screenGeometry.y()+(screenGeometry.height()-this->height())/2);
}

void ChangeHostNameWidget::setNewHostName()
{
    QString newName = ui->lineEdit_input_name->text();
    if(!InfoDbus::SystemInfo::setHostName(newName)) // 调用Dbus接口不成功，弹出警告窗口
    {
        emit sigChangeNameSuccessful(false,newName);
        QString boxTitle = QString(tr("Warning"));
        QString boxText = QString(tr("Change host name failed! Please check the Dbus service!"));
        KiranMessageBox::KiranStandardButton button = KiranMessageBox::message(nullptr,boxTitle,boxText,KiranMessageBox::Ok);
        if(button == KiranMessageBox::Ok)
        {
            return;
        }
    }
    else
    {
        emit sigChangeNameSuccessful(true,newName);
        this->close();
    }
}

void ChangeHostNameWidget::setInputEditStatus()
{
    if(ui->lineEdit_input_name->text().isEmpty())
    {
        ui->btn_save->setEnabled(false);
        ui->btn_save->setStyleSheet("QPushButton#btn_save { color: rgba(255,255,255,0.3)}");

    }
    else
    {
        ui->btn_save->setEnabled(true);
        ui->btn_save->setStyleSheet("QPushButton#btn_save { color:#ffffff}");
    }
}
