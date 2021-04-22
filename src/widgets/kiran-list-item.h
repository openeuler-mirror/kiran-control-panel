#ifndef KIRANLISTITEM_H
#define KIRANLISTITEM_H

#include <QWidget>

namespace Ui {
class KiranListItem;
}

class KiranListItem : public QWidget
{
    Q_OBJECT

public:
    explicit KiranListItem(QWidget *parent = 0);
    ~KiranListItem();

public:
    void setText(QString text);
    void setItemArrow(bool isSelected);

private:
    Ui::KiranListItem *ui;
};

#endif // KIRANLISTITEM_H
