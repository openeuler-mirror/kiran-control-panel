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

KiranSearchLineEdit::KiranSearchLineEdit(const QStringList &completerKeys, QWidget *parent)
    : QLineEdit(parent),
      m_completerKeys(completerKeys)
{
    init();
}

void KiranSearchLineEdit::init()
{
    initUI();
    initConnection();
    setStyleSheet("QLineEdit { color:#ffffff; border-radius: 6px; background: rgba(0,0,0,0);border: 1px solid rgba(255,255,255,20);}");;
}

void KiranSearchLineEdit::initUI()
{
    setFixedSize(352, 40);
    setTextMargins(30, 0, 10, 0);

    ///填充控件，设置布局
    auto *editLayout = new QHBoxLayout();
    setLayout(editLayout);
    editLayout->setContentsMargins(10, 0, 0, 0);
    auto *searchIconLabel = new QLabel(this);
    searchIconLabel->setMaximumSize(20, 20);
    searchIconLabel->setCursor(QCursor(Qt::ArrowCursor));
    searchIconLabel->setPixmap(QPixmap("://images/search.svg"));
    editLayout->addWidget(searchIconLabel);
    auto *spaceItem = new QSpacerItem(322, 40, QSizePolicy::Expanding);
    editLayout->addSpacerItem(spaceItem);

    /// 设置自动补全、自动补全弹出窗口
    setPlaceholderText(tr("search..."));
    QCompleter *completer = new QCompleter(m_completerKeys, this);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCaseSensitivity(Qt::CaseSensitive);
    setCompleter(completer);
    // 设置自动补全弹出窗口
    QListView *listView = new QListView(this);
    m_listView = listView;
    listView->installEventFilter(this);
    listView->setMouseTracking(true);
    // 设置自动补全弹窗模型
    QStandardItemModel *m_proxyModel = new QStandardItemModel();
    listView->setModel(m_proxyModel);
    completer->setPopup(listView);
    // 设置自动补全弹窗中listview的代理
    KiranSearchPopupDelegate *delegate = new KiranSearchPopupDelegate();
    listView->setItemDelegate(delegate);
}

void KiranSearchLineEdit::initConnection()
{
    /// 处理文本输入框变化
    connect(this,&QLineEdit::textChanged,this,&KiranSearchLineEdit::handleTextEdited);
    /// 处理当文本输入框按下回车事件
    connect(this,&QLineEdit::editingFinished,this,&KiranSearchLineEdit::handleEditingFinished);
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

/// 文本输入框变化时，设置代理搜索关键词，为了之后的补全列表弹窗关键词高亮绘制,发送模型数据变化信号，重新绘制
void KiranSearchLineEdit::handleTextEdited(QString text)
{
    auto* searchDelegate = qobject_cast<KiranSearchPopupDelegate*>(m_listView->itemDelegate());
    if( searchDelegate ){
        searchDelegate->setSearchText(text);
    }

    QModelIndex topLeftIdx = m_listView->model()->index(0,0);
    QModelIndex bottomRightIdx = m_listView->model()->index(m_listView->model()->rowCount(),0);
    emit m_listView->model()->dataChanged(topLeftIdx, bottomRightIdx);
}

/// 文本输入框按下回车时，发送搜索确认信号
void KiranSearchLineEdit::handleEditingFinished()
{
    emit sigSearchConfirmed(text());
}