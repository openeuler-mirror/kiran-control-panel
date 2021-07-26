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
    explicit ChooserWidget(QString title, int type = -1, QWidget *parent = 0);
    ~ChooserWidget();

    void setName(QString name);
private:
    void initUI(QString title);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *) ;

signals:
    void clicked();

private:
    Ui::ChooserWidget *ui;
    int m_wallpaperType = -1;
};

#endif // CHOOSERWIDGET_H
