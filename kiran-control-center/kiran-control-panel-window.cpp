#include "kiran-control-panel-window.h"
#include "kiran-search-lineedit/kiran-search-lineedit.h"
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

    KiranControlPanel *centerWgt = new KiranControlPanel(this);
    setWindowContentWidget(centerWgt);

    QHBoxLayout *layout = getTitlebarCustomLayout();
    KiranSearchLineEdit *search = new KiranSearchLineEdit(centerWgt->completerKeys(), this);
    layout->addWidget(search);
    //绑定搜索槽函数
    connect(search, SIGNAL(sigSearch(QString)), centerWgt, SLOT(onSearch(QString)));

    resize(1060, 830);
}
