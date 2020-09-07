#include "kiran-time-zone-list.h"
#include "ui_kiran-time-zone-list.h"
#include "kiran-time-date-data.h"

#include <QTimerEvent>
#include <QDebug>
#include <QTimer>
#include <QtMath>

#define SEARCH_TIMEOUT 150 //ms

KiranTimeZoneList::KiranTimeZoneList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranTimeZoneList),
    m_timeoutSearchTaskID(0)
{
    ui->setupUi(this);

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}

KiranTimeZoneList::~KiranTimeZoneList()
{
    delete ui;
}

bool KiranTimeZoneList::initAllTimeZone()
{
    ZoneInfo info;
    KiranTimeZoneItem *item = nullptr;

    auto timeDateInterface = ComUnikylinKiranSystemDaemonTimeDateInterface::instance();
    m_seletedZoneID = timeDateInterface->time_zone();

    switchToAllTimeZone();
    auto iter = KiranTimeDateData::instance()->timeZoneListStart();
    while(iter!=KiranTimeDateData::instance()->timeZoneListEnd()){
        item = new KiranTimeZoneItem(*iter,ui->timezone);
        if(iter->zone_id==m_seletedZoneID){
            item->setisSelected(true);
        }
        /// 当item点击，发送信号，连接到该窗口，处理保存选中的item，并发送sigSeletedZoneInfoChanged信号给所有item
        connect(item,&KiranTimeZoneItem::clicked,
                this,&KiranTimeZoneList::slotTimeZoneItemClicked);
        connect(this,&KiranTimeZoneList::sigSeletedZoneInfoChanged,
                item,&KiranTimeZoneItem::seletedZoneInfoChanged);

        ui->timezone->layout()->addWidget(item);
        iter++;
    }

    return true;
}

void KiranTimeZoneList::switchToAllTimeZone()
{
    ui->timezone->setVisible(true);
    ui->filter_timezone->setVisible(false);
    if(m_timeoutSearchTaskID!=0){
        killTimer(m_timeoutSearchTaskID);
    }
    m_keyword.clear();
    adjustHeight();
}

void KiranTimeZoneList::addSearchTimeoutTask(const QString &keyword)
{
    if(m_timeoutSearchTaskID!=0){
        killTimer(m_timeoutSearchTaskID);
    }
    m_keyword = keyword;
    m_timeoutSearchTaskID = startTimer(SEARCH_TIMEOUT);
}

void KiranTimeZoneList::reset()
{
    m_seletedZoneID = ComUnikylinKiranSystemDaemonTimeDateInterface::instance()->time_zone();
    emit sigSeletedZoneInfoChanged(m_seletedZoneID);
}

void KiranTimeZoneList::search()
{
    ui->filter_timezone->setVisible(true);
    ui->timezone->setVisible(false);

    cleanFilterTimeZoneWidget();

    KiranTimeZoneItem *item = nullptr;
    auto iter = KiranTimeDateData::instance()->timeZoneListStart();
    while(iter!=KiranTimeDateData::instance()->timeZoneListEnd()){
        if(iter->zone_city.contains(m_keyword) ){
            item = new KiranTimeZoneItem(*iter,ui->filter_timezone);
            item->setHeightLightKeyword(m_keyword);
            if(iter->zone_id==m_seletedZoneID){
                item->setisSelected(true);
            }

            connect(item,&KiranTimeZoneItem::clicked,
                    this,&KiranTimeZoneList::slotTimeZoneItemClicked);
            connect(this,&KiranTimeZoneList::sigSeletedZoneInfoChanged,
                    item,&KiranTimeZoneItem::seletedZoneInfoChanged);

            ui->filter_timezone->layout()->addWidget(item);
            m_filtedZoneInfoList.append(*iter);
        }
        iter++;
    }

    if( m_filtedZoneInfoList.size()==0 ){
        item = new KiranTimeZoneItem(ui->filter_timezone);
        ui->filter_timezone->layout()->addWidget(item);
    }

    adjustHeight();
}

void KiranTimeZoneList::slotTimeZoneItemClicked()
{
    KiranTimeZoneItem* item = dynamic_cast<KiranTimeZoneItem*>(sender());
    QString zoneID = item->getTimeZoneID();

    if(zoneID!=m_seletedZoneID){
        m_seletedZoneID = zoneID;
        emit sigSeletedZoneInfoChanged(zoneID);
    }
}

void KiranTimeZoneList::adjustHeight()
{
    int height = 0;

    if(ui->timezone->isVisible()){
        height = KiranTimeDateData::instance()->timeZoneListSize()*40;
        setFixedHeight(height);
        emit sigHeightChanged(height);
    }else if(ui->filter_timezone->isVisible()){
        height = m_filtedZoneInfoList.size()==0?40:m_filtedZoneInfoList.size()*40;
        setFixedHeight(height);
        emit sigHeightChanged(height);
    }
}

void KiranTimeZoneList::cleanFilterTimeZoneWidget()
{
    while(auto iter=ui->filter_timezone->layout()->takeAt(0)){
        iter->widget()->setParent(nullptr);
        delete iter;
    }
    m_filtedZoneInfoList.clear();
}

void KiranTimeZoneList::rebuildFilterTimeZoneWidget()
{

}

void KiranTimeZoneList::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId()==m_timeoutSearchTaskID){
        killTimer(m_timeoutSearchTaskID);
        m_timeoutSearchTaskID = 0;
        search();
    }
}
