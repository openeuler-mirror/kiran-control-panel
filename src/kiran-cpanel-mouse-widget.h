#ifndef KIRANCPANELMOUSEWIDGET_H
#define KIRANCPANELMOUSEWIDGET_H

#include <QWidget>

namespace Ui {
class KiranCPanelMouseWidget;
}

class KiranCPanelMouseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranCPanelMouseWidget(QWidget *parent = 0);
    ~KiranCPanelMouseWidget();

private:
    Ui::KiranCPanelMouseWidget *ui;
};

#endif // KIRANCPANELMOUSEWIDGET_H
