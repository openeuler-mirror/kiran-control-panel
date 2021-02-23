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
