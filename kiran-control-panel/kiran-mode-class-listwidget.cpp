#include "kiran-mode-class-listwidget.h"
#include <QEvent>
#include <QTimer>
#include <QButtonGroup>
#include <QDebug>

KiranModeClassListWidget::KiranModeClassListWidget(QWidget *parent) : QListWidget(parent),m_showText(false)
{
    installEventFilter(this);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAttribute(Qt::WA_Hover,true);
    setStyleSheet(styleSheetStr());
    initTimer();
    m_btnGroup = new QButtonGroup(this);
    setFixedWidth(iconModeWd());
    connect(this, &KiranModeClassListWidget::currentItemChanged, this, &KiranModeClassListWidget::onCurrentItemChanged);
}

void KiranModeClassListWidget::setData(QMap<int, ModelClass> *data)
{
    QMapIterator<int, ModelClass> i(*data);
    while (i.hasNext()) {
        i.next();
        ModelClass &modeClass = (*data)[i.key()];
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole,  QVariant::fromValue((void *) &modeClass));
        item->setSizeHint(QSize(KiranModeClassListWidgetItemWidget::iconModeWd(), KiranModeClassListWidgetItemWidget::iconModeWd()));
        addItem(item);
        modeClass.item = item;
        KiranModeClassListWidgetItemWidget *btn = new KiranModeClassListWidgetItemWidget(this);
        btn->setText(modeClass.nameZh);
        btn->setIcon(modeClass.icon);
        btn->setToolTip(modeClass.commentF());
        setItemWidget(item, btn);
        m_btns.insert(item, btn);
    }
    setIconMode();
}

void KiranModeClassListWidget::setIconMode()
{
    m_showText = false;
    m_timer->start();
}

void KiranModeClassListWidget::setListMode()
{
    QHashIterator<QListWidgetItem *, KiranModeClassListWidgetItemWidget *> i(m_btns);
    while (i.hasNext()) {
        i.next();
        i.value()->setTextVisible(true);
    }
    m_showText = true;
    m_timer->start();
}

QString KiranModeClassListWidget::styleSheetStr()
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

void KiranModeClassListWidget::initTimer()
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
                QHashIterator<QListWidgetItem *, KiranModeClassListWidgetItemWidget *> i(m_btns);
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

int KiranModeClassListWidget::iconModeWd()
{
    return KiranModeClassListWidgetItemWidget::iconModeWd()+2*cListWidgetPadding;
}

int KiranModeClassListWidget::textModeWd()
{
    return KiranModeClassListWidgetItemWidget::textModeWd()+2*cListWidgetPadding;
}

void KiranModeClassListWidget::onCurrentItemChanged(QListWidgetItem *item)
{
    QHashIterator<QListWidgetItem *, KiranModeClassListWidgetItemWidget *> i(m_btns);
    while (i.hasNext()) {
        i.next();
        i.value()->setChecked(i.key() == item);
    }
}

bool KiranModeClassListWidget::eventFilter(QObject * obj, QEvent * event)
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
