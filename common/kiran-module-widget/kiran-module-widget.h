/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANMODULEWIDGET_H
#define KIRANMODULEWIDGET_H

#include <QWidget>
#include "plugin-info.h"

namespace Ui
{
class KiranModuleWidget;
}

class QListWidgetItem;
class KiranModuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KiranModuleWidget(QWidget *parent = 0);
    ~KiranModuleWidget();

    void setLeftContentsMargins(const int &leftmargin);
    void clear();
    void setPlugins(const QList<QSharedPointer<CPanelPluginHelper>> &plugins);
    bool checkHasUnSaved();

private slots:
    void handleSubItemChanged(QListWidgetItem* current,QListWidgetItem* prev);

private:
    void init();

private:
    QList<QSharedPointer<CPanelPluginHelper>> m_plugins;
    Ui::KiranModuleWidget *ui;
    int m_currentSubItemIdx = -1;
    QWidget *m_subItemWidget = nullptr;
};

#endif  // KIRANMODULEWIDGET_H
