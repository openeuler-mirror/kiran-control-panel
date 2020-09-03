#include "kiran-time-zone.h"
#include "ui_kiran-time-zone.h"
#include "kiran-time-zone-item.h"
#include "SystemDaemonTimeDate.h"

#include <QStyleOption>
#include <QStyle>
#include <QPainter>
#include <QDebug>
#include <QMessageBox>

KiranTimeZone::KiranTimeZone(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranTimeZone),
    m_editHasFocus(false),
    m_hasChanged(false)
{
    ui->setupUi(this);

    ui->edit_search->installEventFilter(this);
    initUI();;
}

KiranTimeZone::~KiranTimeZone()
{
    delete ui;
}

bool KiranTimeZone::editHasFocus() const
{
    return m_editHasFocus;
}

QSize KiranTimeZone::sizeHint() const
{
    QSize hint;
    hint.setWidth(QWidget::sizeHint().width());
    hint.setHeight(this->maximumHeight());
    return hint;
}

bool KiranTimeZone::hasChanged() const
{
    return m_hasChanged;
}

bool KiranTimeZone::save()
{
    if( !m_hasChanged ){
        //TODO: 提示
        return true;
    }

    QPair<bool,QString> res = ComUnikylinKiranSystemDaemonTimeDateInterface::instance()->SyncSetTimeZone(m_selectedZoneID);

    if(!res.first){
        qInfo() << "SetTimeZone failed," << res.second;
        return false;
    }
    m_hasChanged = false;
    return true;
}

void KiranTimeZone::reset()
{
    ui->edit_search->clear();
    ui->timeZoneList->reset();
}

void KiranTimeZone::initUI()
{
    ui->edit_search->setPlaceholderText("在所有时区中搜索...");
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
       }else{
           ui->timeZoneList->addSearchTimeoutTask(text);
       }
    });

    connect(ui->timeZoneList,&KiranTimeZoneList::sigSeletedZoneInfoChanged,[this](const QString& zoneID){
        m_selectedZoneID = zoneID;
        m_hasChanged = true;
    });

    ui->timeZoneList->loadAllTimeZone();
}

void KiranTimeZone::setEditHasFocus(bool editHasFocus)
{
    m_editHasFocus = editHasFocus;
    emit editHasFocusChanged(m_editHasFocus);
    style()->polish(this);
    ///NOTE:如果不重绘，聚焦边框样式不会更新
    update();
}

void KiranTimeZone::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

bool KiranTimeZone::eventFilter(QObject *obj, QEvent *event)
{
    ///NOTE: 通过event filter来获取输入框聚焦事件
    ///      修改样式为聚焦样式

    if(obj==ui->edit_search){
        switch ( event->type() ) {
        case QEvent::FocusIn:
            setEditHasFocus(true);
            break;
        case QEvent::FocusOut:
            setEditHasFocus(false);
            break;
        case QEvent::FocusAboutToChange:
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(obj,event);
}
