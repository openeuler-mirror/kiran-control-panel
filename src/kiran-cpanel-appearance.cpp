#include "kiran-cpanel-appearance.h"
#include "ui_kiran-cpanel-appearance.h"
#include <QListWidgetItem>

KiranCpanelAppearance::KiranCpanelAppearance(QWidget *parent) :
    KiranTitlebarWindow(parent),
    ui(new Ui::KiranCpanelAppearance)
{
    ui->setupUi(getWindowContentWidget());
    initUI();

    connect(ui->listWidget,&KiranSidebarWidget::itemClicked,
            [=](QListWidgetItem *item) {
        int row = ui->listWidget->row(item);
        ui->stackedWidget->setCurrentIndex(row);
        //TODO:后续根据需求确定是否切换列表项时需要指定第一页
        if(row == ITEM_BACKGROUND);
        else if(row == ITEM_THEME)
        {
            ui->page_theme->setPage(0);
        }
        else;
    });
}

KiranCpanelAppearance::~KiranCpanelAppearance()
{
    delete ui;
}

bool KiranCpanelAppearance::initUI()
{
    //TODO:connect dbus if faile return false;
    setIcon(QIcon(":/images/icon.png"));

    ui->listWidget->setIconSize(QSize(16,16));
    addSidebarItem(tr("Wallpaper Setting"),":/images/wallpaper.svg");
    addSidebarItem(tr("Theme Setting"),":/images/themes.svg");
    addSidebarItem(tr("Font Setting"), ":/images/themes.svg");

    ui->listWidget->setCurrentRow(0);
    return true;
}

void KiranCpanelAppearance::addSidebarItem(QString text, QString icon)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setIcon(QIcon(icon));
    item->setText(text);
    ui->listWidget->addItem(item);
}
