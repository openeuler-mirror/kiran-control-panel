/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-search-lineedit.h"
#include "kiran-search-lineedit/kiran-search-popup-delegate.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QCompleter>
#include <QListView>
#include <QEvent>
#include <QStandardItemModel>

KiranSearchLineEdit::KiranSearchLineEdit(const QStringList &completerKeys, QWidget *parent) : QLineEdit(parent), m_completerKeys(completerKeys)
{
    setFixedSize(352, 40);
    setTextMargins(30, 0, 10, 0);

    QLabel *lock = new QLabel(this);
    lock->setMaximumSize(20, 20);
    lock->setCursor(QCursor(Qt::ArrowCursor));
    lock->setPixmap(QPixmap("://images/search.svg"));

    QSpacerItem *spaceItem = new QSpacerItem(322, 40, QSizePolicy::Expanding);
    QHBoxLayout *editLayout = new QHBoxLayout();
    editLayout->setContentsMargins(10, 0, 0, 0);
    editLayout->addWidget(lock);
    editLayout->addSpacerItem(spaceItem);
    setLayout(editLayout);

    setPlaceholderText(tr("search..."));
    QCompleter *completer = new QCompleter(m_completerKeys, this);
    //设置任意位置匹配
    completer->setFilterMode(Qt::MatchContains);
    completer->setCaseSensitivity(Qt::CaseSensitive);

    //popup窗口
    QListView *listView = new QListView();
    m_listView = listView;
    listView->installEventFilter(this);
    listView->setMouseTracking(true);
    //需要一个module,没有自带module.
    QStandardItemModel *m_proxyModel = new QStandardItemModel();
    listView->setModel(m_proxyModel);
    completer->setPopup(listView);
    //设置popup会重置delegate,所以需要后置delegate.
    KiranSearchPopupDelegate *delegate = new KiranSearchPopupDelegate();
    listView->setItemDelegate(delegate);
    setCompleter(completer);
    //textEdited信号在通过复制,键盘上下建等方式修改文本时不会被触发.
    connect(this, &QLineEdit::textEdited, this, [=](QString text){
        delegate->setSearchText(text);
        emit listView->model()->dataChanged(listView->model()->index(0, 0), listView->model()->index(listView->model()->rowCount(), 0));
    });

    connect(this, &KiranSearchLineEdit::editingFinished, this, [=](){emit sigSearch(text());});

    setStyleSheet("QLineEdit { color:#ffffff; border-radius: 6px; background: rgba(0,0,0,0);border: 1px solid rgba(255,255,255,20);}");;
}
//只有QListView *listView = new QListView();对象被监听
bool KiranSearchLineEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Show) {
        if(m_listView->pos().y() > mapToGlobal( this->pos()).y())
        {
            setStyleSheet("QLineEdit { color:#ffffff; border: 1px solid #43a3f2;border-bottom: 0px;background-color: rgba(255, 255, 255, 0);border-radius: 6px;border-bottom-right-radius: 0px;border-bottom-left-radius: 0px;}");

            QString style = "QAbstractItemView {border: 1px solid #43a3f2;border-top: 0px;border-bottom-right-radius: 6px;border-bottom-left-radius: 6px;background-color: #222222; color:#ffffff;}"\
                            "QAbstractItemView::item {padding: 0px 16px 0px 10px;height: 40px; border-top: 1px solid rgba(255, 255, 255, 30);}"\
                            "QAbstractItemView::item:hover{ background-color: rgba(255, 255, 255, 30);} " \
                            "QAbstractItemView::item:selected{ background-color: rgba(255, 255, 255, 50);} ";

            m_listView->setStyleSheet(style);
        }
        else
        {
            setStyleSheet("QLineEdit { color:#ffffff; border: 1px solid #43a3f2;border-top: 0px;background-color: rgba(255, 255, 255, 0);border-radius: 6px;border-top-right-radius: 0px;border-top-left-radius: 0px;}");

            QString style = "QAbstractItemView {border: 1px solid #43a3f2;border-bottom: 0px;border-top-right-radius: 6px;border-top-left-radius: 6px;background-color: #222222; color:#ffffff;}"\
                            "QAbstractItemView::item {padding: 0px 16px 0px 10px;height: 40px; border-top: 1px solid rgba(255, 255, 255, 30);}"\
                            "QAbstractItemView::item:hover{ background-color: rgba(255, 255, 255, 30);} " \
                            "QAbstractItemView::item:selected{ background-color: rgba(255, 255, 255, 50);} ";

            m_listView->setStyleSheet(style);
        }
        return true;
    }
    else if(event->type() == QEvent::Hide)
    {
        setStyleSheet("QLineEdit { color:#ffffff; border-radius: 6px; background: rgba(0,0,0,0);border: 1px solid rgba(255,255,255,20);}");
        return true;
    }
    else {
        return QObject::eventFilter(obj, event);
    }
}
