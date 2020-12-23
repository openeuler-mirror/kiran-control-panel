#include "kiran-timedate-widget.h"
#include "ui_kiran-timedate-widget.h"
#include "tab-item.h"
#include "kiran-timedate-global-data.h"
#include "mask-widget.h"

#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QTimerEvent>
#include <QFontDatabase>
#include <QListWidgetItem>
#include <kiran-switch-button.h>
#include <style-property-helper.h>

enum KiranTimeDateStackPageEnum{
    PAGE_TIMEZONE_SETTING,
    PAGE_DATETIME_SETTING
};

#define KEY_FONT_NAME "fontName"

KiranTimeDateWidget::KiranTimeDateWidget(QWidget *parent)
    : KiranTitlebarWindow()
    , ui(new Ui::KiranTimeDateWidget)
    , m_updateTimer(0)
    , m_maskWidget(new MaskWidget(this))
{
    ui->setupUi(getWindowContentWidget());
    initUI();
    m_updateTimer = startTimer(1000);
}

KiranTimeDateWidget::~KiranTimeDateWidget()
{
    delete ui;
}

void KiranTimeDateWidget::initUI()
{
    KiranTimeDateGlobalData* globalData = KiranTimeDateGlobalData::instance();

    setTitle(tr("Time And Date Manager"));

    QIcon icon = QIcon::fromTheme("preferences-system-time");
    setIcon(icon);

    /// 显示时区和所在区域
    ui->label_utc->setContentsMargins(-1,24,-1,-1);
    ui->label_dateTime->setContentsMargins(-1,8,-1,-1);

    /// 时区列表上标签
    QMargins labelTimeZoneMargins = ui->label_timeZone->contentsMargins();
    labelTimeZoneMargins.setTop(24);
    labelTimeZoneMargins.setBottom(10);
    ui->label_timeZone->setContentsMargins(labelTimeZoneMargins);

    /// 设置时间日期上标签
    QMargins labelSetTimeMargins = ui->label_setTime->contentsMargins();
    labelSetTimeMargins.setTop(24);
    labelSetTimeMargins.setBottom(10);
    ui->label_setTime->setContentsMargins(labelSetTimeMargins);

    QMargins labelSetDateMargins = ui->label_setTime->contentsMargins();
    labelSetDateMargins.setTop(24);
    labelSetDateMargins.setBottom(10);
    ui->label_setDate->setContentsMargins(labelSetDateMargins);

    /* 初始化侧边栏Tab列表 */
    ui->tabList->setIconSize(QSize(16,16));
    /// 更改时区
    m_changeTimeZoneItem = new QListWidgetItem(ui->tabList);
    m_changeTimeZoneItem->setText(tr("Change Time Zone"));
    m_changeTimeZoneItem->setIcon(QIcon(":/images/time_zone.png"));
    ui->tabList->addItem(m_changeTimeZoneItem);

    /// 更改时间
    m_setTimeManuallyItem = new QListWidgetItem(ui->tabList);
    m_setTimeManuallyItem->setText(tr("Set Time Manually"));
    QIcon setTimeManualIcon;
    setTimeManualIcon.addPixmap(QPixmap(":/images/time.png"),QIcon::Normal);
    setTimeManualIcon.addPixmap(QPixmap(":/images/time_d.png"),QIcon::Disabled);
    m_setTimeManuallyItem->setIcon(setTimeManualIcon);
    ui->tabList->addItem(m_setTimeManuallyItem);

    /* 侧边栏切换处理 */
    connect(ui->tabList,&KiranSidebarWidget::itemSelectionChanged,[this](){
        QList<QListWidgetItem*> selecteds = ui->tabList->selectedItems();
        if(selecteds.size()!=1){
            qFatal("tabList: selecteds size != 1");
            return;
        }
        int row = ui->tabList->row(selecteds.at(0));
        if( row == PAGE_DATETIME_SETTING ){/* 切换到设置日期事件Tab,复位日期和时间 */
            ui->widget_setDate->reset();
            ui->widget_setTime->reset();
        }else if( row == PAGE_TIMEZONE_SETTING ){/* 切换到时区设置,当前选择的时区居中 */
            ui->timezone->reset();
        }
        ui->stack->setCurrentIndex(row);
    });

    /// 自动同步
    m_autoSyncSwitch = new KiranSwitchButton(this);
    ui->widget_autoSync->layout()->addWidget(m_autoSyncSwitch);

    //自动同步开关的触发，设置"手动设置时间"标签的enable,判断当前页并切换
    connect(m_autoSyncSwitch,&KiranSwitchButton::toggled,[this](bool checked){
        bool autoSyncEnable = checked;
        QListWidgetItem* dateTimeItem = ui->tabList->item(PAGE_DATETIME_SETTING);
        QWidget* dateTimeItemWidget = ui->tabList->itemWidget(dateTimeItem);

        if(autoSyncEnable != KiranTimeDateGlobalData::instance()->systemNTP() ){
            QPair<bool,QString> setNtpRes;

            setMaskWidgetVisible(true);
            setNtpRes = ComKylinsecKiranSystemDaemonTimeDateInterface::instance()->SyncSetNTP(autoSyncEnable);
            setMaskWidgetVisible(false);

            if(!setNtpRes.first){
                qWarning() << "SetNTP failed," << setNtpRes.second;
                m_autoSyncSwitch->setChecked(autoSyncEnable ? Qt::Unchecked : Qt::Checked);
                return;
            }
        }
        if(autoSyncEnable){
            dateTimeItem->setFlags(dateTimeItem->flags()&(~Qt::ItemIsEnabled));
            if(ui->tabList->currentRow()==PAGE_DATETIME_SETTING){
                ui->tabList->setCurrentRow(PAGE_TIMEZONE_SETTING);
            }
        }else{
            dateTimeItem->setFlags(dateTimeItem->flags()|Qt::ItemIsEnabled);
        }
    });
    connect(globalData,&KiranTimeDateGlobalData::systemNTPChanged,[this](bool ntp){
        m_autoSyncSwitch->setChecked(ntp?Qt::Checked:Qt::Unchecked);
    });
    connect(globalData,&KiranTimeDateGlobalData::systemCanNTPChanged,[this](bool can_ntp){
        m_autoSyncSwitch->setEnabled(can_ntp?true:false);
    });

    //获取ntp是否可开启
    bool can_ntp = globalData->systemCanNTP();
    m_autoSyncSwitch->setChecked(false);
    m_autoSyncSwitch->setEnabled(can_ntp);
    if(can_ntp){
        //时钟同步状态
        bool ntpStatus = globalData->systemNTP();
        m_autoSyncSwitch->setChecked(ntpStatus);
    }

    /// 保存
    Kiran::PropertyHelper::setButtonType(ui->btn_save,Kiran::BUTTON_Default);
    ui->btn_save->setFixedSize(252,60);
    connect(ui->btn_save,&QPushButton::clicked,[this](bool checked){
        bool bRes = true;
        QString error;
        if(ui->tabList->currentRow()==PAGE_TIMEZONE_SETTING){
            setMaskWidgetVisible(true);
            bRes = ui->timezone->save();
            setMaskWidgetVisible(false);
        }else if(ui->tabList->currentRow()==PAGE_DATETIME_SETTING){
            QDateTime dateTime;
            qint64 microsecondsSinceEpoch;
            dateTime.setDate(ui->widget_setDate->currentDate());
            dateTime.setTime(ui->widget_setTime->currentTime());
            microsecondsSinceEpoch = dateTime.toMSecsSinceEpoch()*1000;
            setMaskWidgetVisible(true);
            QPair<bool,QString> res = ComKylinsecKiranSystemDaemonTimeDateInterface::instance()->SyncSetTime(microsecondsSinceEpoch,false);
            setMaskWidgetVisible(false);
            bRes = res.first;
        }
    });

    /// 重置
    ui->btn_reset->setFixedSize(252,60);
    Kiran::PropertyHelper::setButtonType(ui->btn_reset,Kiran::BUTTON_Warning);
    connect(ui->btn_reset,&QPushButton::clicked,[this](bool checked){
        if(ui->tabList->currentRow()==PAGE_TIMEZONE_SETTING){
            ui->timezone->reset();
        }else{
            ui->widget_setDate->reset();
            ui->widget_setTime->reset();
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

void KiranTimeDateWidget::updateTimeLabel()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString displayDateTime = currentDateTime.toString(tr("ddd MMM HH:mm:ss yyyy"));
    ui->label_dateTime->setText(displayDateTime);
}

void KiranTimeDateWidget::updateTimeZoneLabel()
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
        QLocale systemLocale = QLocale::system();
        QString displayText = QString(tr("%1(%2)")).arg(city).arg(utc);
        ui->label_utc->setText(displayText);
    }else{
        ui->label_utc->setText("???");
    }
}

void KiranTimeDateWidget::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==m_updateTimer){
        updateTimeLabel();
    }else{
        QWidget::timerEvent(event);
    }
}

void KiranTimeDateWidget::setMaskWidgetVisible(bool visible)
{
    m_maskWidget->setVisible(visible);
    if( visible ){
        this->stackUnder(m_maskWidget);
    }
}
