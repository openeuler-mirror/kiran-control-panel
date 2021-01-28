/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANMODULEWIDGET_H
#define KIRANMODULEWIDGET_H

#include "kiran-control-center-global.h"
using namespace KiranControlCenterGlobal;

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
    void setData(QMap<int, ModuleItem> *data);
    void setModuleCurSubItem(const QString &functionName);
    void setCurModuleSubRow(const int &row);
    bool checkHasUnSaved();
    void changeCurModuleSubItem(QListWidgetItem *current);

    void setDefaultSelectFirstItem(bool defaultSelectFirstItem);

public slots:
    void onSelectedClassItemChanged(QMap<int, ModuleItem> *modules);

private slots:
    void on_listWidget_module_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void closeCenterWidgetPlugin(QListWidgetItem *current);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::KiranModuleWidget *ui;
    //存储当前item和当前widget，用于切换时清除。
    QListWidgetItem *m_curItem;
    QWidget *m_curCenterWidget;
    //分类切换时，是否默认选中第一项。搜索的时候不需要默认选择，避免无效的切换。
    bool m_defaultSelectFirstItem;
};

#endif // KIRANMODULEWIDGET_H
