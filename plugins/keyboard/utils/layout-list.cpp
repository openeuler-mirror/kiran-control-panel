/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "layout-list.h"
#include "layout-item.h"
#include "ui_layout-list.h"

#include <kiran-log/qt5-log-i.h>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <style-palette.h>

using namespace Kiran;

LayoutList::LayoutList(QWidget* parent) : QWidget(parent),
                                          ui(new Ui::LayoutList),
                                          m_editHasFocus(false)
{
    ui->setupUi(this);
    m_timer = new QTimer(this);
    ui->stackedWidget->setCurrentWidget(ui->page_list);
    ui->lineEdit_search->installEventFilter(this);
    ui->listWidget_filter->viewport()->setAutoFillBackground(false);
    ui->listWidget_list->viewport()->setAutoFillBackground(false);
    connect(m_timer, &QTimer::timeout,
            [this]
            {
                search();
                m_timer->stop();
            });

    connect(ui->lineEdit_search, &QLineEdit::textChanged,
            [this](QString text) {
                if (text.isEmpty())
                {
                    ui->stackedWidget->setCurrentWidget(ui->page_list);
                    if (m_timer->isActive())
                        m_timer->stop();
                    adjustSize();
                }
                else
                {
                    if (!m_timer->isActive())
                        m_timer->start();
                }
            });
    connect(this, &LayoutList::heightChanged,
            [this](int height) {
                setMaximumHeight(height +
                                 contentsMargins().top() +
                                 contentsMargins().bottom() +
                                 layout()->spacing() +
                                 2);
                updateGeometry();
            });
}

LayoutList::~LayoutList()
{
    delete ui;
}

void LayoutList::setCountryList(QStringList layoutList)
{
    m_lists = layoutList;
    for (int i = 0; i < m_lists.size(); i++)
    {
        LayoutItem* item = addItems(ui->listWidget_list, m_lists.at(i));
        connect(item, &LayoutItem::clicked, this, &LayoutList::itemClicked);
        connect(this, &LayoutList::itemChanged, item, &LayoutItem::selectedItemChanged);
    }
    adjustSize();
}

QString LayoutList::getSelectedCountry()
{
    return m_countryName;
}

bool LayoutList::editHasFocus() const
{
    return m_editHasFocus;
}

QSize LayoutList::sizeHint() const
{
    QSize hint;
    hint.setWidth(QWidget::sizeHint().width());
    hint.setHeight(this->maximumHeight());
    return hint;
}

void LayoutList::itemClicked()
{
    LayoutItem* item = dynamic_cast<LayoutItem*>(sender());

    QString countryName = item->getItemText();

    m_countryName = countryName;
    emit itemChanged(countryName);
}

void LayoutList::setEditHasFocus(bool editHasFocus)
{
    m_editHasFocus = editHasFocus;
    emit editHasFocusChanged(m_editHasFocus);
    style()->polish(this);
    ///NOTE:如果不重绘，聚焦边框样式不会更新
    update();
}

void LayoutList::search()
{
    clearFilterItems();
    ui->stackedWidget->setCurrentWidget(ui->page_filter);
    QString text = ui->lineEdit_search->text();

    foreach (QString list, m_lists)
    {
        if (list.contains(text))
        {
            LayoutItem* item = addItems(ui->listWidget_filter, list);
            m_filterList.append(list);
            connect(item, &LayoutItem::clicked, this, &LayoutList::itemClicked);
            connect(this, &LayoutList::itemChanged, item, &LayoutItem::selectedItemChanged);
        }
    }
    if (m_filterList.isEmpty())
    {
        QString tips = QObject::tr("No search results, please search again...");
        addItems(ui->listWidget_filter, tips);
        m_filterList.append(tips);
    }
    adjustSize();
}

void LayoutList::clearFilterItems()
{
    while (auto item = ui->listWidget_filter->takeItem(0))
    {
        delete item;
        item = nullptr;
    }
    m_filterList.clear();
}

LayoutItem* LayoutList::addItems(QListWidget* parent, QString text)
{
    QListWidgetItem* newItem = nullptr;
    LayoutItem* customItem = nullptr;

    newItem = new QListWidgetItem(parent);
    customItem = new LayoutItem(parent);

    newItem->setSizeHint(QSize(customItem->width(), 40));
    newItem->setTextAlignment(Qt::AlignVCenter);

    customItem->setItemText(text);
    parent->addItem(newItem);
    parent->setItemWidget(newItem, customItem);

    return customItem;
}

void LayoutList::adjustSize()
{
    int height = 0;
    if (ui->stackedWidget->currentWidget() == ui->page_list)
    {
        height = m_lists.size() * 40;
        ui->stackedWidget->setFixedHeight(height);
        emit heightChanged(height + ui->lineEdit_search->height());
    }
    else
    {
        height = m_filterList.size() * 40;
        ui->stackedWidget->setFixedHeight(height);
        emit heightChanged(height + ui->lineEdit_search->height());
    }
}

void LayoutList::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath painterPath;
    QRectF frect = rect();
    frect.adjust(0.5,0.5,-0.5,-0.5);
    painterPath.addRoundedRect(frect,6,6);

    auto stylePalette = StylePalette::instance();
    QColor borderColor = stylePalette->color(editHasFocus()?StylePalette::Checked:StylePalette::Normal,
                                             StylePalette::Widget,
                                             StylePalette::Border);

    p.setPen(borderColor);
    p.drawPath(painterPath);

    QWidget::paintEvent(event);
#if 0
    QStyleOption opt;
    //editHasFocus
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
#endif
}

bool LayoutList::eventFilter(QObject* obj, QEvent* event)
{
    ///NOTE: 通过event filter来获取输入框聚焦事件,修改样式为聚焦样式
    if (obj == ui->lineEdit_search)
    {
        switch (event->type())
        {
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

    return QWidget::eventFilter(obj, event);
}
