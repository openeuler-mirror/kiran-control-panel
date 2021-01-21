#include "kiran-display-configuration-window.h"
#include "kiran-display-configuration.h"
#include <QIcon>

KiranDisplayConfigurationWindow::KiranDisplayConfigurationWindow() : KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    setIcon(QIcon("://images/kiran-display-configuration-pane.svg"));
    setTitle(tr("Display Setting"));
    KiranDisplayConfiguration *centerWgt = new KiranDisplayConfiguration(this);
    setWindowContentWidget(centerWgt);
    connect(centerWgt, &KiranDisplayConfiguration::sigClose, this, &KiranDisplayConfigurationWindow::close);
    resize(708, 880);
}
