#include "kiran-time-zone-item.h"
#include "ui_kiran-time-zone-item.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

KiranTimeZoneItem::KiranTimeZoneItem(const ZoneInfo zoneInfo, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranTimeZoneItem),
    m_searchHighLightColor("#43a3f2"),
    m_isSelected(false),
    m_isResultTip(false)
{
    ui->setupUi(this);

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setFixedHeight(40);

    m_city = zoneInfo.zone_city;
    m_zoneID = zoneInfo.zone_id;

    int hour = qAbs(zoneInfo.zone_utc)/3600;
    int minute = (qAbs(zoneInfo.zone_utc)-hour*3600)/60;
    m_displayUTC = QString("UTC%1%2:%3").arg(zoneInfo.zone_utc>=0?"+":"-").arg(hour,2,10,QChar('0')).arg(minute,2,10,QChar('0'));

    ui->label->setText(QString("(%1) %2").arg(m_displayUTC).arg(m_city));
}

KiranTimeZoneItem::KiranTimeZoneItem(QWidget *parent):
    QWidget(parent),
    ui(new Ui::KiranTimeZoneItem),
    m_searchHighLightColor("#43a3f2"),
    m_isSelected(false),
    m_isResultTip(true)
{
    ui->setupUi(this);

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setFixedHeight(40);

    ui->label->setText(QString("<font color=\"gray\">%1</font>").arg(tr("No search results, please search again...")));
}

KiranTimeZoneItem::~KiranTimeZoneItem()
{
    delete ui;
}

QString KiranTimeZoneItem::getTimeZoneID()
{
    return m_zoneID;
}

QColor KiranTimeZoneItem::searchHighLightColor() const
{
    return m_searchHighLightColor;
}

bool KiranTimeZoneItem::isSelected() const
{
    return m_isSelected;
}

void KiranTimeZoneItem::setHeightLightKeyword(const QString &keyword)
{
    if(m_isResultTip){
        return;
    }
    QString city = m_city;
    QString hightLightKeyword = QString("<font color=\"%1\">%2</font>").arg(m_searchHighLightColor.name()).arg(keyword);
    city = city.replace(keyword,hightLightKeyword);
    ui->label->setText(QString("(%1) %3").arg(m_displayUTC).arg(city));
    m_keyword = keyword;
}

void KiranTimeZoneItem::unsetHeightLightKeyword()
{
    if(m_isResultTip){
        return;
    }

    ui->label->setText(QString("(%1) %3").arg(m_displayUTC).arg(m_city));
}

void KiranTimeZoneItem::setSearchHighLightColor(QColor searchHighLightColor)
{
    m_searchHighLightColor = searchHighLightColor;
}

void KiranTimeZoneItem::seletedZoneInfoChanged(const QString &zoneID)
{
    if(m_isResultTip){
        return;
    }

    if(zoneID==m_zoneID){
        setisSelected(true);
    }else{
        setisSelected(false);
    }
}

void KiranTimeZoneItem::setisSelected(bool isSelected)
{
    if (m_isSelected == isSelected)
        return;
    m_isSelected = isSelected;

    if(m_isSelected){///选中状态，取消掉关键词高亮
        unsetHeightLightKeyword();
    }else{///取消选中，若存在关键词则设置关键词高亮
        setHeightLightKeyword(m_keyword);
    }

    this->style()->polish(this);
    this->style()->polish(ui->icon);
    emit isSelectedChanged(m_isSelected);
}

void KiranTimeZoneItem::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton&&!m_isResultTip){
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void KiranTimeZoneItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}
