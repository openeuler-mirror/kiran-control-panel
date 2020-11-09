#include "kiran-combobox.h"
#include <QListView>
#include <QScrollBar>

KiranComboBox::KiranComboBox(QWidget *parent) : QComboBox(parent)
{
    setView(new QListView(this));
    QString style = "QComboBox {border: 1px solid  rgba(255, 255, 255, 20);border-radius: 6px; padding: 0px 0px 0px 10px}"\
                    "QComboBox:hover {background-color: rgba(255, 255, 255, 5);}"\
                    "QComboBox:on { border: 1px solid #2eb3ff;border-bottom: 0px;background-color: rgba(255, 255, 255, 0);border-bottom-right-radius: 0px;border-bottom-left-radius: 0px;}"\
                    "QComboBox QAbstractItemView::item {padding: 0px 16px 0px 10px;height: 40px; border-top: 1px solid rgba(255, 255, 255, 10);}"\
                    "QComboBox QAbstractItemView::item:selected  { background-color: #2eb3ff;} "\
                    "QComboBox QAbstractItemView{border: 1px solid #2eb3ff;border-radius: 6px;}"\
                    "QComboBox::drop-down{border:0px;width:48px}" \
                    "QComboBox::down-arrow {image: url(://images/icon_arrow.svg);}" \
                    "QComboBox::down-arrow:hover {image: url(://images/icon_arrow.svg);}"\
                    "QComboBox::down-arrow:pressed {border-image: url(://images/icon_arrow.svg);}" \
                    "QComboBox QAbstractScrollArea QScrollBar:vertical{width: 0px;background-color: #21a6ff;}"
            ;

//    QString style = "QComboBox {border: 1px solid  rgba(255, 255, 255, 20);border-radius: 6px; padding: 0px 0px 0px 10px}"\
//                    "QComboBox:hover {background-color: rgba(255, 255, 255, 5);}"\
//                    "QComboBox:on { border: 1px solid #2eb3ff;border-bottom: 0px;background-color: rgba(255, 255, 255, 0);border-bottom-right-radius: 0px;border-bottom-left-radius: 0px;}"\
//                    "QComboBox QAbstractItemView::item {padding: 0px 16px 0px 10px;height: 40px; border-top: 1px solid rgba(255, 255, 255, 10);}"\
//                    "QComboBox QAbstractItemView::item:selected  { background-color: #2eb3ff;} "\
//                    "QComboBox QAbstractItemView{border: 1px solid #2eb3ff;border-top: 0px;border-bottom-right-radius: 6px;border-bottom-left-radius: 6px;}"\
//                    "QComboBox::drop-down{border:0px;width:48px}" \
//                    "QComboBox::down-arrow {image: url(://images/icon_arrow.svg);}" \
//                    "QComboBox::down-arrow:hover {image: url(://images/icon_arrow.svg);}"\
//                    "QComboBox::down-arrow:pressed {border-image: url(://images/icon_arrow.svg);}" \
//                    "QComboBox QAbstractScrollArea QScrollBar:vertical{width: 0px;background-color: #21a6ff;}"\
////                    "QComboBox QAbstractScrollArea QScrollBar::handle:vertical {border-radius: 3px;background: rgba(0,0,0,0.1);width:20px;height:10px}"\
////                    "QComboBox QAbstractScrollArea QScrollBar::handle:vertical:hover {background: rgb(90, 91, 93);}"\
////                    "QComboBox QScrollBar::add-line::vertical{border:none;}"\
////                    "QComboBox QScrollBar::sub-line::vertical{border:none;}"
//            ;
    //"QComboBox::down-arrow:on { image: url(://images/open_btn.png);}"
    setStyleSheet(style);
}
