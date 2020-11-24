/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-module-class-listwidget.h"
#include <QEvent>
#include <QTimer>
#include <QButtonGroup>
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
    connect(this, &KiranModuleClassListWidget::currentItemChanged, this, &KiranModuleClassListWidget::onCurrentItemChanged);
}

void KiranModuleClassListWidget::setData(QMap<int, ModuleClass> *data)
{
    QMapIterator<int, ModuleClass> i(*data);
    while (i.hasNext()) {
        i.next();
        ModuleClass &moduleClass = (*data)[i.key()];
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole,  QVariant::fromValue((void *) &moduleClass));
        item->setSizeHint(QSize(KiranModuleClassListWidgetItemWidget::iconModeWd(), KiranModuleClassListWidgetItemWidget::iconModeWd()));
        addItem(item);
        moduleClass.item = item;
        KiranModuleClassListWidgetItemWidget *btn = new KiranModuleClassListWidgetItemWidget(this);
        btn->setText(moduleClass.getNameTranslate());
        btn->setIcon(moduleClass.icon);
        btn->setToolTip(moduleClass.getCommentTranslate());
        setItemWidget(item, btn);
        m_btns.insert(item, btn);
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
                    "border-right: 1px solid rgba(255, 255, 255, 10);"\
                    "border-bottom: 0px;"\
                    "padding-left:%1px;"\
                    "padding-right:%1px;"\
                    "padding-top:14px;"\
                    "padding-bottom:14px;"\
                    "outline:0px;"\
                    "}"\
                    "QListView::item{"\
                    "border-radius:10px;"\
                    "}"\
                    "QListWidget::item:hover {"\
                    "background-color: rgba(255, 255, 255, 5);"\
                    "}"\
                    "QListWidget::item:selected {"\
                    "background-color:#43a3f2;"\
                    "}"
                    ).arg(cListWidgetPadding);
}

void KiranModuleClassListWidget::initTimer()
{
    m_step = 4;  //步长

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

int KiranModuleClassListWidget::iconModeWd()
{
    return KiranModuleClassListWidgetItemWidget::iconModeWd()+2*cListWidgetPadding;
}

int KiranModuleClassListWidget::textModeWd()
{
    return KiranModuleClassListWidgetItemWidget::textModeWd()+2*cListWidgetPadding;
}

void KiranModuleClassListWidget::onCurrentItemChanged(QListWidgetItem *item)
{
    QHashIterator<QListWidgetItem *, KiranModuleClassListWidgetItemWidget *> i(m_btns);
    while (i.hasNext()) {
        i.next();
        i.value()->setChecked(i.key() == item);
    }
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
