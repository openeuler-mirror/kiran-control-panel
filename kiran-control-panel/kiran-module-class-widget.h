/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANMODULECLASSWIDGET_H
#define KIRANMODULECLASSWIDGET_H

#include "kiran-module-class-listwidget.h"
#include <QWidget>

namespace Ui {
class KiranModuleClassWidget;
}

class KiranModuleClassWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranModuleClassWidget(QWidget *parent = 0);
    ~KiranModuleClassWidget();

    KiranModuleClassListWidget *m_listWidget;

        int iconModeWd();
private:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::KiranModuleClassWidget *ui;
    bool eventFilter(QObject *obj, QEvent *event);
    void setIconMode(const bool &iconMode);
    int textModeWd();

    bool m_showShadow;//是否有阴影
    int m_showShadowWidth;//阴影宽度
};

#endif // KIRANMODULECLASSWIDGET_H
