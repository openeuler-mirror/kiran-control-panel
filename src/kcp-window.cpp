#include "kcp-window.h"
#include <QListWidgetItem>
#include "ui_kcp-window.h"
KCPWindow::KCPWindow(QWidget* parent) : KiranTitlebarWindow(parent),
                                        ui(new Ui::KCPWindow)
{
    ui->setupUi(getWindowContentWidget());
    addSidebarItem(tr("General Option"), ":/images/kck-page-generation.svg");
    addSidebarItem(tr("Keyboard Layout"), ":/images/kck-page-layout.png");
}

KCPWindow::~KCPWindow()
{
    delete ui;
}

void KCPWindow::addSidebarItem(QString text, QString icon)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setIcon(QIcon(icon));
    item->setText(text);
    ui->listWidget->addItem(item);
}
