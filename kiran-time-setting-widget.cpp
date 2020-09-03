#include "kiran-time-setting-widget.h"
#include "ui_kiran-time-setting-widget.h"

#include <QStyleOption>
#include <QPainter>
#include <QListView>
#include <QDateTime>

KiranTimeSettingWidget::KiranTimeSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranTimeSettingWidget)
{
    ui->setupUi(this);
    initUI();
}

KiranTimeSettingWidget::~KiranTimeSettingWidget()
{
    delete ui;
}

void KiranTimeSettingWidget::reset()
{
    QTime currentTime = QTime::currentTime();

    ui->combo_hour->setCurrentIndex(currentTime.hour());
    ui->combo_minute->setCurrentIndex(currentTime.minute());
    ui->combo_second->setCurrentIndex(currentTime.second());
}

QTime KiranTimeSettingWidget::getTime()
{
    QTime time;
    int hour = ui->combo_hour->currentData().toInt();
    int minute = ui->combo_minute->currentData().toInt();
    int second = ui->combo_second->currentData().toInt();

    time.setHMS(hour,minute,second);
    return time;
}

void KiranTimeSettingWidget::initUI()
{
    QTime currentTime = QTime::currentTime();
    QListView* listview = nullptr;

    ///时
    listview = new QListView(ui->combo_hour);
    listview->setUniformItemSizes(true);
    ui->combo_hour->setView(listview);
    ui->combo_hour->view()->window()->setAttribute(Qt::WA_TranslucentBackground);
    for(int i=0;i<=23;i++){
        ui->combo_hour->addItem(QString("%1时").arg(i,2,10,QChar('0')),i);
        ui->combo_hour->setItemData(i,QSize(-1,40),Qt::SizeHintRole);
    }
    ui->combo_hour->setCurrentIndex(currentTime.hour());

    ///分
    listview = new QListView(ui->combo_minute);
    listview->setUniformItemSizes(true);
    ui->combo_minute->setView(listview);
    ui->combo_minute->view()->window()->setAttribute(Qt::WA_TranslucentBackground);
    for(int i=0;i<=59;i++){
        ui->combo_minute->addItem(QString("%1分").arg(i,2,10,QChar('0')),i);
        ui->combo_minute->setItemData(i,QSize(-1,40),Qt::SizeHintRole);
    }
    ui->combo_minute->setCurrentIndex(currentTime.minute());

    ///秒
    listview = new QListView(ui->combo_second);
    listview->setUniformItemSizes(true);
    ui->combo_second->setView(listview);
    ui->combo_second->view()->window()->setAttribute(Qt::WA_TranslucentBackground);
    for(int i=0;i<=59;i++){
        ui->combo_second->addItem(QString("%1秒").arg(i,2,10,QChar('0')),i);
        ui->combo_second->setItemData(i,QSize(-1,40),Qt::SizeHintRole);
    }
    ui->combo_second->setCurrentIndex(currentTime.second());
}

void KiranTimeSettingWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QWidget::paintEvent(event);
}
