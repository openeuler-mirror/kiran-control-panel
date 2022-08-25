/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-timedate is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "kiran-time-zone.h"
#include "ui_kiran-time-zone.h"
#include "kiran-time-zone-item.h"
#include "timedate-interface.h"
#include "kiran-timedate-global-data.h"

#include <style-palette.h>

#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QStyleOption>

using namespace Kiran;

KiranTimeZone::KiranTimeZone(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranTimeZone),
    m_editHasFocus(false)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    setAccessibleName("TimeZoneWidget");
    ui->edit_search->installEventFilter(this);
    initUI();;
}

KiranTimeZone::~KiranTimeZone()
{
    delete ui;
}

QSize KiranTimeZone::sizeHint() const
{
    QSize hint;
    hint.setWidth(QWidget::sizeHint().width());
    hint.setHeight(this->maximumHeight());
    return hint;
}

bool KiranTimeZone::save()
{
    if( KiranTimeDateGlobalData::instance()->systemTimeZone()==m_selectedZoneID ){
        return true;
    }

    QPair<bool,QString> res = ComKylinsecKiranSystemDaemonTimeDateInterface::instance()->SyncSetTimeZone(m_selectedZoneID);

    if(!res.first){
        qInfo() << "SetTimeZone failed," << res.second;
        return false;
    }

    return true;
}

void KiranTimeZone::reset()
{
    ui->edit_search->clear();
    ui->timeZoneList->reset();
    QTimer::singleShot(0,this,SLOT(scrollToCurrent()));
}

void KiranTimeZone::scrollToCurrent()
{
    QObjectList objList = ui->timeZoneList->allTimeZoneWidget()->children();
    foreach (QObject* obj, objList) {
        KiranTimeZoneItem* item = qobject_cast<KiranTimeZoneItem*>(obj);
        if(item&&item->getTimeZoneID()==m_selectedZoneID){
            int ymargin = ui->timeZoneList->height()/2 - 20;
            ui->scrollArea->ensureWidgetVisible(item,0,ymargin);
        }
    }
}

void KiranTimeZone::initUI()
{
    ui->label_search->setPixmap(QPixmap(":/kiran-control-panel/images/search.svg"));
    ui->edit_search->setPlaceholderText(tr("Search in all time zones..."));
    ui->scrollArea->setWidgetResizable(true);

    /// NOTE: 为了完成搜索项的数目变更，窗口高度自动适应的效果
    ///        通过设置最大高度setMaximumHeight,来将控件压缩到40px,不然始终占据过多空间
    /// 当滚动区域高度变化，调整该窗口最大高度
    connect(ui->timeZoneList,&KiranTimeZoneList::sigHeightChanged,[this](int height){
        setMaximumHeight(ui->widget_edit->height()+
                         height+
                         contentsMargins().top()+
                         contentsMargins().bottom()+
                         layout()->spacing()+
                         2);
        updateGeometry();
    });
    /// 搜索文本变化时，调用timeZone
    connect(ui->edit_search,&QLineEdit::textChanged,[this](const QString& text){
       if(text.isEmpty()){
           ui->timeZoneList->switchToAllTimeZone();
           QTimer::singleShot(0,this,SLOT(scrollToCurrent()));
       }else{
           ui->timeZoneList->addSearchTimeoutTask(text);
       }
    });

    connect(ui->timeZoneList,&KiranTimeZoneList::sigSeletedZoneInfoChanged,[this](const QString& zoneID){
        m_selectedZoneID = zoneID;
    });

    ui->timeZoneList->initAllTimeZone();
}

bool KiranTimeZone::event(QEvent *event)
{
    if(event->type()==QEvent::ShowToParent){
        scrollToCurrent();
    }
    return QWidget::event(event);
}

void KiranTimeZone::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if( m_editHasFocus )
    {
        opt.state |= QStyle::State_Selected;
    }

    auto kiranPalette = StylePalette::instance();
    auto borderColor = kiranPalette->color(m_editHasFocus?StylePalette::Checked:StylePalette::Normal,
                                           StylePalette::Widget,
                                           StylePalette::Border);

    QPainterPath painterPath;
    QRectF rectF = opt.rect;
    painterPath.addRoundedRect(rectF.adjusted(0.5,0.5,-0.5,-0.5),6,6);

    QPen pen;
    pen.setWidth(0);
    pen.setColor(borderColor);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(painterPath);

    QWidget::paintEvent(event);
}

bool KiranTimeZone::eventFilter(QObject *obj, QEvent *event)
{
    ///NOTE: 通过event filter来获取输入框聚焦事件,修改样式为聚焦样式
    if(obj==ui->edit_search){
        switch ( event->type() ) {
        case QEvent::FocusIn:
            m_editHasFocus = true;
            update();
            break;
        case QEvent::FocusOut:
            m_editHasFocus = false;
            update();
            break;
        case QEvent::FocusAboutToChange:
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(obj,event);
}

