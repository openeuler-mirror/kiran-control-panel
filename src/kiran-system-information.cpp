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

#include "kiran-system-information.h"
#include <kiran-log/qt5-log-i.h>
#include <QDesktopWidget>
#include <QPainter>
#include <QVBoxLayout>
#include "pages/license-information/license-information.h"
#include "ui_kiran-system-information.h"

#ifdef DISABLE_KIRANWIDGETS
kiranSystemInformation::kiranSystemInformation(QWidget* parent) : QWidget(parent),
                                                                  ui(new Ui::kiranSystemInformation)
{
    ui->setupUi(this);
    setWindowTitle(tr("kiran-system-imformation"));
    setWindowIcon(QIcon(":/images/kylin-about.png"));
#else
kiranSystemInformation::kiranSystemInformation(QWidget* parent) : KiranTitlebarWindow(parent),
                                                                  ui(new Ui::kiranSystemInformation)
{
    ui->setupUi(getWindowContentWidget());
    setTitle(tr("kiran-system-imformation"));
    setIcon(QIcon(":/images/kylin-about.png"));
#endif
    initUI();
}

kiranSystemInformation::~kiranSystemInformation()
{
    delete ui;
}

QSize kiranSystemInformation::sizeHint() const
{
    /*根据系统分辨率设置窗口大小*/
    return QSize(670, 730);
}

void kiranSystemInformation::paintEvent(QPaintEvent* painEvent)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(painEvent);
}

void kiranSystemInformation::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    ui->scrollAreaWidgetContents->setLayout(mainLayout);

    LicenseInformation* licenseInformation = new LicenseInformation(this);
    mainLayout->addWidget(licenseInformation);
}
