/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANMODULECLASSLISTWIDGET_H
#define KIRANMODULECLASSLISTWIDGET_H

#include "kiran-control-panel-global.h"
#include "kiran-module-class-listwidget-itemwidget.h"
using namespace KiranControlPanelGlobal;

#include <QListWidget>
#include <QButtonGroup>

class KiranModuleClassListWidget : public QListWidget
{
    Q_OBJECT
    bool eventFilter(QObject *obj, QEvent *event);
public:
    explicit KiranModuleClassListWidget(QWidget *parent = nullptr);
    void setData(QMap<int, ModuleClass> *data);

signals:

public slots:
    void setIconMode(const bool &iconMode=true);
    int iconModeWd();
    int textModeWd();

private:
    QString styleSheetStr();
    QString styleExpandSheetStr();
    void initTimer();

private:
    QHash<QListWidgetItem *, KiranModuleClassListWidgetItemWidget *> m_btns;
    int m_step;
    bool m_showText;
    QButtonGroup *m_btnGroup;
};

#endif // KIRANMODULECLASSLISTWIDGET_H
