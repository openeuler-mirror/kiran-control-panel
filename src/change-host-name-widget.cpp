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

#include "change-host-name-widget.h"
#include "ui_change-host-name-widget.h"
#include "system-info-dbus.h"
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <kiranwidgets-qt5/kiran-style-public-define.h>
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

bool ChangeHostNameWidget::getLineEditStatus()
{
    if(ui->lineEdit_input_name->text() != NULL)
        return true;
    else
        return false;
}

void ChangeHostNameWidget::initUI()
{
    setTitle(tr("Host Name"));
    setIcon(QIcon(":/images/kylin-about.png"));
    setButtonHints(TitlebarMinimizeButtonHint|TitlebarCloseButtonHint);
    setContentWrapperMarginBottom(0);
    setResizeable(false);
    ui->btn_save->setEnabled(false);

    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save,Kiran::BUTTON_Default);

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
