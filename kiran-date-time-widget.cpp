#include "kiran-date-time-widget.h"
#include "ui_kiran-date-time-widget.h"
#include "tab-item.h"
#include "kiran-timedate-global-data.h"

#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QTimerEvent>

enum KiranDateTimeStackPageEnum{
    PAGE_TIMEZONE_SETTING,
    PAGE_DATETIME_SETTING
};

KiranDateTimeWidget::KiranDateTimeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KiranDateTimeWidget)
    , m_updateTimer(0)
{
    ui->setupUi(this);
    initUI();
    m_updateTimer = startTimer(1000);
}

KiranDateTimeWidget::~KiranDateTimeWidget()
{
    delete ui;
}

void KiranDateTimeWidget::initUI()
{
    QListWidgetItem* item;
    TabItem* tabItem;
    KiranTimeDateGlobalData* globalData = KiranTimeDateGlobalData::instance();

    setWindowTitle("日期和时间管理");

    QIcon icon = QIcon::fromTheme("preferences-system-time");
    setWindowIcon(icon);

    /// 更改时区
    item = new QListWidgetItem(ui->tabList);
    tabItem = new TabItem(ui->tabList);
    tabItem->setObjectName("tab_timeZone");
    tabItem->setText("更改时区");
    ui->tabList->addItem(item);
    ui->tabList->setItemWidget(item,tabItem);
    //更改时区选中触发
    connect(tabItem,&TabItem::isSelectedChanged,[this](bool isSelected){
        //TODO:切换判断是否修改
        if(isSelected){
            ui->stack->setCurrentIndex(PAGE_TIMEZONE_SETTING);
            ui->timezone->reset();
        }
    });

    /// 更改时间
    //手动设置时区选中触发
    item = new QListWidgetItem(ui->tabList);
    tabItem = new TabItem(ui->tabList);
    tabItem->setObjectName("tab_setTimeManually");
    tabItem->setText(tr("手动设置时间"));
    ui->tabList->addItem(item);
    ui->tabList->setItemWidget(item,tabItem);
    connect(tabItem,&TabItem::isSelectedChanged,[this](bool isSelected){
        if(isSelected){
            ui->stack->setCurrentIndex(PAGE_DATETIME_SETTING);
            ui->widget_dateSetting->reset();
            ui->widget_timeSetting->reset();
        }
    });

    /// 侧边栏切换
    //QListWidget的当前行切换设置与之关联的TabItem isSelected属性 更新stylesheet
    connect(ui->tabList,&QListWidget::currentRowChanged,[this](int currentRow){
        QListWidgetItem* currentItem = ui->tabList->item(currentRow);
        TabItem* currentTabItem = dynamic_cast<TabItem*>(ui->tabList->itemWidget(currentItem));
        currentTabItem->setSelected(true);
        for(int i=0;i<ui->tabList->count();i++){
            QListWidgetItem* item = ui->tabList->item(i);
            if(item!=currentItem){
                TabItem* tabItem = dynamic_cast<TabItem*>(ui->tabList->itemWidget(item));
                if(tabItem!=nullptr)
                    tabItem->setSelected(false);
            }
        }
    });

    /// 自动同步
    //自动同步开关的触发，设置"手动设置时间"标签的enable,判断当前页并切换
    connect(ui->checkbox_autoSync,&QCheckBox::stateChanged,[this](int state){
        bool enable = state!=0;
        QListWidgetItem* dateTimeItem = ui->tabList->item(PAGE_DATETIME_SETTING);
        QWidget* dateTimeItemWidget = ui->tabList->itemWidget(dateTimeItem);

        if( enable!=KiranTimeDateGlobalData::instance()->systemNTP() ){
            QPair<bool,QString> setNtpRes;
            setNtpRes = ComUnikylinKiranSystemDaemonTimeDateInterface::instance()->SyncSetNTP(enable);
            if(!setNtpRes.first){
                qInfo() << "SetNTP failed," << setNtpRes.second;
                ui->checkbox_autoSync->setCheckState(enable?Qt::Unchecked:Qt::Checked);
                return;
            }
        }
        if(enable){
            dateTimeItem->setFlags(dateTimeItem->flags()&(~Qt::ItemIsEnabled));
            dateTimeItemWidget->setEnabled(false);
            TabItem* tabItem = dynamic_cast<TabItem*>(dateTimeItemWidget);
            if(ui->tabList->currentRow()==PAGE_DATETIME_SETTING){
                ui->tabList->setCurrentRow(PAGE_TIMEZONE_SETTING);
            }
        }else{
            dateTimeItem->setFlags(dateTimeItem->flags()|Qt::ItemIsEnabled);
            dateTimeItemWidget->setEnabled(true);
        }
    });
    connect(globalData,&KiranTimeDateGlobalData::systemNTPChanged,[this](bool ntp){
        ui->checkbox_autoSync->setCheckState(ntp?Qt::Checked:Qt::Unchecked);
    });
    connect(globalData,&KiranTimeDateGlobalData::systemCanNTPChanged,[this](bool can_ntp){
        ui->checkbox_autoSync->setEnabled(can_ntp);
    });
    //获取ntp是否可开启
    bool can_ntp = globalData->systemCanNTP();
    ui->checkbox_autoSync->setChecked(false);
    ui->checkbox_autoSync->setEnabled(can_ntp);
    if(can_ntp){
        //时钟同步状态
        bool ntpStatus = globalData->systemNTP();
        ui->checkbox_autoSync->setChecked(ntpStatus);
    }

    ///侧边栏时区显示


    /// 保存
    connect(ui->btn_save,&QPushButton::clicked,[this](bool checked){
        bool bRes = true;
        QString error;
        if(ui->tabList->currentRow()==PAGE_TIMEZONE_SETTING){
            bRes = ui->timezone->save();
        }else if(ui->tabList->currentRow()==PAGE_DATETIME_SETTING){
            QDateTime dateTime;
            qint64 microsecondsSinceEpoch;
            dateTime.setDate(ui->widget_dateSetting->getDate());
            dateTime.setTime(ui->widget_timeSetting->getTime());
            microsecondsSinceEpoch = dateTime.toMSecsSinceEpoch()*1000;
            QPair<bool,QString> res = ComUnikylinKiranSystemDaemonTimeDateInterface::instance()->SyncSetTime(microsecondsSinceEpoch,false);
            bRes = res.first;
        }
    });

    /// 重置
    connect(ui->btn_reset,&QPushButton::clicked,[this](bool checked){
        if(ui->tabList->currentRow()==PAGE_TIMEZONE_SETTING){
            ui->timezone->reset();
        }else{
            ui->widget_dateSetting->reset();
            ui->widget_timeSetting->reset();
        }
    });

    ///更新侧边栏时间文本和时区显示
    updateTimeZoneLabel();
    connect(globalData,&KiranTimeDateGlobalData::systemTimeZoneChanged,[this](QString timeZone){
        updateTimeZoneLabel();
    });

    updateTimeLabel();

    ///设置默认页
    ui->tabList->setCurrentRow(0);
}

