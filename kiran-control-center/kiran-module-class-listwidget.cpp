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
#include <QScrollBar>

KiranModuleClassListWidget::KiranModuleClassListWidget(QWidget *parent) : QListWidget(parent), m_showText(false)
{
    installEventFilter(this);
    setAttribute(Qt::WA_Hover,true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet(styleSheetStr());
    m_btnGroup = new QButtonGroup(this);
    setViewportMargins(cListWidgetLeftPadding,12,cListWidgetRightPadding,12);
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

    if(count() <= 0) return;
    setCurrentRow(0);
}

void KiranModuleClassListWidget::setIconMode(const bool &iconMode)
{
    iconMode ? setStyleSheet(styleSheetStr()) : setStyleSheet(styleExpandSheetStr());
}

void KiranModuleClassListWidget::setTextShow(const bool &showText)
{
    QHashIterator<QListWidgetItem *, KiranModuleClassListWidgetItemWidget *> i(m_btns);
    while (i.hasNext()) {
        i.next();
        i.value()->setTextVisible(showText);
    }
}

QString KiranModuleClassListWidget::styleExpandSheetStr()
{
    return  QString(" QListWidget{"\
                    "border: 0px;"\
                    "border-top: 0px;"\
                    "border-right: 1px solid rgba(255, 255, 255, 20);"\
                    "border-bottom: 0px;"\
                    //                    "border-bottom-left-radius: 8px;"
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
                    );
}

QString KiranModuleClassListWidget::styleSheetStr()
{
    return  QString(" QListWidget{"\
                    "border: 0px;"\
                    "border-top: 0px;"\
                    "border-right: 1px solid rgba(255, 255, 255, 20);"\
                    "border-bottom: 0px;"\
                    //                    "border-bottom-left-radius: 8px;"
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
                    );
}

void KiranModuleClassListWidget::resizeEvent(QResizeEvent *event)
{
    setViewportMargins(cListWidgetLeftPadding, 12, verticalScrollBar()->isVisible() ? (cListWidgetRightPadding-6) : cListWidgetRightPadding, 12);
    QListWidget::resizeEvent(event);
}
