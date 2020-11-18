#include "kiran-mode-class-wgt.h"
#include <QEvent>
#include <QTimer>
#include <QButtonGroup>
#include <QDebug>

KiranModeClassWgt::KiranModeClassWgt(QWidget *parent) : QListWidget(parent),m_showText(false)
{
    installEventFilter(this);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAttribute(Qt::WA_Hover,true);
    setStyleSheet(styleSheetStr());
    initTimer();
    m_btnGroup = new QButtonGroup(this);
    setFixedWidth(iconModeWd());
    connect(this, &KiranModeClassWgt::currentItemChanged, this, &KiranModeClassWgt::onCurrentItemChanged);
}

void KiranModeClassWgt::setData(QMap<int, ModelClass> *data)
{
    QMapIterator<int, ModelClass> i(*data);
    while (i.hasNext()) {
        i.next();
        ModelClass &modeClass = (*data)[i.key()];
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole,  QVariant::fromValue((void *) &modeClass));
        item->setSizeHint(QSize(KiranModeClassItemWgt::iconModeWd(), KiranModeClassItemWgt::iconModeWd()));
        addItem(item);
        modeClass.item = item;
        KiranModeClassItemWgt *btn = new KiranModeClassItemWgt(this);
        btn->setText(modeClass.nameZh);
        btn->setIcon(modeClass.icon);
        btn->setToolTip(modeClass.commentF());
        setItemWidget(item, btn);
        m_btns.insert(item, btn);
    }
    setIconMode();
}

void KiranModeClassWgt::setIconMode()
{
    m_showText = false;
    m_timer->start();
}

void KiranModeClassWgt::setListMode()
{
    QHashIterator<QListWidgetItem *, KiranModeClassItemWgt *> i(m_btns);
    while (i.hasNext()) {
        i.next();
        i.value()->setTextVisible(true);
    }
    m_showText = true;
    m_timer->start();
}

QString KiranModeClassWgt::styleSheetStr()
{
    return  " QListWidget{"\
            "border: 0px;"\
            "border-top: 0px;"\
            "border-right: 1px solid rgba(255, 255, 255, 10);"\
            "border-bottom: 0px;"\
            "padding:20px;"\
            "outline:0px;"\
            "}"\
            "QListView::item{"\
            "border-radius:10px;"\
            "}";
}

void KiranModeClassWgt::initTimer()
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
                QHashIterator<QListWidgetItem *, KiranModeClassItemWgt *> i(m_btns);
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

int KiranModeClassWgt::iconModeWd()
{
    return KiranModeClassItemWgt::iconModeWd()+40+1;
}

int KiranModeClassWgt::textModeWd()
{
    return KiranModeClassItemWgt::textModeWd()+40+1;
}

void KiranModeClassWgt::onCurrentItemChanged(QListWidgetItem *item)
{
    QHashIterator<QListWidgetItem *, KiranModeClassItemWgt *> i(m_btns);
    while (i.hasNext()) {
        i.next();
        i.value()->setChecked(i.key() == item);
    }
}

bool KiranModeClassWgt::eventFilter(QObject * obj, QEvent * event)
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
