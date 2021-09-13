#include "main-window.h"
#include <QListWidgetItem>
#include "ui_main-window.h"

MainWindow::MainWindow(QWidget* parent) : KiranTitlebarWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(getWindowContentWidget());
    addSidebarItem(tr("General Option"), ":/images/kck-page-generation.svg");
    addSidebarItem(tr("Keyboard Layout"), ":/images/kck-page-layout.png");

    connect(ui->listWidget, &KiranSidebarWidget::currentRowChanged,
            [this](int currentRow) {
                if (currentRow == KCP_LIST_ITEM_GENERAL)
                    ui->stackedWidget->setCurrentWidget(ui->page_general);
                else
                    ui->stackedWidget->setCurrentWidget(ui->page_layout);
            });
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
