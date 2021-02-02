#include "kiran-combobox.h"
#include <QListView>
#include <QScrollBar>
#include <QEvent>
#include <QDebug>

KiranComboBox::KiranComboBox(QWidget *parent) : QComboBox(parent)
{
    m_listView = new QListView(this);
    setView(m_listView);

    QString style = "QComboBox {border: 1px solid  rgba(255, 255, 255, 20);border-radius: 6px; padding: 0px 0px 0px 10px}"\
                    "QComboBox:hover {background-color: rgba(255, 255, 255, 5);}"\
                    "QComboBox {combobox-popup: 1;}"\
                    "QComboBox QAbstractItemView::item {padding: 0px 16px 0px 10px;height: 40px; border-top: 1px solid rgba(255, 255, 255, 10);}"\
                    "QComboBox QAbstractItemView::item:selected  { background-color: #2eb3ff;} "\
                    "QComboBox QAbstractItemView{border: 1px solid #2eb3ff;border-radius: 6px;}"\
                    "QComboBox::drop-down{border:0px;width:48px}" \
                    "QComboBox::down-arrow {image: url(:/images/icon_arrow.svg);}" \
                    "QComboBox::down-arrow:hover {image: url(:/images/icon_arrow.svg);}"\
                    "QComboBox::down-arrow:pressed {border-image: url(:/images/icon_arrow.svg);}" \
                    "QComboBox QAbstractScrollArea QScrollBar:vertical{width: 0px;background-color: #21a6ff;}"
            ;

    setStyleSheet(style);
    //"QComboBox::down-arrow:on { image: url(:/images/open_btn.png);}"

   // m_listView->installEventFilter(this);
}

bool KiranComboBox::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Show) {
        qDebug() << m_listView->mapToGlobal(QPoint(0, 0)) <<  this->mapToGlobal(this->pos());
        if(m_listView->mapToGlobal(m_listView->pos()).y() > this->mapToGlobal(this->pos()).y())
        {
            QString style = "QComboBox {border: 1px solid  rgba(255, 255, 255, 20);border-radius: 6px; padding: 0px 0px 0px 10px}"\
                            "QComboBox:hover {background-color: rgba(255, 255, 255, 5);}"\
                            "QComboBox {combobox-popup: 0;}"\
                            "QComboBox:on { border: 1px solid #2eb3ff;border-bottom: 0px;background-color: rgba(255, 255, 255, 0);border-bottom-right-radius: 0px;border-bottom-left-radius: 0px;}"\
                            "QComboBox QAbstractItemView::item {padding: 0px 16px 0px 10px;height: 40px; border-top: 1px solid rgba(255, 255, 255, 10);}"\
                            "QComboBox QAbstractItemView::item:selected  { background-color: #2eb3ff;} "\
                            "QComboBox QAbstractItemView{border: 1px solid #2eb3ff;border-top: 0px;border-bottom-right-radius: 6px;border-bottom-left-radius: 6px;}"\
                            "QComboBox::drop-down{border:0px;width:48px}" \
                            "QComboBox::down-arrow {image: url(:/images/icon_arrow.svg);}" \
                            "QComboBox::down-arrow:hover {image: url(:/images/icon_arrow.svg);}"\
                            "QComboBox::down-arrow:pressed {border-image: url(:/images/icon_arrow.svg);}" \
                            "QComboBox QAbstractScrollArea QScrollBar:vertical{width: 0px;background-color: #21a6ff;}"
                    ;

            setStyleSheet(style);
        }
        else
        {
            QString style = "QComboBox {border: 1px solid  rgba(255, 255, 255, 20);border-radius: 6px; padding: 0px 0px 0px 10px}"\
                            "QComboBox:hover {background-color: rgba(255, 255, 255, 5);}"\
                            "QComboBox {combobox-popup: 0;}"\
                            "QComboBox:on { border: 1px solid #2eb3ff;border-top: 0px;background-color: rgba(255, 255, 255, 0);border-top-right-radius: 0px;border-top-left-radius: 0px;}"\
                            "QComboBox QAbstractItemView::item {padding: 0px 16px 0px 10px;height: 40px; border-bottom: 1px solid rgba(255, 255, 255, 10);}"\
                            "QComboBox QAbstractItemView::item:selected  { background-color: #2eb3ff;} "\
                            "QComboBox QAbstractItemView{border: 1px solid #2eb3ff;border-bottom: 0px;border-top-right-radius: 6px;border-top-left-radius: 6px;}"\
                            "QComboBox::drop-down{border:0px;width:48px}" \
                            "QComboBox::down-arrow {image: url(:/images/icon_arrow.svg);}" \
                            "QComboBox::down-arrow:hover {image: url(:/images/icon_arrow.svg);}"\
                            "QComboBox::down-arrow:pressed {border-image: url(:/images/icon_arrow.svg);}" \
                            "QComboBox QAbstractScrollArea QScrollBar:vertical{width: 0px;background-color: #21a6ff;}"
                    ;

            setStyleSheet(style);
        }
    }
    else if(event->type() == QEvent::Hide)
    {
        QString style = "QComboBox {border: 1px solid  rgba(255, 255, 255, 20);border-radius: 6px; padding: 0px 0px 0px 10px}"\
                        "QComboBox:hover {background-color: rgba(255, 255, 255, 5);}"\
                        "QComboBox {combobox-popup: 0;}"\
                        "QComboBox:on { border: 1px solid #2eb3ff;border-bottom: 0px;background-color: rgba(255, 255, 255, 0);border-bottom-right-radius: 0px;border-bottom-left-radius: 0px;}"\
                        "QComboBox QAbstractItemView::item {padding: 0px 16px 0px 10px;height: 40px; border-top: 1px solid rgba(255, 255, 255, 10);}"\
                        "QComboBox QAbstractItemView::item:selected  { background-color: #2eb3ff;} "\
                        "QComboBox QAbstractItemView{border: 1px solid #2eb3ff;border-top: 0px;border-bottom-right-radius: 6px;border-bottom-left-radius: 6px;}"\
                        "QComboBox::drop-down{border:0px;width:48px}" \
                        "QComboBox::down-arrow {image: url(:/images/icon_arrow.svg);}" \
                        "QComboBox::down-arrow:hover {image: url(:/images/icon_arrow.svg);}"\
                        "QComboBox::down-arrow:pressed {border-image: url(:/images/icon_arrow.svg);}" \
                        "QComboBox QAbstractScrollArea QScrollBar:vertical{width: 0px;background-color: #21a6ff;}"
                ;

        setStyleSheet(style);
    }
    return QObject::eventFilter(obj, event);
}
