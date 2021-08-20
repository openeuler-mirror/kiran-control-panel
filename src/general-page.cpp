#include "general-page.h"
#include "ui_general-page.h"

GeneralPage::GeneralPage(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::GeneralPage)
{
    ui->setupUi(this);
    initUI();
}

GeneralPage::~GeneralPage()
{
    delete ui;
}

QSize GeneralPage::sizeHint() const
{
    return QSize(636, 670);
}

void GeneralPage::initUI()
{
    ui->lineEdit_key->setPlaceholderText(tr("Enter characters to test the settings"));
}
