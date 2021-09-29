#include "main-window.h"
#include "ui_main-window.h"

MainWindow::MainWindow(QWidget* parent) : KiranTitlebarWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(getWindowContentWidget());
    addSidebarItem("Shortcut Setting", ":/images/search.svg");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addSidebarItem(QString text, QString icon)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setIcon(QIcon(icon));
    item->setText(text);
    ui->listWidget->addItem(item);
}
