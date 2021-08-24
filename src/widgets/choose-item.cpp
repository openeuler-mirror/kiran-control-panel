#include "choose-item.h"
#include "ui_choose-item.h"

ChooseItem::ChooseItem(QString countryName, QWidget *parent) : QWidget(parent),
                                                               ui(new Ui::ChooseItem)
{
    ui->setupUi(this);
    m_countryName = countryName;
    connect(ui->btn_delete, &QToolButton::clicked,
            [this] {
                emit deleteLayout();
            });
}

ChooseItem::~ChooseItem()
{
    delete ui;
}

void ChooseItem::setSelected(bool isSelected)
{
    ui->label_selected->setVisible(isSelected);
}

void ChooseItem::setDeleteMode(bool isDeleteMode)
{
    ui->btn_delete->setVisible(isDeleteMode);
}

void ChooseItem::initUI()
{
    ui->label_country_name->setText(m_countryName);
    ui->btn_delete->hide();
    ui->label_selected->hide();
}

void ChooseItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}
