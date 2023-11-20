#pragma once
#include <QWidget>

namespace Ui
{
class ListExpansionSpace;
}

class ListExpansionSpace : public QWidget
{
    Q_OBJECT

public:
    explicit ListExpansionSpace(QWidget *parent = nullptr);
    ~ListExpansionSpace() override;

signals:
    void addedListWidgetItem();
    void removedListWidgetItem();

public slots:
    void addListExpansionSpaceItem(QWidget *widget);
    void removeListExpansionSpaceCurrentItem();
    void removeListExpansionSpaceItem(int index);

private:
    void init();

private:
    Ui::ListExpansionSpace *ui;
};
