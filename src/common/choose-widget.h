#ifndef CHOOSEWIDGET_H
#define CHOOSEWIDGET_H

#include <QWidget>

namespace Ui {
class ChooseWidget;
}

class ChooseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChooseWidget(QString title,QWidget *parent = 0);
    ~ChooseWidget();

    void setName(QString name);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *) ;

signals:
    void clicked();

private:
    Ui::ChooseWidget *ui;
};

#endif // CHOOSEWIDGET_H
