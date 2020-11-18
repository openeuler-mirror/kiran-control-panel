#include "kiran-control-panel-window.h"
#include "kiran-control-panel.h"
#include <QHBoxLayout>
#include <QCompleter>
#include <QIcon>
#include <QDebug>

KiranControlPanelWindow::KiranControlPanelWindow() : KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    setIcon(QIcon("://images/kiran-display-configuration-pane.svg"));
    setTitle(tr("Control Center"));
    //
    QHBoxLayout *layout = getTitlebarCustomLayout();
    m_search = new QLineEdit(this);
    m_search->addAction(QIcon("/root/kiran-control-panel/image/test.png"),QLineEdit::LeadingPosition);

    m_search->setPlaceholderText(tr("search..."));
    layout->addWidget(m_search);
    //
    KiranControlPanel *centerWgt = new KiranControlPanel(this);
    setWindowContentWidget(centerWgt);

    QStringList wordList;
    wordList << centerWgt->completerKeys();
    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_search->setCompleter(completer);
    connect(completer, SIGNAL(activated(QString)), centerWgt, SLOT(onSearch(QString)));

    resize(708, 830);
}
