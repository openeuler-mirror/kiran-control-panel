#ifndef KIRANTIMESETTINGWIDGET_H
#define KIRANTIMESETTINGWIDGET_H

#include <QWidget>

namespace Ui {
class KiranTimeSettingWidget;
}

class KiranTimeSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranTimeSettingWidget(QWidget *parent = nullptr);
    ~KiranTimeSettingWidget();

    void reset();

    QTime getTime();
private:
    void initUI();
protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    Ui::KiranTimeSettingWidget *ui;
};

#endif // KIRANTIMESETTINGWIDGET_H