void KiranDateTimeWidget::updateTimeLabel()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString displayDateTime = currentDateTime.toString("yyyy-MM-dd HH:mm:ss ddd");
    ui->label_dateTime->setText(displayDateTime);
}

void KiranDateTimeWidget::updateTimeZoneLabel()
{
    ZoneInfo zoneInfo;
    KiranTimeDateGlobalData* globalData = KiranTimeDateGlobalData::instance();
    QString currentTimeZoneID = globalData->systemTimeZone();

    if(globalData->findZoneInfoByZoneID(currentTimeZoneID,zoneInfo)){
        QString city = zoneInfo.zone_city;
        QStringList splitRes = city.split('/');

        city = splitRes.last();
        int hour = qAbs(zoneInfo.zone_utc)/3600;
        int minute = (qAbs(zoneInfo.zone_utc)-hour*3600)/60;
        QString utc = QString("UTC%1%2:%3")
                .arg(zoneInfo.zone_utc>=0?"+":"-")
                .arg(hour,2,10,QChar('0'))
                .arg(minute,2,10,QChar('0'));
        QString displayText = QString("%1时间(%2)").arg(city).arg(utc);
        ui->label_utc->setText(displayText);
    }else{
        ui->label_utc->setText("???");
    }
}

void KiranDateTimeWidget::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==m_updateTimer){
        updateTimeLabel();
    }else{
        QWidget::timerEvent(event);
    }
}
