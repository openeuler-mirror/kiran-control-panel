#ifndef CHOOSERWIDGET_H
#define CHOOSERWIDGET_H

#include <QWidget>

namespace Ui {
class ChooserWidget;
}

class ChooserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChooserWidget(QString title,QWidget *parent = 0);
    ~ChooserWidget();

    void setName(QString name);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *) ;

signals:
    void clicked();

private:
    Ui::ChooserWidget *ui;
};

#endif // CHOOSERWIDGET_H
