#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPaintEvent>
#include <qzint.h>

using namespace Zint;

class QRCodeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QRCodeWidget(QWidget* parent,const QString &text);
    ~QRCodeWidget() override = default;

    const QString &getData();
    void setData(const QString &data_);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QZint zint;
    QString data;
};

#endif // MAINWINDOW_H
