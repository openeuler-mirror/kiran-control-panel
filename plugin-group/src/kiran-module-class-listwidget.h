/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANMODULECLASSLISTWIDGET_H
#define KIRANMODULECLASSLISTWIDGET_H

#include "kiran-control-center-global.h"
#include "kiran-module-class-listwidget-itemwidget.h"
using namespace KiranControlCenterGlobal;

#include <QListWidget>
#include <QButtonGroup>
class QGraphicsDropShadowEffect;

class KiranModuleClassListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit KiranModuleClassListWidget(QWidget *parent = nullptr);
    void setData(QMap<int, ModuleClass> *data);

signals:
    void sigShowShadow(const bool &show);

public slots:
    void setIconMode(const bool &iconMode=true);
    void setTextShow(const bool &showText);

private:
    QString styleSheetStr();
    QString styleExpandSheetStr();
    void initTimer();

private:
    QHash<QListWidgetItem *, KiranModuleClassListWidgetItemWidget *> m_btns;
    int m_step;
    bool m_showText;
    QButtonGroup *m_btnGroup;
    void resizeEvent(QResizeEvent *event);
};

#endif // KIRANMODULECLASSLISTWIDGET_H
