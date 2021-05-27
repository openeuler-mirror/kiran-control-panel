/**
 * @file kiran-control-center-single-plugin.cpp
 * @brief description
 * @author yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 * @copyright (c) 2021 KylinSec. All rights reserved.
*/
#include "launcher.h"
#include "kiran-module-widget/kiran-module-widget.h"

#include <qt5-log-i.h>
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QIcon>
#include <QScreen>

Launcher::Launcher(QWidget *parent) :
    KiranTitlebarWindow(parent)
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    m_moduleWidget = new KiranModuleWidget(this);

    setWindowContentWidget(m_moduleWidget);
    installEventFilter(this);
}

Launcher::~Launcher()
{

}

void Launcher::setPlugins(const QList<QSharedPointer<CPanelPluginHelper>> &plugins)
{
    m_moduleWidget->setPlugins(plugins);
}

void Launcher::setPlugin(QSharedPointer<CPanelPluginHelper> plugin)
{
    QList<QSharedPointer<CPanelPluginHelper>> plugins = {plugin};
    m_moduleWidget->setPlugins(plugins);
}

QSize Launcher::sizeHint() const
{
    const float fontSize = QFontDatabase::systemFont(QFontDatabase::GeneralFont).pointSizeF();
    const QSize targetSize = QSize(qRound(100*fontSize),qRound(78*fontSize));
    const QSize screenSize = qApp->primaryScreen()->availableSize()*0.9;
    return targetSize.boundedTo(screenSize);
}