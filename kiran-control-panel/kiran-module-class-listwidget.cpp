/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-module-class-listwidget.h"
#include <QEvent>
#include <QTimer>
#include <QButtonGroup>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

KiranModuleClassListWidget::KiranModuleClassListWidget(QWidget *parent) : QListWidget(parent),m_showText(false)
{
    installEventFilter(this);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAttribute(Qt::WA_Hover,true);
    setStyleSheet(styleSheetStr());
    initTimer();
    m_btnGroup = new QButtonGroup(this);
    setFixedWidth(iconModeWd());

//    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
//    effect->setOffset(1, 1);          //设置向哪个方向产生阴影效果(dx,dy)，特别地，(0,0)代表向四周发散
//    effect->setColor("#333333");       //设置阴影颜色，也可以setColor(QColor(220,220,220))
//    effect->setBlurRadius(10);        //设定阴影的模糊半径，数值越大越模糊
//    setGraphicsEffect(effect);
//    setSpacing(cClassItemMargin);
}

void KiranModuleClassListWidget::setData(QMap<int, ModuleClass> *data)
{
    QMapIterator<int, ModuleClass> i(*data);
    while (i.hasNext()) {
        i.next();
        ModuleClass *moduleClass = &(*data)[i.key()];
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole,  QVariant::fromValue(moduleClass));
        item->setSizeHint(QSize(KiranModuleClassListWidgetItemWidget::iconModeWd(), KiranModuleClassListWidgetItemWidget::heightInt()));
        addItem(item);
        moduleClass->row = row(item);
        KiranModuleClassListWidgetItemWidget *itemWidget = new KiranModuleClassListWidgetItemWidget(this);
        itemWidget->setText(moduleClass->getNameTranslate());
        itemWidget->setIcon(moduleClass->icon);
        itemWidget->setToolTip(moduleClass->getCommentTranslate());//鼠标移上去就会展开,变为图片+文字模式,所以不存在只有icon的情况的tooltip.
        setItemWidget(item, itemWidget);
        m_btns.insert(item, itemWidget);//方便icon模式和text模式切换
    }
    setIconMode();

    if(count() <= 0) return;
    setCurrentRow(0);
}

void KiranModuleClassListWidget::setIconMode()
{
    m_showText = false;
    m_timer->start();
}

void KiranModuleClassListWidget::setListMode()
{
    QHashIterator<QListWidgetItem *, KiranModuleClassListWidgetItemWidget *> i(m_btns);
    while (i.hasNext()) {
        i.next();
        i.value()->setTextVisible(true);
    }
    m_showText = true;
    m_timer->start();
}

QString KiranModuleClassListWidget::styleSheetStr()
{
    return  QString(" QListWidget{"\
                    "border: 0px;"\
                    "border-top: 0px;"\
                    "border-right: 1px solid rgba(255, 255, 255, 20);"\
                    "border-bottom: 0px;"\
//                    "border-bottom-left-radius: 8px;"
                    "padding-left:%1px;"\
                    "padding-right:%1px;"\
                    "padding-top:12px;"\
                    "padding-bottom:12px;"\
                    "background-color:#333333;"\
                    "outline:0px;"\
                    "}"\
                    "QListView::item{"\
                    "border-radius:10px;"\
                    "}"\
                    "QListWidget::item:hover {"\
                    "background-color: rgba(255, 255, 255, 10);"\
                    "}"\
                    "QListWidget::item:selected {"\
                    "background-color:#43a3f2;"\
                    "}"
                    ).arg(cListWidgetPadding);
}
/*!
 * \brief KiranModuleClassListWidget::initTimer 模拟class列表展开的动画效果
 */
void KiranModuleClassListWidget::initTimer()
{
    m_step = (textModeWd()-iconModeWd());  //步长

    m_timer = new QTimer();
    m_timer->setInterval(1);
    QObject::connect(m_timer, &QTimer::timeout, [=](){
        if((!m_showText && width()<=iconModeWd()) || (m_showText && width() >= textModeWd()))
        {
            m_timer->stop();
            if(!m_showText)
            {
                QHashIterator<QListWidgetItem *, KiranModuleClassListWidgetItemWidget *> i(m_btns);
                while (i.hasNext()) {
                    i.next();
                    i.value()->setTextVisible(false);
                }
            }

            return;
        }

        setFixedWidth(width() + (m_showText ? m_step : -m_step));
    });
}
/*!
 * \brief KiranModuleClassListWidget::iconModeWd listWidget的总宽度,包括item的宽度+item的Space属性的宽度+listWidget的padding宽度.
 * \return
 */
int KiranModuleClassListWidget::iconModeWd()
{
    return KiranModuleClassListWidgetItemWidget::iconModeWd()+2*cListWidgetPadding+2*cClassItemMargin;
}

int KiranModuleClassListWidget::textModeWd()
{
    return KiranModuleClassListWidgetItemWidget::textModeWd()+2*cListWidgetPadding+2*cClassItemMargin;
}

bool KiranModuleClassListWidget::eventFilter(QObject * obj, QEvent * event)
{
    if(Q_LIKELY(obj == this))
    {
        switch (event->type()) {
        case QEvent::HoverEnter:
            setListMode();
            break;
        case QEvent::HoverLeave:
            setIconMode();
            break;
        default:
            break;
        }
    }

    return QListWidget::eventFilter(obj, event);
}
