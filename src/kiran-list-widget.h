#ifndef KIRANLISTWIDGET_H
#define KIRANLISTWIDGET_H

#include <QListWidget>

class KiranListWidget :public QListWidget
{
public:
    KiranListWidget(QWidget* parent=nullptr);
    ~KiranListWidget() = default;

    virtual QSize sizeHint() const override;
};

#endif // KIRANLISTWIDGET_H
