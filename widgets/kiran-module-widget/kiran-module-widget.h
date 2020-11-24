/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANMODULEWIDGET_H
#define KIRANMODULEWIDGET_H

#include "kiran-control-panel-global.h"
using namespace KiranControlPanelGlobal;

#include <QWidget>

namespace Ui {
class KiranModuleWidget;
}
class QListWidgetItem;
class KiranModeClassListWidget;
class KiranModuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranModuleWidget(QWidget *parent = 0);
    ~KiranModuleWidget();
    /*!
     * \brief KiranModuleWidget::setLeftContentsMargins 预留空间给模块分类列表窗口.
     * \param leftmargin 预留宽度,等于模块列表窗口的宽度.
     */
    void setLeftContentsMargins(const int &leftmargin);
    //多个模块数据合并。
    void setModulesData(QMap<int, ModuleItem> &data);
    void setCurModuleSubItem( QListWidgetItem *item);
    bool checkHasUnSaved();
    void changeCurModuleSubItem(QListWidgetItem *current);

public slots:
    void onSelectedClassItemChanged(QListWidgetItem *current);

private slots:
    void on_listWidget_item_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void closeCenterWidgetPlugin(QListWidgetItem *current);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::KiranModuleWidget *ui;
    QListWidgetItem *m_curItem;
    QWidget *m_curCenterWgt;
    bool m_retracement;
};

#endif // KIRANMODULEWIDGET_H
