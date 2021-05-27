#include "panel-window.h"
#include "plugin-manager.h"
#include "panel-search-edit.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>

PanelWindow::PanelWindow(QWidget *parent) : KiranTitlebarWindow(parent)
{
    initUI();
}

void PanelWindow::initUI()
{
    //初始化窗口相关
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setIcon(QIcon::fromTheme(qAppName()));
    setTitle(tr("Control Panel"));

    //初始化中心显示窗口
    auto *centerWidget = new PanelWidget(this);
    setWindowContentWidget(centerWidget);

    //添加搜索框
    auto *searchBox = new CPanelSearchEdit(this);
    searchBox->setFixedSize(352, 40);
    getTitlebarCustomLayout()->addWidget(searchBox);
    setTitlebarCustomLayoutAlignHCenter(true);
}