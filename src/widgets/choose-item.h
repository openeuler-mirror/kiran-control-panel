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
    explicit ChooseItem(QWidget *parent = 0);
    ~ChooseItem();
    void setSelected(bool isSelected);
    void setEditMode(bool isEditMode);
    QString getLayoutName();
    void setNames(QString countryName, QString layoutName);

signals:
    void sigDelete(QString layoutName);
    void clicked();

private:
    void initUI();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

public slots:
    void seletedLayoutChanged(QString selectLayout);

private:
    Ui::ChooseItem *ui;
    QString m_layoutName;
    bool m_hover = false;
};

#endif  // CHOOSEITEM_H
