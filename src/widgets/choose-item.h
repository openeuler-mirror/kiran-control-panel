#ifndef CHOOSEITEM_H
#define CHOOSEITEM_H

#include <QWidget>

namespace Ui
{
class ChooseItem;
}

class ChooseItem : public QWidget
{
    Q_OBJECT

public:
    explicit ChooseItem(QString countryName, QWidget *parent = 0);
    ~ChooseItem();
    void setSelected(bool isSelected);
    void setDeleteMode(bool isDeleteMode);

signals:
    void deleteLayout();
    void clicked();

private:
    void initUI();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::ChooseItem *ui;
    QString m_countryName;
};

#endif  // CHOOSEITEM_H
