#include "kiran-system-widget.h"
#include "kiran-system-information.h"

KiranSystemWidget::KiranSystemWidget(): KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    setTitle(tr("kiran-system-imformation"));
    setIcon(QIcon(":/images/kylin-about.png"));
    //setWindowModality(Qt::NonModal);

    centerWgt = new kiranSystemInformation;
    setWindowContentWidget(centerWgt);
    connect(centerWgt, &kiranSystemInformation::destroyed, this, &KiranSystemWidget::close);
}

KiranSystemWidget::~KiranSystemWidget()
{
    if(centerWgt != nullptr)
    {
       delete centerWgt;
    }
}
