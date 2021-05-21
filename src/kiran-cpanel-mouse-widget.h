/**
 * @file kiran-cpanel-mouse-widget.h
 * @brief  加载鼠标、触摸板设置页面
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#ifndef KIRANCPANELMOUSEWIDGET_H
#define KIRANCPANELMOUSEWIDGET_H

#include <QWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QStackedWidget>

namespace Ui {
class KiranCPanelMouseWidget;
}

class ComKylinsecKiranSessionDaemonMouseInterface;
class ComKylinsecKiranSessionDaemonTouchPadInterface;
class MouseSettings;
class TouchPadSettings;
class KiranSwitchButton;
class KiranListItem;
class KiranSwitchButton;
class KiranCPanelMouseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranCPanelMouseWidget(QWidget *parent = 0);
    ~KiranCPanelMouseWidget();

public:
    bool initUI();

private:
    void addSidebarItem(QString,QString);

public slots:
    void setCurrentPageWhenItemClicked(QListWidgetItem* item);

private:
    Ui::KiranCPanelMouseWidget *ui;
    QStackedWidget* m_stackedWidget;

    ComKylinsecKiranSessionDaemonTouchPadInterface *m_touchPadInterface;
    ComKylinsecKiranSessionDaemonMouseInterface *m_mouseInterface;

    MouseSettings* mouseSettingsPage;
    TouchPadSettings *touchPadSettingsPage;

    bool m_hasTouchPad = false;
};

#endif // KIRANCPANELMOUSEWIDGET_H
