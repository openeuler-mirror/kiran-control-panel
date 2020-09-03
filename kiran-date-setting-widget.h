#ifndef KIRANDATESETTINGWIDGET_H
#define KIRANDATESETTINGWIDGET_H

#include <QWidget>

namespace Ui {
class KiranDateSettingWidget;
}

class KiranDateSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranDateSettingWidget(QWidget *parent = nullptr);
    ~KiranDateSettingWidget();

    void reset();
    QDate getDate();
private:
    void initUI();
protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    Ui::KiranDateSettingWidget *ui;
};

#endif // KIRANDATESETTINGWIDGET_H
