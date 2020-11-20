#include "kiran-control-panel-window.h"
#include "kiran-control-panel.h"
#include "kiran-search-lineedit/kiran-search-popup-delegate.h"
#include <QHBoxLayout>
#include <QCompleter>
#include <QIcon>
#include <QLabel>
#include <QAbstractItemView>
#include <QListView>
#include <QStandardItemModel>
#include <QDebug>

KiranControlPanelWindow::KiranControlPanelWindow() : KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    setIcon(QIcon("://image/kiran-display-configuration-pane.png"));
    setTitle(tr("Control Center"));
    //
    QHBoxLayout *layout = getTitlebarCustomLayout();
    m_search = new QLineEdit(this);
    m_search->setFixedSize(352, 40);
    m_search->setTextMargins(30, 0, 10, 0);

       QLabel *lock = new QLabel(this);
       lock->setMaximumSize(20, 20);
       lock->setCursor(QCursor(Qt::ArrowCursor));
       lock->setPixmap(QPixmap("://image/search.svg"));

       QSpacerItem *spaceItem = new QSpacerItem(322, 40, QSizePolicy::Expanding);
       QHBoxLayout *editLayout = new QHBoxLayout();
       editLayout->setContentsMargins(10, 0, 0, 0);
       editLayout->addWidget(lock);
       editLayout->addSpacerItem(spaceItem);
       m_search->setLayout(editLayout);



//    QPixmap pixmap("://image/search.svg") ;
//    pixmap = pixmap.scaled(QSize(10,10));
//    QAction *actnew = new QAction(this);
//    actnew->setIcon(pixmap);

//    m_search->addAction(actnew, QLineEdit::LeadingPosition);

    m_search->setPlaceholderText(tr("search..."));
    layout->addWidget(m_search);
    //
    KiranControlPanel *centerWgt = new KiranControlPanel(this);
    setWindowContentWidget(centerWgt);

    QStringList wordList;
    wordList << centerWgt->completerKeys();
    QCompleter *completer = new QCompleter(wordList, this);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCaseSensitivity(Qt::CaseSensitive);

    QListView *listView = new QListView();
    listView->setMouseTracking(true);
    QStandardItemModel *m_proxyModel = new QStandardItemModel();
    listView->setModel(m_proxyModel);
     completer->setPopup(listView);

    KiranSearchPopupDelegate *delegate = new KiranSearchPopupDelegate();
    listView->setItemDelegate(delegate);

   // listView->addItem("账户");


    m_search->setCompleter(completer);
    connect(m_search, &QLineEdit::textChanged, this, [=](QString text){
        delegate->setSearchText(text);
        emit listView->model()->dataChanged(listView->model()->index(0, 0), listView->model()->index(listView->model()->rowCount(), 0));
    });
    connect(completer, SIGNAL(activated(QString)), centerWgt, SLOT(onSearch(QString)));

    QAbstractItemView *popup = completer->popup();
  //  qDebug() << "333" << listView << popup;
//       popup->setStyleSheet("  QAbstractItemView {background-color: rgba(255, 255, 255, 0);border: 4px solid darkgray;selection-background-color: lightgray;}");

    m_search->setStyleSheet("QLineEdit {"\
                  "color:#ffffff;"\
                  "border-radius: 6px;"\
                  "background: rgba(0,0,0,0);"\
                  " border: 1px solid rgba(255,255,255,20);"\
             " }");
    resize(1060, 730);


    QString style = "QAbstractItemView {background-color: rgba(255, 255, 255, 0); border: 1px solid #2eb3ff; color:#ffffff;}"\
                    "QAbstractItemView::item {padding: 0px 16px 0px 10px;height: 40px; border-top: 1px solid rgba(255, 255, 255, 10);}"\
                    " QAbstractItemView::item:selected  { background-color: #2eb3ff;} "\
                    " QAbstractItemView{border: 1px solid #2eb3ff;border-top: 0px;border-bottom-right-radius: 6px;border-bottom-left-radius: 6px;}"
//                    "QComboBox QAbstractScrollArea QScrollBar::handle:vertical {border-radius: 3px;background: rgba(0,0,0,0.1);width:20px;height:10px}"\
//                    "QComboBox QAbstractScrollArea QScrollBar::handle:vertical:hover {background: rgb(90, 91, 93);}"\
//                    "QComboBox QScrollBar::add-line::vertical{border:none;}"\
//                    "QComboBox QScrollBar::sub-line::vertical{border:none;}"
            ;
    //"QComboBox::down-arrow:on { image: url(://images/open_btn.png);}"
    popup->setStyleSheet(style);

}
