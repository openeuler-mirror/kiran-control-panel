#ifndef SYSTEMINFORMATIONWIDGET_H
#define SYSTEMINFORMATIONWIDGET_H

#include <QWidget>

namespace Ui {
class SystemInformationWidget;
}

class SystemInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemInformationWidget(QWidget *parent = 0);
    ~SystemInformationWidget();

private:
    void initUI();
    void readSystemInfo(int infoType);
    void getJsonValueFromString(QString jsonString);

private:
    Ui::SystemInformationWidget *ui;
};

#endif // SYSTEMINFORMATIONWIDGET_H
