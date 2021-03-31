#include "kiran-control-center-window.h"
//#include "kiranwidgets-qt5/kiran-search-box.h"
#include "kiran-search-lineedit/kiran-search-lineedit.h"
#include <QHBoxLayout>
#include <QCompleter>
#include <QIcon>
#include <QLabel>
#include <QAbstractItemView>
#include <QListView>
#include <QStandardItemModel>

KiranControlCenterWindow::KiranControlCenterWindow(QWidget *parent) : KiranTitlebarWindow(parent)
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    setIcon(QIcon(APP_IMAGE_PATH));
    setTitle(tr("Control Center"));

    KiranControlCenter *centerWgt = new KiranControlCenter(this);
    setWindowContentWidget(centerWgt);

    QHBoxLayout *layout = getTitlebarCustomLayout();
    KiranSearchLineEdit *search = new KiranSearchLineEdit(centerWgt->completerKeys(), this);
    layout->addWidget(search);
    //绑定搜索槽函数
    connect(search, SIGNAL(sigSearch(QString)), centerWgt, SLOT(onSearch(QString)));
//    KiranSearchBox *search = new KiranSearchBox(this);
//    search->setFixedSize(352, 40);
//    layout->addWidget(search);
//    QCompleter *completer = new QCompleter(centerWgt->completerKeys(), this);
//    //设置任意位置匹配
//    completer->setFilterMode(Qt::MatchContains);
//    completer->setCaseSensitivity(Qt::CaseSensitive);
//    search->setCompleter(completer);
//    connect(completer, SIGNAL(activated(QString)), centerWgt, SLOT(onSearch(QString)));

    resize(1060, 830);
}
