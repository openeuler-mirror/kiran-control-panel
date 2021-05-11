/**
 * @file kiran-cpanel-mouse-widget.cpp
 * @brief  加载鼠标、触摸板设置页面
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#include "kiran-cpanel-mouse-widget.h"
#include "ui_kiran-cpanel-mouse-widget.h"
#include "pages/page-mouse/mouse-settings.h"
#include "pages/page-touchpad/touchpad-settings.h"
#include "general-functions/general-function-class.h"

#include <QMouseEvent>
#include <QSlider>
#include <QStackedWidget>
#include <QDebug>

using namespace std;

KiranCPanelMouseWidget::KiranCPanelMouseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranCPanelMouseWidget)
{
    ui->setupUi(this);
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),
            this,SLOT(setCurrentPageWhenItemClicked(QListWidgetItem*)));
}

KiranCPanelMouseWidget::~KiranCPanelMouseWidget()
{
    delete ui;
}

/**
 * @brief 初始化主窗口，插入鼠标、触摸板设置页面
 * @return true:连接Dbus服务成功
 *         false:连接Dbus服务失败
 */
bool KiranCPanelMouseWidget::initUI()
{
    mouseSettingsPage = new MouseSettings;
    touchPadSettingsPage = new TouchPadSettings;
    if(!mouseSettingsPage->initUI() || !touchPadSettingsPage->initUI())
    {
        qDebug() << "connect faild" << endl;
        return false;
    }
    ui->listWidget->resize(QSize(282,this->height()));
    ui->listWidget->setIconSize(QSize(16,16));

    addSidebarItem(tr("Mouse Settings"),":/images/mouse.svg");
    addSidebarItem(tr("TouchPad Settings"),":/images/touchpad.svg");
    ui->listWidget->setCurrentRow(ITEM_MOUSE);

    QVBoxLayout *layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->addWidget(mouseSettingsPage);
    m_stackedWidget->addWidget(touchPadSettingsPage);

    m_stackedWidget->setCurrentIndex(PAGE_MOUSE);

    layout->addWidget(m_stackedWidget);
    return true;
}

void KiranCPanelMouseWidget::addSidebarItem(QString text, QString icon)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setIcon(QIcon(icon));
    item->setText(text);
    ui->listWidget->addItem(item);
}

void KiranCPanelMouseWidget::setCurrentPageWhenItemClicked(QListWidgetItem *item)
{
    int itemNum = ui->listWidget->row(item);
    if(itemNum == ITEM_MOUSE)
    {
        m_stackedWidget->setCurrentWidget(mouseSettingsPage);
    }
    else if(itemNum == ITEM_TOUCHPAD)
    {
        m_stackedWidget->setCurrentWidget(touchPadSettingsPage);
    }
}



