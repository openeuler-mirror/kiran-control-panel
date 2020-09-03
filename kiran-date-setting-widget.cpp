#include "kiran-date-setting-widget.h"
#include "ui_kiran-date-setting-widget.h"

#include <QStyleOption>
#include <QPainter>
#include <QListView>
#include <QDateTime>
#include <QStyledItemDelegate>

///NOTE:闰年二月29天
static int DayCountOfMonth[12]={31,28,31,30,31,30,31,31,30,31,30,31};


KiranDateSettingWidget::KiranDateSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranDateSettingWidget)
{
    ui->setupUi(this);
    initUI();
}

KiranDateSettingWidget::~KiranDateSettingWidget()
{
    delete ui;
}

void KiranDateSettingWidget::reset()
{
    initUI();
}

QDate KiranDateSettingWidget::getDate()
{
    QDate date;
    int year = ui->combo_year->currentData().toInt();
    int month = ui->combo_month->currentData().toInt();
    int day = ui->combo_day->currentData().toInt();
    date.setDate(year,month,day);
    return date;
}

void KiranDateSettingWidget::initUI()
{
     QDate currentDate = QDate::currentDate();


    auto getDayCountOfMonthFunc = [this](int year,int month){
        if(QDate::isLeapYear(year)&&month==2){
            return 29;
        }else{
            return DayCountOfMonth[month-1];
        }
    };


    QListView* listview = nullptr;
    ///年
    listview = new QListView(ui->combo_year);
    listview->setUniformItemSizes(true);
    ui->combo_year->setView(listview);
    ui->combo_year->view()->window()->setAttribute(Qt::WA_TranslucentBackground);
    ui->combo_year->clear();
    int currentYear = currentDate.year();
    for(int i=currentYear-50;i<currentYear+50;i++){
        ui->combo_year->addItem(QString::number(i)+QString("年"),i);
        ///设置QComboBox中ListView item高度
        ui->combo_year->setItemData(ui->combo_year->count()-1,
                                    QSize(-1,40),
                                    Qt::SizeHintRole);
    }
    ui->combo_year->setCurrentIndex(50);
    connect(ui->combo_year,QOverload<int>::of(&QComboBox::currentIndexChanged),[this](int idx){
        QVariant userRoleData;
        int currentSelectYear,currentSelectMonth,currentSelectDay;

        userRoleData = ui->combo_year->itemData(idx);
        currentSelectYear = userRoleData.toInt();
        userRoleData = ui->combo_month->itemData(ui->combo_month->currentIndex());
        currentSelectMonth = userRoleData.toInt();
        userRoleData = ui->combo_day->itemData(ui->combo_day->currentIndex());
        currentSelectDay = userRoleData.toInt();

        ///年的切换只影响到二月的天数
        if( currentSelectMonth!=2 ){
            return;
        }
        ///闰年二月且找不到29日时添加29日。不是闰年二月并找到29日删除29日
        if( QDate::isLeapYear(currentSelectYear) && (ui->combo_day->findData(29)==-1) ){
            ui->combo_day->addItem(QString("%1日").arg(29,2,10,QChar('0')),29);
        }else if( !QDate::isLeapYear(currentSelectYear) && (ui->combo_day->findData(29)!=-1) ){
            ui->combo_day->removeItem(28);
        }
    });

    ///月
    listview = new QListView(ui->combo_month);
    listview->setUniformItemSizes(true);
    ui->combo_month->setView(listview);
    ui->combo_month->view()->window()->setAttribute(Qt::WA_TranslucentBackground);
    ui->combo_month->clear();
    for(int i=1;i<=12;i++){
        ui->combo_month->addItem(QString("%1月").arg(i,2,10,QChar('0')),i);
        ui->combo_month->setItemData(i-1,QSize(-1,40),Qt::SizeHintRole);
    }
    ui->combo_month->setCurrentIndex(currentDate.month()-1);
    connect(ui->combo_month,QOverload<int>::of(&QComboBox::currentIndexChanged),[this](int idx){
        QVariant userRoleData;
        int currentSelectYear,currentSelectMonth;
        int currentDayCount,dayCount;
        int currentSelectDay;

        userRoleData = ui->combo_year->itemData(ui->combo_year->currentIndex());
        currentSelectYear = userRoleData.toInt();

        userRoleData = ui->combo_month->itemData(idx);
        currentSelectMonth = userRoleData.toInt();

        ///获取现在显示天数和该月的天数
        currentDayCount = (currentSelectMonth==2&&QDate::isLeapYear(currentSelectYear))?29:DayCountOfMonth[currentSelectMonth-1];
        dayCount = ui->combo_day->count();
        currentSelectDay = ui->combo_day->currentData().toInt();

        if(currentDayCount!=dayCount){
            ui->combo_day->clear();
            for(int i=1;i<=currentDayCount;i++){
                ui->combo_day->addItem(QString("%1日").arg(i,2,10,QChar('0')),i);
                ui->combo_day->setItemData(i-1,QSize(-1,40),Qt::SizeHintRole);
            }
            int idx = ui->combo_day->findData(currentSelectDay);
            if(idx!=-1){
                ui->combo_day->setCurrentIndex(idx);
            }else{
                ui->combo_day->setCurrentIndex(0);
            }
        }
    });

    ///日
    listview = new QListView(ui->combo_day);
    listview->setUniformItemSizes(true);
    ui->combo_day->setView(listview);
    ui->combo_day->view()->window()->setAttribute(Qt::WA_TranslucentBackground);
    ui->combo_day->clear();
    int dayCount = getDayCountOfMonthFunc(currentDate.year(),
                                          currentDate.month());
    for(int i=1;i<=dayCount;i++){
        ui->combo_day->addItem(QString("%1日").arg(i,2,10,QChar('0')),i);
        ui->combo_day->setItemData(i-1,QSize(-1,40),Qt::SizeHintRole);
    }
    ui->combo_day->setCurrentIndex(currentDate.day()-1);
}

void KiranDateSettingWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QWidget::paintEvent(event);
}
