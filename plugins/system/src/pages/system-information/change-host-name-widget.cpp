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

#include "change-host-name-widget.h"
#include "dbus-wrapper/system-info-dbus.h"
#include "ui_change-host-name-widget.h"

#include <QDesktopWidget>
#include <QMessageBox>

#include <kiranwidgets-qt5/kiran-message-box.h>
#include <kiranwidgets-qt5/kiran-style-public-define.h>
#include <kiranwidgets-qt5/widget-property-helper.h>

ChangeHostNameWidget::ChangeHostNameWidget() : KiranTitlebarWindow(),
                                               ui(new Ui::ChangeHostNameWidget)
{
    ui->setupUi(getWindowContentWidget());
    setButtonHints(TitlebarMinimizeButtonHint | TitlebarCloseButtonHint);
    setContentWrapperMarginBottom(0);
    setResizeable(false);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save, Kiran::BUTTON_Default);
    initUI();
    connect(ui->btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btn_save, SIGNAL(clicked()), this, SLOT(setNewHostName()));
    connect(ui->lineEdit_input_name, SIGNAL(textChanged(QString)), SLOT(setInputEditStatus()));
}

ChangeHostNameWidget::~ChangeHostNameWidget()
{
    delete ui;
}

bool ChangeHostNameWidget::getLineEditStatus()
{
    if (ui->lineEdit_input_name->text() != NULL)
        return true;
    else
        return false;
}

void ChangeHostNameWidget::initUI()
{
    setTitle(tr("Host Name"));
    setIcon(QIcon(":/images/kylin-about.png"));

    ui->btn_save->setEnabled(false);

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    this->move(screenGeometry.x() + (screenGeometry.width() - this->width()) / 2,
               screenGeometry.y() + (screenGeometry.height() - this->height()) / 2);
}

void ChangeHostNameWidget::setNewHostName()
{
    QString newName = ui->lineEdit_input_name->text();
    if (!SystemInfoDBus::setHostName(newName))  // 调用Dbus接口不成功，弹出警告窗口
    {
        emit sigChangeNameSuccessful(false, newName);
        QString boxTitle = QString(tr("Warning"));
        QString boxText = QString(tr("Change host name failed! Please check the Dbus service!"));

        KiranMessageBox::KiranStandardButton button = KiranMessageBox::message(nullptr, boxTitle, boxText, KiranMessageBox::Ok);
        if (button == KiranMessageBox::Ok)
        {
            return;
        }
    }
    else
    {
        emit sigChangeNameSuccessful(true, newName);
        this->close();
    }
}

void ChangeHostNameWidget::setInputEditStatus()
{
    if (ui->lineEdit_input_name->text().isEmpty())
    {
        ui->btn_save->setEnabled(false);
        ui->btn_save->setStyleSheet("QPushButton#btn_save { color: rgba(255,255,255,0.3);}");
    }
    else
    {
        ui->btn_save->setEnabled(true);
        ui->btn_save->setStyleSheet("QPushButton#btn_save { color:#ffffff;}");
    }
}
