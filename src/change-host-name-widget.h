/**
 * @file change-host-name-widget.h
 * @brief 修改用户名接口
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef CHANGEHOSTNAMEWIDGET_H
#define CHANGEHOSTNAMEWIDGET_H

#include <QWidget>
#include <kiranwidgets-qt5/kiran-titlebar-window.h>

namespace Ui {
class ChangeHostNameWidget;
}

class ChangeHostNameWidget : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit ChangeHostNameWidget();
    ~ChangeHostNameWidget();
    bool getLineEditStatus();

private:
    void initUI();

signals:
    void sigChangeNameSuccessful(bool isChanged, QString name);

private slots:
    void setNewHostName();
    void setInputEditStatus();

private:
    Ui::ChangeHostNameWidget *ui;
};

#endif // CHANGEHOSTNAMEWIDGET_H
