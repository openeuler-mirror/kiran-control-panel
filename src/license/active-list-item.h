#ifndef ACTIVELISTITEM_H
#define ACTIVELISTITEM_H

#include <QWidget>

namespace Ui {
class ActListItem;
}

class ActListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ActListItem(QWidget *parent = 0);
    ~ActListItem();

public:
    void iniUI();
    void setItemText(const QString &text);


private:
    Ui::ActListItem *ui;
};

#endif // ACTIVELISTITEM_H
